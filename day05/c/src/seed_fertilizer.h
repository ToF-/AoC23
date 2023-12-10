#include <stdbool.h>
#define MAXCONVERTERS 100
#define MAXMAPS 10
#define MAXSEEDS 200
#define MAXRANGES 100
#define MAXSEEDRANGES 100
#define INVALID 0xFFFFFFFF

typedef struct {
    unsigned long start;
    unsigned long len;
} Range;

typedef struct {
    int maxRanges;
    Range ranges[MAXRANGES];
} RangeSet;

typedef struct {
    unsigned long dest;
    Range range;
} Converter;

typedef struct {
    int maxConverters;
    Converter converters[MAXCONVERTERS];
} Map;

typedef struct {
    int maxSeeds;
    int maxSeedRanges;
    int maxMaps;
    unsigned long seeds[MAXSEEDS];
    unsigned long minLocations[MAXSEEDRANGES];
    Range seedRanges[MAXSEEDRANGES];
    Map maps[MAXMAPS];
} Almanach;

typedef struct {
    Range intersect;
    Range before;
    Range beyond;
} Split;

bool read_almanach(Almanach *, char *);
int scan_ints(unsigned long *,int, char *);
int scan_seeds(unsigned long *, char *);
bool scan_converter(Converter*, char *);
int scan_map(Map*, char **, int);
void print_almanach(Almanach*);
unsigned long convert(unsigned long, Converter);
unsigned long map_convert(unsigned long, Map);
unsigned long minimum_location_map_all_seeds(Almanach *);
unsigned long minimum_location_map_all_seed_ranges(Almanach *);
Converter id_converter(Range);
Split split_range(Range, Converter);
bool valid_range(Range);
void print_split(Split);
void print_converter(Converter);
void split_map(Map*, Split);
void print_map(Map*);
void map_map(Map*, Map *, Map*);
void map_all_maps(Map*, Map*, Almanach *);
void add_converter(Map*, Converter);

void add_range(RangeSet*, Range);

void empty_map(Map *);
void append_map(Map *, Map *);
unsigned long end(Range);
