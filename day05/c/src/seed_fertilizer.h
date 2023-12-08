#include <stdbool.h>
#define MAXCONVERTERS 100
#define MAXMAPS 10
#define MAXSEEDS 200
#define MAXSEEDRANGES 100



struct Interval {
    long start;
    long len;
};

struct Range {
    long start;
    long len;
};

struct Converter {
    long dest;
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
    long seeds[MAXSEEDS];
    struct Range seedRanges[MAXSEEDRANGES];
    struct Map maps[MAXMAPS];
};

bool read_almanach(struct Almanach *, char *);
int scan_ints(long *,int, char *);
int scan_seeds(long *, char *);
bool scan_converter(struct Converter*, char *);
int scan_map(struct Map*, char *[], int);
void print_almanach(struct Almanach*);
