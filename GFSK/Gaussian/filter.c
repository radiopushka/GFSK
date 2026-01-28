#include "filter.h"
#include <math.h>
#include <stdlib.h>

struct GaussianFilter* init_gaussian_filter(int points) {
    struct GaussianFilter *filter = malloc(sizeof(struct GaussianFilter));
    filter->gaussian_filter = malloc(points * sizeof(float));
    filter->gaussian_end = filter->gaussian_filter + points;
    filter->ring = malloc(points * sizeof(float));


    return filter;
}
