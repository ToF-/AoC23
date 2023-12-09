#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "seed_fertilizer.h"

#define MAXLINE 300

unsigned long min(unsigned long, unsigned long);
unsigned long max(unsigned long, unsigned long);

void make_seed_ranges(struct Almanach *);

int scan_ints(unsigned long *dest, int max_to_scan, char *srce) {
    int scanned = 0;
    char *p = srce;
    while(scanned < max_to_scan && *p) {
        char *end;
        unsigned long n = strtol(p, &end, 10);
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

int scan_seeds(unsigned long *dest, char *srce) {
    char *p = strchr(srce,' ');
    return scan_ints(dest, MAXSEEDS, p);
}
bool scan_converter(struct Converter* converter, char *srce) {
    unsigned long values[3];
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
    unsigned long *seeds = almanach->seeds;
    almanach->maxSeeds = scan_seeds(seeds, lines[0]);
    make_seed_ranges(almanach);
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
            struct Converter last = map->converters[map->maxConverters-1];
            for(int k=0; k<map->maxConverters-1; k++) {
                struct Split split = split_converter(map->converters[k], last);
                assert(!valid_converter(split.intersect));
            }
        }
    }
    for(int i=0; i<lineCount; i++) {
        free(lines[i]);
    }
    for(int i=0; i<7; i++)
        assert(almanach->maps[i].maxConverters);
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

unsigned long convert(unsigned long n, struct Converter converter) {
    unsigned long result = n >= converter.range.start && n < converter.range.start + converter.range.len ? 
        converter.dest + n - converter.range.start : INVALID;
    return result;
}

unsigned long map_convert(unsigned long n, struct Map map) {
    for(int i=0; i<map.maxConverters; i++) {
        unsigned long v = convert(n, map.converters[i]);
        if(v != INVALID)
            return v;
    }
    return n;
}

unsigned long minimum_location_map_all_seeds(struct Almanach *almanach) {
    for(int seed_no = 0; seed_no < almanach->maxSeeds; seed_no++) {
        unsigned long value = almanach->seeds[seed_no];
        for(int map_no = 0; map_no < almanach->maxMaps; map_no++) {
            struct Map map = almanach->maps[map_no];
            value = map_convert(value, map);
        }
        almanach->seeds[seed_no] = value;
    }
    unsigned long min = INVALID;
    for(int i=0; i < almanach->maxSeeds; i++) {
        if( almanach->seeds[i] < min) {
            min = almanach->seeds[i];
        }
    }
    return min;
}

void make_seed_ranges(struct Almanach *almanach) {
    almanach->maxSeedRanges = almanach->maxSeeds / 2;
    for(int i = 0; i < almanach->maxSeedRanges; i++) {
        almanach->seedRanges[i].start = almanach->seeds[i*2];
        almanach->seedRanges[i].len   = almanach->seeds[i*2+1];
    }
}

unsigned long minimum_location_map_all_seed_ranges(struct Almanach *almanach) {
    for(int seed_range_no = 0; seed_range_no < almanach->maxSeedRanges; seed_range_no++) {
        unsigned long min_value = INVALID;
        struct Range seedRange = almanach->seedRanges[seed_range_no];
        for(unsigned long l = 0; l < seedRange.len; l++) {
            if(!(l%1000000)) printf("%2d %12lu/%12lu (%2.2f%)\n", seed_range_no,l, seedRange.len, 100.0*((double)l / (double)seedRange.len));
            unsigned long value = seedRange.start + l;
            for(int map_no = 0; map_no < almanach->maxMaps; map_no++) {
                struct Map map = almanach->maps[map_no];
                value = map_convert(value, map);
            }
            if (value < min_value) {
                min_value = value;
            }
        }
        almanach->minLocations[seed_range_no] = min_value;
            printf("min value: %lu\n", min_value);
    }
    unsigned long min = INVALID;
    for(int i=0; i < almanach->maxSeedRanges; i++) {
        if( almanach->minLocations[i] < min) {
            min = almanach->minLocations[i];
        }
    }
    return min;
}

struct Converter id_converter(struct Range range) {
    struct Converter result = { range.start, range };
    return result;
}

unsigned long min(unsigned long a, unsigned long b) {
    if (a < b)
        return a;
    return b;
}

unsigned long max(unsigned long a, unsigned long b) {
    if (a > b)
        return a;
    return b;
}

struct Split split_converter(struct Converter original, struct Converter splitter) {
   struct Converter empty = { 0, { 0, 0}};
   struct Split result = { empty, empty, empty, empty };
   struct Range intersect_range;
   intersect_range.start = max(original.range.start, splitter.range.start);
   unsigned long intersect_end = min(original.range.start + original.range.len - 1, splitter.range.start + splitter.range.len - 1);

   intersect_range.len = intersect_end > intersect_range.start ? intersect_end - intersect_range.start + 1 : 0 ;
   result.intersect.range = intersect_range;
   if (!valid_converter(result.intersect)) {
       result.original = original;
       result.intersect = empty;
       return result;
   }
   result.original = empty;
   result.intersect.range.start = intersect_range.start;
   result.intersect.dest = splitter.dest;

   if(original.range.start < result.intersect.range.start) {
       result.before.dest = original.dest;
       result.before.range.start = original.range.start;
       result.before.range.len = result.intersect.range.start - result.before.range.start;
   }
   if((original.range.start + original.range.len - 1) > (result.intersect.range.start + result.intersect.range.len - 1)) {
       result.beyond.dest = result.intersect.range.start + result.intersect.range.len;
       result.beyond.range.start = result.beyond.dest;
       result.beyond.range.len = original.range.start + original.range.len - result.beyond.range.start;
   }
   assert(result.before.range.len + result.intersect.range.len + result.beyond.range.len == original.range.len);
   return result;
}

bool valid_converter(struct Converter converter) {
    return converter.range.len > 0;
}

void print_split(struct Split split) {
    printf("\noriginal: %lu %lu %lu [%lu..%lu]\t before: %lu %lu %lu [%lu..%lu]\t intersect: %lu %lu %lu [%lu %lu]\t beyond: %lu %lu %lu [%lu %lu]\n",
            split.original.dest, split.original.range.start, split.original.range.len, split.original.range.start, split.original.range.start + split.original.range.len - 1,
            split.before.dest, split.before.range.start, split.before.range.len, split.before.range.start, split.before.range.start + split.before.range.len - 1,
            split.intersect.dest, split.intersect.range.start, split.intersect.range.len, split.intersect.range.start, split.intersect.range.start + split.intersect.range.len - 1,
            split.beyond.dest, split.beyond.range.start, split.beyond.range.len, split.beyond.range.start, split.beyond.range.start + split.beyond.range.len - 1);

}
void print_converter(struct Converter converter) {
    printf("%lu %lu %lu [%lu..%lu]\n", converter.dest, converter.range.start, converter.range.len,
            converter.range.start, converter.range.start + converter.range.len - 1);
}

void split_map(struct Map *dest, struct Split split) {
    dest->maxConverters = 0;
    if (!valid_converter(split.intersect)) {
        dest->converters[dest->maxConverters] = split.original;
        dest->maxConverters++;
        return;
    }
    if (valid_converter(split.before)) {
        dest->converters[dest->maxConverters] = split.before;
        dest->maxConverters++;
    }
    if (valid_converter(split.intersect)) {
        dest->converters[dest->maxConverters] = split.intersect;
        dest->converters[dest->maxConverters].range.start = dest->converters[dest->maxConverters].dest;
        dest->maxConverters++;
    }
    if (valid_converter(split.beyond)) {
        dest->converters[dest->maxConverters] = split.beyond;
        dest->maxConverters++;
    }
}

void print_map(struct Map *map) {
    for(int i = 0; i < map->maxConverters; i++) {
        print_converter(map->converters[i]);
    }
}

void map_map(struct Map *dest, struct Map *srce, struct Map *map) {
    printf("initial:\n"); print_map(dest);
    printf("source:\n"); print_map(srce);
    printf("map:\n"); print_map(map);
    dest->maxConverters = 0;
    for(int i=0; i < srce->maxConverters; i++) {
        struct Converter source = srce->converters[i];
        for(int j=0; j < map->maxConverters; j++) {
            struct Converter converter = map->converters[j];
            struct Split split = split_converter(source, converter);
            struct Map inter;
            split_map(&inter, split);
            assert(inter.maxConverters > 0);
            for(int k=0; k<inter.maxConverters; k++) {
                dest->converters[dest->maxConverters] = inter.converters[k];
                dest->maxConverters++;
            }
        }
    }
    assert(dest->maxConverters > 0);
    printf("result:\n"); print_map(dest);
}
