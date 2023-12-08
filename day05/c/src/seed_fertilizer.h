#include <stdbool.h>
#define MAXCONVERTERS 100
#define MAXMAPS 10
#define MAXSEEDS 200
#define MAXSEEDRANGES 100



struct Interval {
    int start;
    int len;
};

struct Range {
    int start;
    int len;
};

struct Converter {
    int dest;
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
    int seeds[MAXSEEDS];
    struct Range seedRanges[MAXSEEDRANGES];
    struct Map maps[MAXMAPS];
};

bool read_almanach(struct Almanach *, char *);
int scan_ints(int *,int, char *);
int scan_seeds(int *, char *);
bool scan_converter(struct Converter*, char *);
int scan_map(struct Map*, char *[], int);
