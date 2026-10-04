// Runtime support library linked into every compiled program.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void rt_print_int(long value) { printf("%ld", value); }

void rt_print_str(const char* str) { fputs(str, stdout); }

void rt_div_by_zero(long line, long column) {
    fflush(stdout);
    fprintf(stderr, "%ld:%ld: division by zero\n", line, column);
    exit(1);
}

void rt_out_of_bounds(long index, long size, long line, long column) {
    fflush(stdout);
    fprintf(stderr, "%ld:%ld: array index %ld out of bounds %ld\n", line,
            column, index, size);
    exit(1);
}

// The interpreter's rand() differs on every run, so seed it the same way.
__attribute__((constructor)) static void rt_init(void) {
    srand((unsigned)time(NULL));
}
