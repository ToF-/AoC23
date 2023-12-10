#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "seed_fertilizer.h"

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
    lines = (char **)malloc(lineCount * sizeof(char *));
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
    if(!strcmp(lines[lineCount-1],"")) {
        lines[lineCount] = (char *)malloc(strlen("  ")+1);
        strcpy(lines[lineCount],"  ");
        lineCount++;
    }
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
    assert(false);
}
