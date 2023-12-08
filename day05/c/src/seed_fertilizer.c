#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "seed_fertilizer.h"

#define MAXLINE 300

int scan_ints(long *dest, int max_to_scan, char *srce) {
    int scanned = 0;
    char *p = srce;
    while(scanned < max_to_scan && *p) {
        char *end;
        long n = strtol(p, &end, 10);
        if(end == p) {
            return scanned;
        }
        dest[scanned] = n;
        scanned++;
        p = end;
        while(*p && !isdigit(*p)) p++;
    }
    return scanned;
}

int scan_seeds(long *dest, char *srce) {
    char *p = strchr(srce,' ');
    return scan_ints(dest, MAXSEEDS, p);
}
bool scan_converter(struct Converter* converter, char *srce) {
    long values[3];
    if(scan_ints(values, 3, srce)) {
        converter->dest = values[0];
        converter->range.start = values[1];
        converter->range.len = values[2];
        return true;
    }
    return false;
}

int scan_map(struct Map* map, char **lines, int remaining) {
    map->maxConverters = 0;
    if(!strchr(lines[0],':')) {
        return 0;
    }
    for(int i = 1; i < remaining && isdigit(lines[i][0]); i++) {
        struct Converter *converter = &map->converters[map->maxConverters];
        scan_converter(converter, lines[i]);
        map->maxConverters++;
    }
    return map->maxConverters;
}

bool read_almanach(struct Almanach *almanach, char *filename) {
    char **lines;
    char line[MAXLINE];
    int lineCount = 0;
    FILE *file;
    file = fopen(filename, "r");
    if(!file) {
        perror("can't open file");
        return false;
    }
    while(fgets(line, MAXLINE, file) != NULL) {
        lineCount++;
    }
    lines = (char **)malloc(lineCount * sizeof(char *));
    if(lines == NULL) {
        perror("memory allocation error");
        fclose(file);
        return false;
    }
    rewind(file);
    for(int i=0; i<lineCount; i++) {
        fgets(line, MAXLINE, file);
        line[strcspn(line,"\n")] = 0;
        lines[i] = (char *)malloc(strlen(line)+1);
        if (lines[i] == NULL) {
            perror("memory allocation error");
            for(int j=0; j<i; j++) {
                free(lines[j]);
            }
            free(lines);
            fclose(file);
            return false;
        }
        strcpy(lines[i], line);
    }
    fclose(file);
    long *seeds = almanach->seeds;
    almanach->maxSeeds = scan_seeds(seeds, lines[0]);
    almanach->maxMaps = 0;
    struct Map *map;
    for(int i=1; i<lineCount ;i++) {
        assert(almanach->maxMaps < 7);
        if(strlen(lines[i]) == 0) {
            if(i > 2){
                almanach->maxMaps++;
            }
        } else if(strchr(lines[i], ':')) {
            struct Map *map = &almanach->maps[almanach->maxMaps];
            map->maxConverters = 0;
        } else {
            struct Map *map = &almanach->maps[almanach->maxMaps];
            assert(scan_converter(&map->converters[map->maxConverters], lines[i]));
            map->maxConverters++;
            assert(almanach->maps[almanach->maxMaps].maxConverters);
        }
    }
    for(int i=0; i<lineCount; i++) {
        free(lines[i]);
    }
    for(int i=0; i<7; i++)
        assert(almanach->maps[i].maxConverters);
    print_almanach(almanach);
    return true;
}

void print_almanach(struct Almanach *almanach) {
    printf("seeds:");
    for(int i=0; i<almanach->maxSeeds; i++)
        printf(" %ld", almanach->seeds[i]);
    printf("\n");
    for(int i=0; i<almanach->maxMaps; i++) {
        printf("map#%d\n", i);
        assert(almanach->maps[i].maxConverters);
        for(int j=0; j<almanach->maps[i].maxConverters; j++) {
            struct Converter c = almanach->maps[i].converters[j];
            printf("%ld %ld %ld\n", c.dest, c.range.start, c.range.len);
        }
    }
    printf("\n");
}
