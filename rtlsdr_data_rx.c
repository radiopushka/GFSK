#include "GFSK/modulator.h"
#include<stdio.h>
#include<stdlib.h>

int main(){
    int bsize = 1024;

    short *frame = malloc(bsize*sizeof(short));

    struct gfsk_demod* mod = create_gfsk_demod(8000.0,8000,48000);

    char* output = malloc(1024*sizeof(char));

    while(1){

      int samples =fread(frame, sizeof(short), bsize, stdin);

      int chars = run_gfsk_demod(mod,frame,output,bsize);

      output[chars] = 0;
      printf("%s",output);
    }

    free_gfsk_demod(mod);
    free(output);
    free(frame);

}
