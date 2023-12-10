#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "seed_fertilizer.h"
#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a > b ? a : b)

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

    set->item[set->count++] = range;
}

unsigned long range_end(Range range) {
    return range.start + range.len - 1;
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
    for(int i=0; i<dest->valueCount; i+=2) {
        Range range = { dest->values[i], dest->values[i+1] };
        add_range(&dest->seedRanges, range);
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

void convert_range(RangeSet *set, Range range, Converter converter) {
    Range inter;

    inter.start = MAX(range.start, converter.range.start);
    unsigned long inter_end = MIN(range_end(range), range_end(converter.range));
    inter.len = inter_end > inter.start ? inter_end - inter.start + 1 : 0;
    if(inter.len == 0)
        return;

    if(range.start < inter.start) {
        Range before;
        before.start = range.start;
        before.len = inter.start - before.start;
        add_range(set, before);
    }
    Range convert;
    convert.start = inter.start + converter.dest - converter.range.start;
    convert.len = inter.len;
    add_range(set, convert);
    if(range_end(range) > range_end(inter)) {
        Range after;
        after.start = inter.start + inter.len;
        after.len = range.start + range.len - after.start;
        add_range(set, after);
    }
}

void map_convert_range(RangeSet *result, Range range, ConverterSet *set) {
    result->count = 0;
    for(int i=0; i<set->count; i++) {
        convert_range(result, range, set->item[i]);
    }
    if(result->count == 0) {
        add_range(result, range);
    }
}

void print_range(Range range) {
    printf("%8lu %8lu [%lu..%lu]\n",
            range.start, range.len,
            range.start, range_end(range));
}

void all_maps_range(RangeSet *result, Range range, Almanach *almanach) {
    RangeSet source;
    source.count = 0;
    add_range(&source, range);
    for(int i = 0; i < 7; i++) {
        ConverterSet converters = almanach->maps[i];
        RangeSet dest;
        dest.count = 0;
        for(int j=0; j<source.count; j++) {
            map_convert_range(&dest, source.item[j], &converters);
        }
        source.count = 0;
        append_ranges(&source, &dest);
        for(int i =0; i<source.count; i++) { print_range(source.item[i]); }
        getchar();
    }
    result->count = 0;
    append_ranges(result, &source);
}

void append_ranges(RangeSet *dest, RangeSet *srce) {
    for(int i = 0; i < srce->count; i++) {
        add_range(dest, srce->item[i]);
    }
}
