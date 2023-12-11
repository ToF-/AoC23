#include <stdbool.h>
#define MAXLINE 100
#define MAXVALUES 30
#define MAXMAPS 7
#define MAXCONVERTERS 100
#define MAXRANGES 1000

typedef struct {
    unsigned long start;
    unsigned long len;
} Range;

typedef struct {
    int count;
    Range item[MAXRANGES];
} RangeSet;

typedef struct {
    unsigned long dest;
    Range range;
} Converter;

typedef struct {
    int count;
    Converter item[MAXCONVERTERS];
} ConverterSet;

typedef struct {
    unsigned long values[MAXVALUES];
    int valueCount;
    RangeSet seedRanges;
    ConverterSet maps[MAXMAPS];
} Almanach;

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
void print_range_set(RangeSet *);
void print_converter(Converter);
void print_converter_set(ConverterSet *);
void map_convert_range_all_maps(RangeSet *, Range, ConverterSet *);
void all_maps_range(RangeSet *, Range, Almanach *);
unsigned long minimum_all_maps_ranges(Almanach *);
void append_ranges(RangeSet *, RangeSet *);
void copy_ranges(RangeSet *, RangeSet *);
void empty_ranges(RangeSet *);
unsigned long minimum(RangeSet *);
