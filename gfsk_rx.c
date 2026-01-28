#include "./alsa/alsa.h"
#include "GFSK/modulator.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(){
    int bsize = 1024;
    if(setup_alsa("default",0,bsize, 48000)<0)
        printf("Failed to setup alsa\n");

    short *frame = malloc(bsize*sizeof(short));

    struct gfsk_demod* mod = create_gfsk_demod(19000.0,1500,48000);

    char* output = malloc(1024*sizeof(char));
    char* trm = malloc(1024*sizeof(char));

    while(1){

      aread(frame,bsize);
      int chars = run_gfsk_demod(mod,frame,output,bsize);
      output[chars] = 0;
      printf("%s",output);
    }

    free_gfsk_demod(mod);

    free_alsa();
}
