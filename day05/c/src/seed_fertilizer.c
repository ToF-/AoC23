#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "seed_fertilizer.h"

#define MAXLINE 300

int scan_ints(int *dest, int max_to_scan, char *srce) {
    int scanned = 0;
    char *p = srce;
    while(scanned < max_to_scan && *p) {
        char *end;
        long n = strtol(p, &end, 10);
        if(end == p) {
            return scanned;
        }
        dest[scanned] = (int)n;
        scanned++;
        p = end;
        while(*p && !isdigit(*p)) p++;
    }
    return scanned;
}

int scan_seeds(int *dest, char *srce) {
    char *p = strchr(srce,' ');
    return scan_ints(dest, MAXSEEDS, p);
}
bool scan_converter(struct Converter* converter, char *srce) {
    int values[3];
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
            lines[maxLine] = (char *)malloc(strlen(line));
            strcpy(lines[maxLine], line);
            maxLine++;
        }
    }
    fclose(file);
    lines[maxLine] = (char *)malloc(strlen(" "));
    strcpy(lines[maxLine], " ");
    maxLine++;
    int seeds[MAXSEEDS];
    int maxSeeds = scan_seeds(seeds, lines[0]);
    if (maxSeeds == 0)
        return false;

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
    return true;
}

void print_almanach(struct Almanach *almanach) {
    printf("seeds:");
    for(int i=0; i<almanach->maxSeeds; i++)
        printf(" %d", almanach->seeds[i]);
    for(int i=0; i<almanach->maxMaps; i++) {
        printf("map#%d\n", i);
        for(int j=0; j<almanach->maps[i].maxConverters; j++) {
            struct Converter c = almanach->maps[i].converters[j];
            printf("%d %d %d\n", c.dest, c.range.start, c.range.len);
        }
    }
    printf("\n");
}
