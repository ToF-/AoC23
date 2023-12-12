#include <stdbool.h>
#define MAXLINE 1000
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
    Converter converters[MAXCONVERTERS];
} Map;

typedef struct {
    unsigned long minimumSeed;
    unsigned long *values;
    int valueCount;
    RangeSet *seedRanges;
    Map maps[MAXMAPS];
} Almanach;


Almanach *new_almanach();
void destroy_almanach(Almanach *);

RangeSet *new_range_set();
void destroy_range_set(RangeSet *);

bool has_range(RangeSet *, Range);
void add_range(RangeSet *, Range);
bool pop_range(Range *, RangeSet *);
RangeSet new_RangeSet();
void add_converter(Map *, Converter);
void read_almanach(Almanach *, char *);
void convert_range(RangeSet *, RangeSet *, Range, Converter);
unsigned long range_end(Range);
void map_range(RangeSet *, Range, Map *);
void map_ranges(RangeSet *, RangeSet *, Map *);
unsigned long minimum_range_start(Almanach *);
void print_range(Range);
void print_ranges(RangeSet *);
void print_converter(Converter);
void print_converter_set(Map *);
void append_ranges(RangeSet *, RangeSet *);
void copy_ranges(RangeSet *, RangeSet *);
void empty_ranges(RangeSet *);
unsigned long minimum_start(RangeSet *);
