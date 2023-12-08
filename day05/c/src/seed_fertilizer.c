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

int scan_map(struct Map* map, char *lines[], int line) {
    map->maxConverters = 0;
    if(!strchr(lines[line],':')) {
        return 0;
    }
    int n = 1;
    while(strlen(lines[line+n]) > 0 && scan_converter(&(map->converters[map->maxConverters]), lines[line+n])) {
        struct Converter c = map->converters[map->maxConverters];
        map->maxConverters++;
        n++;
    }
    return map->maxConverters;
}

bool read_almanach(struct Almanach *almanach, char *filename) {
    char *lines[MAXLINE];
    int maxLine = 0;
    FILE *file = fopen(filename, "r");
    if(!file) {
        printf("can't open %s\n", filename);
        return false;
    }
    char line[MAXLINE];
    while(fgets(line,MAXLINE,file)!= NULL) {
        if(strlen(line)>2) {
            line[strcspn(line, "\n")] = '\0';
            assert(strlen(line) > 0);
            lines[maxLine] = (char *)malloc(strlen(line));
            printf("%p\n", lines[maxLine]);
            strcpy(lines[maxLine], line);
            maxLine++;
        }
    }
    printf("####\n");
    fclose(file);
    lines[maxLine] = (char *)malloc(strlen("     "));
    strcpy(lines[maxLine], "     ");
    maxLine++;
    long seeds[MAXSEEDS];
    int maxSeeds = scan_seeds(seeds, lines[0]);
    if (maxSeeds == 0) {
        printf("ooops\n");
        for(int i=0;i<maxLine; i++)
            free(lines[i]);
        return false;
    }
    for(int i=0; i<maxSeeds; i++) {
        almanach->seeds[i] = seeds[i];
    }
    almanach->maxSeeds = maxSeeds;
    int l = 1;
    almanach->maxMaps = 0;
    while(l < maxLine ) {
        int result = scan_map(&almanach->maps[almanach->maxMaps], lines, l);
        if (result > 0) {
            l = l + result + 1;
            almanach->maxMaps++;
        }
        else {
            l = maxLine;
        }
    }
    for(int i=0;i<maxLine; i++) {
        printf("%p\n", lines[i]);
        free(lines[i]);
    }
    return true;
}

void print_almanach(struct Almanach *almanach) {
    printf("seeds:");
    for(int i=0; i<almanach->maxSeeds; i++)
        printf(" %ld", almanach->seeds[i]);
    for(int i=0; i<almanach->maxMaps; i++) {
        printf("map#%d\n", i);
        for(int j=0; j<almanach->maps[i].maxConverters; j++) {
            struct Converter c = almanach->maps[i].converters[j];
            printf("%ld %ld %ld\n", c.dest, c.range.start, c.range.len);
        }
    }
    printf("\n");
}
