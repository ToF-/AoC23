#include <stdbool.h>
#define MAXLINE 100
#define MAXVALUES 30
#define MAXMAPS 7
#define MAXCONVERTERS 100
#define MAXRANGES 120

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
void add_converter(ConverterSet *, Converter);
void read_almanach(Almanach *, char *);
void convert_range(RangeSet *, Range, Converter);
unsigned long range_end(Range);
void map_convert_range(RangeSet *, Range, ConverterSet *);
void print_range(Range );
void all_maps_range(RangeSet *, Range, Almanach *);
void append_ranges(RangeSet *, RangeSet *);
