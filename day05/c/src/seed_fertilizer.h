#include <stdbool.h>
#define MAXLINE 100
#define MAXVALUES 30
#define MAXMAPS 7
#define MAXCONVERTERS 100
#define MAXRANGES 2500

typedef struct {
    unsigned long start;
    unsigned long len;
} Range;

typedef struct {
    int count;
    Range *items;
} RangeSet;

typedef struct {
    unsigned long dest;
    Range range;
} Converter;

typedef struct {
    int count;
    Converter *items;
} ConverterSet;

typedef struct {
    unsigned long minimumSeed;
    unsigned long *values;
    int valueCount;
    RangeSet *seedRanges;
    ConverterSet *maps;
} Almanach;


Almanach *new_almanach();
void destroy_almanach(Almanach *);

RangeSet *new_range_set();
void destroy_range_set(RangeSet *);

ConverterSet *new_converter_set();
void destroy_converter_set(ConverterSet *);


bool has_range(RangeSet *, Range);
void add_range(RangeSet *, Range);
ConverterSet new_ConverterSet();
RangeSet new_RangeSet();
void add_converter(ConverterSet *, Converter);
void read_almanach(Almanach *, char *);
void convert_range(RangeSet *, RangeSet *, Range, Converter);
unsigned long range_end(Range);
void map_convert_range(RangeSet *, Range, ConverterSet *);
void print_range(Range);
void print_ranges(RangeSet *);
void print_converter(Converter);
void print_converter_set(ConverterSet *);
void map_convert_range_all_maps(RangeSet *, Range, ConverterSet *);
void all_maps_range(RangeSet *, Range, Almanach *);
unsigned long minimum_all_maps_ranges(Almanach *);
void append_ranges(RangeSet *, RangeSet *);
void copy_ranges(RangeSet *, RangeSet *);
void empty_ranges(RangeSet *);
unsigned long minimum(RangeSet *);
void map_convert_ranges_all_map_level(RangeSet *, int, Almanach *);
