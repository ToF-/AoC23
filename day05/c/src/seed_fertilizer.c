#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <limits.h>
#include "seed_fertilizer.h"
#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a > b ? a : b)
#define DEBUG

int scan_int(unsigned long *, int, char *);
int scan_values(unsigned long *, char *);

int scan_ints(unsigned long *dest, int max_to_scan, char *line) {
    int count = 0;
    char *p = line;
    while(count < max_to_scan && *p) {
        char *end;
        unsigned long l = strtol(p, &end, 10);
        if(end == p) {
            return count;
        }
        dest[count++] = l;
        p = end;
        while(*p && !isdigit(*p)) p++;
    }
    return count;
}

int scan_values(unsigned long *dest, char *line) {
    char *p = strchr(line, ' ');
    return scan_ints(dest, MAXVALUES, p);
}

Converter scan_converter(char *line) {
    Converter result;
    unsigned long values[3];
    if(scan_ints(values, 3, line)) {
        result.dest = values[0];
        result.range.start = values[1];
        result.range.len = values[2];
        return result;
    }
    return result;
}

bool has_range(RangeSet *set, Range range) {
    for(int i=0; i<set->count; i++) {
        Range r = set->item[i];
        if(r.start == range.start && r.len == range.len)
            return true;
    }
    return false;
}
void add_range(RangeSet *set, Range range) {
    if(has_range(set, range))
        return;

    assert(set->count < MAXRANGES);
    set->item[set->count++] = range;
}

unsigned long range_end(Range range) {
    return range.start + range.len - 1;
}

ConverterSet new_ConverterSet() {
    ConverterSet result;
    result.count = 0;
    return result;
}

RangeSet new_RangeSet() {
    RangeSet result;
    result.count = 0;
    return result;
}
void add_converter(ConverterSet *set, Converter converter) {
    set->item[set->count++] = converter;
}

void read_almanach(Almanach *dest, char *filename) {
    char **lines;
    char line[MAXLINE];
    int lineCount = 0;
    FILE *file = fopen(filename, "r");
    if(!file) {
        perror("can't open file");
        exit(1);
    }
    while(fgets(line, MAXLINE, file) != NULL) {
        lineCount++;
    }
    lines = (char **)malloc((lineCount + 1)* sizeof(char *));
    if(lines == NULL) {
        perror("memory allocation error");
        fclose(file);
        exit(1);
    }
    rewind(file);
    for(int i=0; i<lineCount; i++) {
        fgets(line, MAXLINE, file);
        line[strcspn(line, "\n")] = 0;
        lines[i] = (char *)malloc(strlen(line)+1);
        if(lines[i] == NULL) {
            perror("memory allocation error");
            for(int j=0; j<i; j++)
                free(lines[j]);
            free(lines);
            fclose(file);
            exit(1);
        }
        strcpy(lines[i], line);
    }
    lines[lineCount] = (char *)malloc(strlen("")+1);
    strcpy(lines[lineCount],"");
    lineCount++;
    fclose(file);
    dest->valueCount = scan_values(dest->values, lines[0]);
    for(int i=0; i<dest->valueCount; i++) {
        if(i%2) {
            Range range;
            range.start = dest->values[i-1];
            range.len = dest->values[i];
            add_range(&dest->seedRanges, range);
        }
    }
    int mapCount = 0;
    for(int i=1; i<lineCount; i++) {
        char *line = lines[i];
        if(strlen(line) == 0 && i > 2) {
            mapCount++;
        } else if(strchr(line, ':')) {
            dest->maps[mapCount].count = 0;
        } else {
            ConverterSet *set = &dest->maps[mapCount];
            add_converter(set, scan_converter(line));
        }
    }
    for(int i=0; i<lineCount; i++) {
        free(lines[i]);
    }
}

// convert a range via a converter, producing two sets of ranges:
// converted : the intersection between range and converter, mapped to the converter offset
// remaining : the ranges that don't intersect with the converter
void convert_range(RangeSet *converted, RangeSet *remaining, Range range, Converter converter) {
    Range inter;

    inter.start = MAX(range.start, converter.range.start);
    unsigned long inter_end = MIN(range_end(range), range_end(converter.range));
    inter.len = inter_end > inter.start ? inter_end - inter.start + 1 : 0;

    if(inter.len == 0) {
        add_range(remaining, range);
        return;
    }

    if(range.start < inter.start) {
        Range before;
        before.start = range.start;
        before.len = inter.start - before.start;
        add_range(remaining, before);
    }
    Range convert;
    convert.start = inter.start + converter.dest - converter.range.start;
    convert.len = inter.len;
    add_range(converted, convert);
    if(range_end(range) > range_end(inter)) {
        Range after;
        after.start = inter.start + inter.len;
        after.len = range.start + range.len - after.start;
        add_range(remaining, after);
    }
}

// R0 C0-> R01,R02,R03 
// R01 C1 => R011 R012 R013
// R02 C1 => R021 R022 R023
// R03 C1 => R031 R032 R033
//
// convert first range through first converter 
// if conversion -> call 

void map_convert_ranges_all_map_level(RangeSet *result, RangeSet *current, int level, Almanach *almanach) {
#ifdef DEBUG
    printf("level:%d\n",level);
    printf("current:\n");
    print_ranges(current);
    printf("result size:%d\n", result->count);
#endif
    if(level >= MAXMAPS) {
        append_ranges(result, current);
        return;
    }
    ConverterSet converters = almanach->maps[level];
    RangeSet *work = (RangeSet *)malloc(sizeof(RangeSet));
    RangeSet *temp = (RangeSet *)malloc(sizeof(RangeSet));
    copy_ranges(work, current);
    for(int r=0; r<work->count; r++) {
        Range range = work->item[r];
        map_convert_range(temp, range, &converters);
        map_convert_ranges_all_map_level(result, temp, level+1, almanach);
    }
    free(temp);
    free(work);
}
// convert a range through a set of converter, each remaining range from a conversion
// being processed with the next converter in the set
// all remaining ranges after the next converter is processed are added to the result
void map_convert_range(RangeSet *result, Range range, ConverterSet *converters) {
    RangeSet *remaining = (RangeSet *)malloc(sizeof(RangeSet));
    RangeSet *work =      (RangeSet *)malloc(sizeof(RangeSet));
    empty_ranges(remaining);
    empty_ranges(result);
    empty_ranges(work);
    add_range(work, range);
    for(int c=0; c<converters->count; c++) {
#ifdef DEBUG
        printf("ranges to convert\n"); print_ranges(work);
        printf("current converter:\t"); print_converter(converters->item[c]); printf("\n");
#endif
        for(int w=0; w<work->count; w++) {
            convert_range(result, remaining, work->item[w], converters->item[c]);
        }
        copy_ranges(work, remaining);
        empty_ranges(remaining);
    }
    append_ranges(result, work);
    free(remaining);
    free(work);
}

// convert a range through all 7 sets of converters
void map_convert_range_all_maps(RangeSet *result, Range range, ConverterSet *maps) {
    RangeSet *work = (RangeSet *)malloc(sizeof(RangeSet));
    RangeSet *temp = (RangeSet *)malloc(sizeof(RangeSet));
    empty_ranges(work);
    add_range(work, range);
    for(int m = 0; m < 7; m++) {
#ifdef DEBUG
        printf("################# map#%d\n", m);
#endif
        ConverterSet converters = maps[m];
        empty_ranges(temp);
        for(int r = 0; r < work->count; r++) {
            map_convert_range(temp, work->item[r], &converters);
#ifdef DEBUG
        printf("------------------ map#%d done with result:\n", m);
        print_ranges(temp);
        getchar();
#endif
            copy_ranges(work, temp);
        }
    }
    copy_ranges(result, work);
}

void print_range(Range range) {
    printf("%lu %lu [%lu..%lu]\n",
            range.start, range.len,
            range.start, range_end(range));
}
void print_converter(Converter converter) {
    printf("%lu %lu %lu [%lu..%lu] -> [%lu..%lu]\n",
            converter.dest,
            converter.range.start, converter.range.len,
            converter.range.start, range_end(converter.range),
            converter.dest, converter.dest + converter.range.len - 1);
}

void all_maps_range(RangeSet *result, Range range, Almanach *almanach) {
    RangeSet source= new_RangeSet();
    add_range(&source, range);
    for(int i = 0; i < 7; i++) {
        ConverterSet converters = almanach->maps[i];
        RangeSet dest = new_RangeSet();
        for(int j=0; j<source.count; j++) {
            map_convert_range(&dest, source.item[j], &converters);
        }
        copy_ranges(&source, &dest);
    }
    result->count = 0;
    append_ranges(result, &source);
}

void empty_ranges(RangeSet *dest) {
    dest->count = 0;
}

void append_ranges(RangeSet *dest, RangeSet *srce) {
    for(int i = 0; i < srce->count; i++) {
        add_range(dest, srce->item[i]);
    }
}

void copy_ranges(RangeSet *dest, RangeSet *srce) {
    empty_ranges(dest);
    append_ranges(dest, srce);
}

unsigned long minimum_all_maps_ranges(Almanach *almanach) {
    unsigned long min = ULONG_MAX;
    RangeSet *source = (RangeSet *)malloc(sizeof(RangeSet));
    RangeSet *work= (RangeSet *)malloc(sizeof(RangeSet));
    for(int i = 0; i < almanach->seedRanges.count; i++) {
        printf("seed range %d:",i); print_range(almanach->seedRanges.item[i]); printf("\n");
        source->count = 0;
        add_range(source, almanach->seedRanges.item[i]);
        for(int j = 0; j < MAXMAPS; j++) {
            printf("map %d\n",j); print_ranges(source); printf("\n");
            ConverterSet converters = almanach->maps[j];
            printf("converters\n");
            print_converter_set(&converters);
            work->count = 0;
            for(int k = 0; k < source->count; k++) {
                Range range = source->item[k];
                map_convert_range(work, range, &converters);
            }
            source->count = 0;
            append_ranges(source, work);
        }
        min = MIN(minimum(source), min);
        printf("%lu\n", min);
    }
    free(source);
    free(work);
    return min;
}

unsigned long minimum(RangeSet *ranges) {
    unsigned long min = ULONG_MAX;
    for(int i=0; i < ranges->count; i++) {
        min = MIN(ranges->item[i].start, min);
    }
    return min;
}

void print_ranges(RangeSet *set) {
    for(int i = 0; i < set->count; i++) {
        print_range(set->item[i]);
    }
    printf("\n");
}

void print_converter_set(ConverterSet *set) {
    for(int i = 0; i < set->count; i++) {
        print_converter(set->item[i]);
    }
    printf("\n");
}
