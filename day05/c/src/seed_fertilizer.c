#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <limits.h>
#include "seed_fertilizer.h"
#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a > b ? a : b)


Range make_range(unsigned long start, unsigned long len) {
    return (Range) { start, len };
}

Almanach *new_almanach() {
    Almanach *almanach = (Almanach *)malloc(sizeof(Almanach));
    almanach->values = (unsigned long *)malloc(sizeof(unsigned long)*MAXVALUES);
    almanach->seedRanges = new_range_set();
    almanach->minimumSeed = ULONG_MAX;
    return almanach;
}

void destroy_almanach(Almanach *almanach) {
    free(almanach->values);
    destroy_range_set(almanach->seedRanges);
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

bool pop_range(Range *range, RangeSet *set) {
    if(set->count ==0)
        return false;
    *range = set->items[0];
    for(int r=1; r<set->count; r++) {
        set->items[r-1] = set->items[r];
    }
    set->count--;
    return true;
}

unsigned long range_end(Range range) {
    return range.start + range.len - 1;
}

void add_converter(Map *map, Converter converter) {
    assert(map);
    assert(map->count < MAXCONVERTERS);
    map->converters[map->count++] = converter;
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
            add_range(almanach->seedRanges, make_range(almanach->values[i-1], almanach->values[i]));
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
            Map *converters = &almanach->maps[mapCount];
            add_converter(converters, scan_converter(line));
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

    unsigned long start = MAX(range.start, converter.range.start);
    unsigned long end   = MIN(range_end(range), range_end(converter.range));
    unsigned long len   = end > start ? end - start + 1 : 0;

    Range inter = make_range(start, len);
    if(inter.len == 0) {
        add_range(remaining, range);
        return;
    }
    if(range.start < inter.start) {
        add_range(remaining, make_range( range.start, inter.start - range.start ));
    }
    add_range(converted, make_range(inter.start + converter.dest - converter.range.start, inter.len ));
    if(range_end(range) > range_end(inter)) {
        add_range(remaining, make_range(inter.start + inter.len, range.start + range.len - (inter.start + inter.len)));
    }
}

// convert a range through a set of converter, each remaining range from a conversion
// being processed with the next converter in the set
// all remaining ranges after the next converter is processed are added to the result
void map_range(RangeSet *result, Range range, Map *map) {
    RangeSet *processing = new_range_set();
    RangeSet *rejected = new_range_set();
    empty_ranges(result);
    add_range(processing, range);
    for(int c=0; c<map->count; c++) {
        Converter converter = map->converters[c];
        Range range;
        empty_ranges(rejected);
        while(pop_range(&range, processing)) {
            convert_range(result, rejected, range, converter);
        }
        append_ranges(processing, rejected);
    }
    append_ranges(result, processing);
    destroy_range_set(processing);
    destroy_range_set(rejected);
}

// convert a set of ranges, typically to go from map#0 to map#6
void map_ranges(RangeSet *result, RangeSet *ranges, Map *map) {
    RangeSet *ranges_result = new_range_set();
    Range range;
    empty_ranges(result);
    while(pop_range(&range, ranges)) {
        map_range(ranges_result, range, map);
        append_ranges(result, ranges_result);
    }
    destroy_range_set(ranges_result);
}

// if wa map all the ranges through all the maps, what is the minimal range?
unsigned long minimum_range_start(Almanach *almanach) {
    RangeSet *result = new_range_set();
    RangeSet *ranges = new_range_set();
    Map map;
    unsigned long min = ULONG_MAX;
    for(int r = 0 ; r < almanach->seedRanges->count; r++) {
        empty_ranges(ranges);
        Range range = almanach->seedRanges->items[r];
        add_range(ranges, range);
        for(int m=0; m<MAXMAPS; m++) {
            map = almanach->maps[m];
            map_ranges(result, ranges, &map);
            copy_ranges(ranges, result);
        }
        min = MIN(min, minimum_start(ranges));
    }
    destroy_range_set(ranges);
    destroy_range_set(result);
    return min;
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

unsigned long minimum_start(RangeSet *ranges) {
    unsigned long min = ULONG_MAX;
    for(int i=0; i < ranges->count; i++) {
        min = MIN(min, ranges->items[i].start);
    }
    return min;
}

void print_ranges(RangeSet *set) {
    for(int i = 0; i < set->count; i++) {
        print_range(set->items[i]);
    }
    printf("\n");
}

void print_converter_set(Map *map) {
    for(int i = 0; i < map->count; i++) {
        print_converter(map->converters[i]);
    }
    printf("\n");
}
