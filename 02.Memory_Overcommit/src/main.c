#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

void *allocate_memory(size_t size);

#define STEP (1024 * 4)
#define GB (1024ll * 1024ll * 1024ll)
#define SECOND (1)
#define MEMORY_32GB (GB * 32ll)

void *allocate_memory(size_t size) {
    return malloc(size);
}

#define HELP \
    "Application to make overcommit experiment\n" \
    "\t-s - set size of memory to allocate (in GB)\n" \
    "\t-p - set pause time between memory accesses\n" \
    "\t-v - log proccess\n"

int main(int argc, char *argv[]) {
    size_t size = MEMORY_32GB;
    int pause_time = SECOND;
    int verbose = 0;
    int opt;
    while ((opt = getopt(argc, argv, "s:p:v")) != -1) {
        switch (opt) {
        case 's':
            size = atoll(optarg) * GB;
            break;
        case 'p':
            pause_time = atoi(optarg);
            break;
        case 'v':
            verbose = 1;
            break;
        default:
            fprintf(stderr, HELP);
            return -1;
        }
    }

    // (1) get memory
    void *mem = allocate_memory(size);

    assert(mem && "should be allocated");

    // (2) make store accesses
    for (size_t i = 0; i < size; i += STEP) {
        if (verbose && i % GB == 0) {
            printf("Loaded pages: %lld GB / %lld GB\n", i / GB, size / GB);
            fflush(stdout);
        }

        char *cur_page = (char *) mem + i;
        *cur_page = 42;

        if (pause_time && i % GB == 0) sleep(pause_time);
    }

    for (size_t i = 0; i < size; i += STEP) {
        *((char *) mem);
    }

    return 0;
}
