#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "seed_fertilizer.h"

#define MAXLINE 300

unsigned long min(unsigned long, unsigned long);
unsigned long max(unsigned long, unsigned long);

void make_seed_ranges(Almanach *);
bool has_converter(Map *, Converter);
void append_map(Map *, Map *);

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
bool scan_converter(Converter* converter, char *srce) {
    unsigned long values[3];
    if(scan_ints(values, 3, srce)) {
        converter->dest = values[0];
        converter->range.start = values[1];
        converter->range.len = values[2];
        return true;
    }
    return false;
}

int scan_map(Map* map, char **lines, int remaining) {
    map->maxConverters = 0;
    if(!strchr(lines[0],':')) {
        return 0;
    }
    for(int i = 1; i < remaining && isdigit(lines[i][0]); i++) {
        Converter *converter = &map->converters[map->maxConverters];
        scan_converter(converter, lines[i]);
        map->maxConverters++;
    }
    return map->maxConverters;
}

bool read_almanach(Almanach *almanach, char *filename) {
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
    Map *map;
    for(int i=1; i<lineCount ;i++) {
        assert(almanach->maxMaps < 7);
        if(strlen(lines[i]) == 0) {
            if(i > 2){
                almanach->maxMaps++;
            }
        } else if(strchr(lines[i], ':')) {
            Map *map = &almanach->maps[almanach->maxMaps];
            map->maxConverters = 0;
        } else {
            Map *map = &almanach->maps[almanach->maxMaps];
            assert(scan_converter(&map->converters[map->maxConverters], lines[i]));
            map->maxConverters++;
        }
    }
    for(int i=0; i<lineCount; i++) {
        free(lines[i]);
    }
    for(int i=0; i<7; i++)
        assert(almanach->maps[i].maxConverters);
    return true;
}

void print_almanach(Almanach *almanach) {
    printf("seeds:");
    for(int i=0; i<almanach->maxSeeds; i++)
        printf(" %ld", almanach->seeds[i]);
    printf("\n");
    for(int i=0; i<almanach->maxMaps; i++) {
        printf("map#%d\n", i);
        assert(almanach->maps[i].maxConverters);
        for(int j=0; j<almanach->maps[i].maxConverters; j++) {
            Converter c = almanach->maps[i].converters[j];
            printf("%ld %ld %ld\n", c.dest, c.range.start, c.range.len);
        }
    }
    printf("\n");
}

unsigned long convert(unsigned long n, Converter converter) {
    unsigned long result = n >= converter.range.start && n < converter.range.start + converter.range.len ? 
        converter.dest + n - converter.range.start : INVALID;
    return result;
}

unsigned long map_convert(unsigned long n, Map map) {
    for(int i=0; i<map.maxConverters; i++) {
        unsigned long v = convert(n, map.converters[i]);
        if(v != INVALID)
            return v;
    }
    return n;
}

unsigned long minimum_location_map_all_seeds(Almanach *almanach) {
    for(int seed_no = 0; seed_no < almanach->maxSeeds; seed_no++) {
        unsigned long value = almanach->seeds[seed_no];
        for(int map_no = 0; map_no < almanach->maxMaps; map_no++) {
            Map map = almanach->maps[map_no];
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

void make_seed_ranges(Almanach *almanach) {
    almanach->maxSeedRanges = almanach->maxSeeds / 2;
    for(int i = 0; i < almanach->maxSeedRanges; i++) {
        almanach->seedRanges[i].start = almanach->seeds[i*2];
        almanach->seedRanges[i].len   = almanach->seeds[i*2+1];
    }
}

unsigned long minimum_location_map_all_seed_ranges(Almanach *almanach) {
    for(int seed_range_no = 0; seed_range_no < almanach->maxSeedRanges; seed_range_no++) {
        unsigned long min_value = INVALID;
        Range seedRange = almanach->seedRanges[seed_range_no];
        for(unsigned long l = 0; l < seedRange.len; l++) {
            if(!(l%1000000)) printf("%2d %12lu/%12lu (%2.2f)\n", seed_range_no,l, seedRange.len, 100.0*((double)l / (double)seedRange.len));
            unsigned long value = seedRange.start + l;
            for(int map_no = 0; map_no < almanach->maxMaps; map_no++) {
                Map map = almanach->maps[map_no];
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

Converter id_converter(Range range) {
    Converter result = { range.start, range };
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

unsigned long end(Range range) {
    return range.start + range.len - 1;
}

Split split_range(Range original, Converter splitter) {
   Range empty = { 0, 0};
   Split result = { empty, empty, empty };
   Range intersect;
   intersect.start = max(original.start, splitter.range.start);
   unsigned long intersect_end = min(end(original), end(splitter.range));

   intersect.len = intersect_end > intersect.start ? intersect_end - intersect.start + 1 : 0 ;
   if (intersect.len == 0)
       return result;

   result.intersect.start = intersect.start + splitter.dest - splitter.range.start;
   result.intersect.len = intersect.len;

   if(original.start < intersect.start) {
       result.before.start = original.start;
       result.before.len = intersect.start - result.before.start;
   }
   if(end(original) > end(intersect)) {
       result.beyond.start = intersect.start + intersect.len;
       result.beyond.len = original.start + original.len - result.beyond.start;
   }
   return result;
}

bool valid_range(Range range) {
    return range.len > 0;
}

void print_split(Split split) {
    printf("\nbefore: %lu %lu [%lu..%lu]\t intersect: %lu %lu [%lu %lu]\t beyond: %lu %lu [%lu %lu]\n",
            split.before.start, split.before.len, split.before.start, end(split.before),
            split.intersect.start, split.intersect.len, split.intersect.start, end(split.intersect),
            split.beyond.start, split.beyond.len, split.beyond.start, end(split.beyond));

}
void print_converter(Converter converter) {
    if(converter.dest != converter.range.start) {
        printf("%lu %lu %lu | [%lu..%lu]->[%lu..%lu]\n", 
                converter.dest, converter.range.start, converter.range.len,
                converter.range.start, end(converter.range),
                converter.dest, converter.dest + converter.range.len - 1);
    } else {
        printf("%lu %lu %lu | [%lu..%lu]\n", 
                converter.dest, converter.range.start, converter.range.len,
                converter.range.start, end(converter.range));
    }
}

void split_map(Map *dest, Split split) {
    empty_map(dest);
    if (!valid_range(split.intersect)) {
        return;
    }
    if (valid_range(split.before)) {
        add_converter(dest, split.before);
    }
    if (valid_range(split.intersect)) {
        Converter new_converter = split.intersect;
        new_converter.range.start = new_converter.dest;
        add_converter(dest, new_converter);
    }
    if (valid_range(split.beyond)) {
        add_converter(dest, split.beyond);
    }
}

void print_map(Map *map) {
    for(int i = 0; i < map->maxConverters; i++) {
        print_converter(map->converters[i]);
    }
}

void map_map(Map *dest, Map *srce, Map *map) {
    empty_map(dest);
    for(int i=0; i < srce->maxConverters; i++) {
        Converter source = srce->converters[i];
        for(int j=0; j < map->maxConverters; j++) {
            Converter converter = map->converters[j];
            Split split = split_range(source, converter);
            Map inter;
            empty_map(&inter);
            split_map(&inter, split);
            append_map(dest, &inter);
        }
        if(dest->maxConverters == 0) {
            add_converter(dest, source);
        }
    }
}

void map_all_maps(Map *dest, Map*srce, Almanach *almanach) {
    Map source;
    Map inter;
    empty_map(&source);
    append_map(&source, srce);
    for(int i=0; i < almanach->maxMaps; i++) {
        empty_map(&inter);
        printf("source:\n");
        print_map(&source);
        printf("maps[%d]:\n", i);
        print_map(&almanach->maps[i]);
        map_map(&inter, &source, &almanach->maps[i]);
        printf("inter:\n");
        print_map(&inter);
        empty_map(&source);
        append_map(&source, &inter);
    }
    empty_map(dest);
    append_map(dest, &source);
}

void add_converter(Map *dest, Converter converter) {
    if(has_converter(dest, converter))
            return;
    if(dest->maxConverters >= MAXCONVERTERS) {
        perror("max converters exceeded");
        exit(1);
    }
    dest->converters[dest->maxConverters++] = converter;
}

bool has_converter(Map *map, Converter converter) {
    for(int i = 0; i < map->maxConverters; i++) {
        Converter conv = map->converters[i];
        if(conv.dest == converter.dest 
                && conv.range.start == converter.range.start
                && conv.range.len == converter.range.len)
            return true;
    }
    return false;
}

void empty_map(Map *map) {
    map->maxConverters = 0;
}

void append_map(Map *dest, Map *srce) {
    for(int i=0; i<srce->maxConverters; i++) {
        add_converter(dest, srce->converters[i]);
    }
}
