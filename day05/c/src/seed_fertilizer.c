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

Almanach *new_almanach() {
    Almanach *almanach = (Almanach *)malloc(sizeof(Almanach));
    almanach->values = (unsigned long *)malloc(sizeof(unsigned long)*MAXVALUES);
    almanach->seedRanges = new_range_set();
    almanach->maps = (ConverterSet *)malloc(sizeof(ConverterSet) * MAXMAPS);
    for(int m=0; m < MAXMAPS; m++) {
        almanach->maps[m].count = 0;
        almanach->maps[m].items = (Converter *)malloc(sizeof(Converter)*MAXCONVERTERS);
    }
    almanach->minimumSeed = ULONG_MAX;
    return almanach;
}

void destroy_almanach(Almanach *almanach) {
    free(almanach->values);
    destroy_range_set(almanach->seedRanges);
    for(int m=0; m < MAXMAPS; m++) {
        free(almanach->maps[m].items);
    }
    free(almanach->maps);
    free(almanach);
}

RangeSet *new_range_set() {
    RangeSet *set = (RangeSet *)malloc(sizeof(RangeSet));
    set->count = 0;
    set->items = (Range *)malloc(sizeof(Range)*MAXRANGES);
    return set;
}

void destroy_range_set(RangeSet *set) {
    free(set->items);
    free(set);
}

ConverterSet *new_converter_set() {
    ConverterSet *set = (ConverterSet *)malloc(sizeof(ConverterSet));
    set->count = 0;
    set->items = (Converter *)malloc(sizeof(Converter)*MAXCONVERTERS);
    return set;
}

void destroy_converter_set(ConverterSet *set) {
    free(set->items);
    free(set);
}

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
        Range r = set->items[i];
        if(r.start == range.start && r.len == range.len)
            return true;
    }
    return false;
}
void add_range(RangeSet *set, Range range) {
    if(has_range(set, range))
        return;

    assert(set->count < MAXRANGES);
    set->items[set->count++] = range;
}

unsigned long range_end(Range range) {
    return range.start + range.len - 1;
}

void add_converter(ConverterSet *set, Converter converter) {
    assert(set);
    assert(set->count < MAXCONVERTERS);
    set->items[set->count++] = converter;
}

void read_almanach(Almanach *almanach, char *filename) {
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
    almanach->valueCount = scan_values(almanach->values, lines[0]);
    for(int i=0; i<almanach->valueCount; i++) {
        if(i%2) {
            Range range;
            range.start = almanach->values[i-1];
            range.len = almanach->values[i];
            add_range(almanach->seedRanges, range);
        }
    }
    int mapCount = 0;
    for(int i=1; i<lineCount; i++) {
        char *line = lines[i];
        if(strlen(line) == 0) {
            if (i > 2) {
                mapCount++;
            }
        } else if(strchr(line, ':')) {
            almanach->maps[mapCount].count = 0;
        } else {
            ConverterSet *set = &almanach->maps[mapCount];
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

void map_convert_ranges_all_map_level(RangeSet *current, int level, Almanach *almanach) {
#ifdef DEBUG
    printf("level:%d\n",level);
    printf("current:\n");
    print_ranges(current);
#endif
    if(level >= MAXMAPS) {
        almanach->minimumSeed = MIN(almanach->minimumSeed, minimum(current));
        printf("minimumSeed:%lu\n", almanach->minimumSeed);
        return;
    }
    ConverterSet converters = almanach->maps[level];
#ifdef DEBUG
    printf("%lu\n", sizeof(RangeSet));
#endif
    RangeSet *work = new_range_set();
    RangeSet *temp = new_range_set();
    copy_ranges(work, current);
    for(int r=0; r<work->count; r++) {
        Range range = work->items[r];
        map_convert_range(temp, range, &converters);
        map_convert_ranges_all_map_level(temp, level+1, almanach);
    }
    destroy_range_set(work);
    destroy_range_set(temp);
#ifdef DEBUG
    printf("released\n");
#endif
}
// convert a range through a set of converter, each remaining range from a conversion
// being processed with the next converter in the set
// all remaining ranges after the next converter is processed are added to the result
void map_convert_range(RangeSet *result, Range range, ConverterSet *converters) {
    RangeSet *remaining = new_range_set();
    RangeSet *work =      new_range_set();
    empty_ranges(result);
    add_range(work, range);
    for(int c=0; c<converters->count; c++) {
#ifdef DEBUG
        printf("ranges to convert\n"); print_ranges(work);
        printf("current converter:\t"); print_converter(converters->items[c]); printf("\n");
#endif
        for(int w=0; w<work->count; w++) {
            convert_range(result, remaining, work->items[w], converters->items[c]);
        }
        copy_ranges(work, remaining);
        empty_ranges(remaining);
    }
    append_ranges(result, work);
    destroy_range_set(remaining);
    destroy_range_set(work);
}

// convert a range through all 7 sets of converters
void map_convert_range_all_maps(RangeSet *result, Range range, ConverterSet *maps) {
    RangeSet *work = new_range_set();
    RangeSet *temp = new_range_set();
    add_range(work, range);
    for(int m = 0; m < 7; m++) {
#ifdef DEBUG
        printf("################# map#%d\n", m);
#endif
        ConverterSet converters = maps[m];
        empty_ranges(temp);
        for(int r = 0; r < work->count; r++) {
            map_convert_range(temp, work->items[r], &converters);
#ifdef DEBUG
        printf("------------------ map#%d done with result:\n", m);
        print_ranges(temp);
        getchar();
#endif
            copy_ranges(work, temp);
        }
    }
    copy_ranges(result, work);
    destroy_range_set(work);
    destroy_range_set(temp);
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
    RangeSet *source= new_range_set();
    RangeSet *dest = new_range_set(); 
    add_range(source, range);
    for(int i = 0; i < 7; i++) {
        ConverterSet converters = almanach->maps[i];
        empty_ranges(dest);
        for(int j=0; j<source->count; j++) {
            map_convert_range(dest, source->items[j], &converters);
        }
        copy_ranges(source, dest);
    }
    result->count = 0;
    append_ranges(result, source);
    destroy_range_set(source);
    destroy_range_set(dest);
}

void empty_ranges(RangeSet *dest) {
    dest->count = 0;
}

void append_ranges(RangeSet *dest, RangeSet *srce) {
    for(int i = 0; i < srce->count; i++) {
        add_range(dest, srce->items[i]);
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
    for(int i = 0; i < almanach->seedRanges->count; i++) {
        printf("seed range %d:",i); print_range(almanach->seedRanges->items[i]); printf("\n");
        source->count = 0;
        add_range(source, almanach->seedRanges->items[i]);
        for(int j = 0; j < MAXMAPS; j++) {
            printf("map %d\n",j); print_ranges(source); printf("\n");
            ConverterSet converters = almanach->maps[j];
            printf("converters\n");
            print_converter_set(&converters);
            work->count = 0;
            for(int k = 0; k < source->count; k++) {
                Range range = source->items[k];
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
        min = MIN(ranges->items[i].start, min);
    }
    return min;
}

void print_ranges(RangeSet *set) {
    for(int i = 0; i < set->count; i++) {
        print_range(set->items[i]);
    }
    printf("\n");
}

void print_converter_set(ConverterSet *set) {
    for(int i = 0; i < set->count; i++) {
        print_converter(set->items[i]);
    }
    printf("\n");
}
