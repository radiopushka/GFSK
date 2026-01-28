#ifndef GAUSSIANFILTER_H
#define GAUSSIANFILTER_H
struct GaussianFilter{
    float *gaussian_filter;
    float* gaussian_end;

    float *ring;
    float *ring_index;
    float *ring_end;
};
#endif
