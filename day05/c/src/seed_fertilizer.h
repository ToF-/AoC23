#include <stdbool.h>
#define MAXCONVERTERS 100
#define MAXMAPS 10
#define MAXSEEDS 200
#define MAXSEEDRANGES 100
#define INVALID 0xFFFFFFFF



struct Interval {
    unsigned long start;
    unsigned long len;
};

struct Range {
    unsigned long start;
    unsigned long len;
};

struct Converter {
    unsigned long dest;
    struct Range range;
};

struct Map {
    int maxConverters;
    struct Converter converters[MAXCONVERTERS];
};

struct Almanach {
    int maxSeeds;
    int maxSeedRanges;
    int maxMaps;
    unsigned long seeds[MAXSEEDS];
    unsigned long minLocations[MAXSEEDRANGES];
    struct Range seedRanges[MAXSEEDRANGES];
    struct Map maps[MAXMAPS];
};

bool read_almanach(struct Almanach *, char *);
int scan_ints(unsigned long *,int, char *);
int scan_seeds(unsigned long *, char *);
bool scan_converter(struct Converter*, char *);
int scan_map(struct Map*, char **, int);
void print_almanach(struct Almanach*);
unsigned long convert(unsigned long, struct Converter);
unsigned long map_convert(unsigned long, struct Map);
unsigned long minimum_location_map_all_seeds(struct Almanach *);
unsigned long minimum_location_map_all_seed_ranges(struct Almanach *);
