#include "modulator.h"
#include<math.h>
#include <stdlib.h>

struct gfsk_demod* create_gfsk_demod(float carrier_freq,int tx_rate,int sample_rate){
    struct gfsk_demod* gfsk = malloc(sizeof(struct gfsk_demod));
    gfsk->dcount = 0;
    gfsk->dcount_max = sample_rate/tx_rate;

    float rate_f = sample_rate;
    float txrate_f = tx_rate;
    gfsk->am = 0;
    gfsk->am_2 = 0;
    float spread = 4;
    gfsk->am_shifter = ((carrier_freq-(txrate_f/spread))/rate_f)*(M_PI*2);
    gfsk->am_shifter_2 = ((carrier_freq+(txrate_f/spread))/rate_f)*(M_PI*2);
    float filter_freq = txrate_f;

    gfsk->frame = 0;
    gfsk->p2 = 0;
    gfsk->prev_bval = 0;

    gfsk->lpf_q_1 = create_Bessel(filter_freq,rate_f);
    gfsk->lpf_q_2 = create_Bessel(filter_freq,rate_f);
    gfsk->lpf_i_1 = create_Bessel(filter_freq,rate_f);
    gfsk->lpf_i_2 = create_Bessel(filter_freq,rate_f);

    gfsk->lpf_q_11 = create_Bessel(filter_freq,rate_f);
    gfsk->lpf_q_21 = create_Bessel(filter_freq,rate_f);
    gfsk->lpf_i_11 = create_Bessel(filter_freq,rate_f);
    gfsk->lpf_i_21 = create_Bessel(filter_freq,rate_f);
    gfsk->bit_count = 0;

    return gfsk;
}
struct gfsk_mod* create_gfsk_mod(float carrier_freq,int tx_rate,int sample_rate,float drive){
    struct gfsk_mod* gfsk = malloc(sizeof(struct gfsk_mod));

    float rate_f = sample_rate;
    float txrate_f = tx_rate;
    gfsk->am = 0;
    gfsk->am_shifter = (carrier_freq/rate_f)*(M_PI*2);
    float filter_freq = txrate_f;

    gfsk->lpf_mod = create_Bessel(filter_freq,rate_f);
    gfsk->lpf_mod_2 = create_Bessel(filter_freq,rate_f);

    float spread = 4;
    gfsk->mod_index = txrate_f/spread;
    gfsk->mod_index = (gfsk->mod_index/rate_f)*(M_PI*2);
    gfsk->baseline = drive;

    gfsk->bit_count = 0;

    gfsk->data = 0;
    gfsk->d_end = 0;
    gfsk->value = -1;
    gfsk->d_index = 0;

    gfsk->count = 0;
    gfsk->dcount_max = sample_rate/tx_rate;

    return gfsk;
}
void free_gfsk_mod(struct gfsk_mod* gfsk){
    free_IIR(gfsk->lpf_mod);
    free_IIR(gfsk->lpf_mod_2);
    if(gfsk->data){
        free(gfsk->data);
    }
    free(gfsk);

}

void free_gfsk_demod(struct gfsk_demod* gfsk){
    free_IIR(gfsk->lpf_q_1);
    free_IIR(gfsk->lpf_q_2);
    free_IIR(gfsk->lpf_i_1);
    free_IIR(gfsk->lpf_i_2);
    free_IIR(gfsk->lpf_q_11);
    free_IIR(gfsk->lpf_q_21);
    free_IIR(gfsk->lpf_i_11);
    free_IIR(gfsk->lpf_i_21);
    free(gfsk);
}
int _on_rx(uint16_t data,struct gfsk_demod* gfsk){
    unsigned char* as_str = (unsigned char*)&data;
    unsigned char delim1 = as_str[0];
    unsigned char delim2 = as_str[1];
    if(delim1 == ((~delim2)&255) && gfsk->bit_count >= 16){
        gfsk->bit_count = 1;
        //printf("Got %c\n",delim2);
        return delim2;

    }
    gfsk->bit_count++;
    return -1;
}


int run_gfsk_demod(struct gfsk_demod* gfsk,short* in,char* obuffer,int size){

    int chars = 0;
    for(short* i = in;i<in+size;i++){
        float in = *i;

        float ai = sinf(gfsk->am)*in;
        float aq = cosf(gfsk->am)*in;
        float ai_1 = calculate_lpf(gfsk->lpf_i_1,ai);
        ai = calculate_lpf(gfsk->lpf_i_2,ai_1);
        float aq_1 = calculate_lpf(gfsk->lpf_q_1,aq);
        aq = calculate_lpf(gfsk->lpf_q_2,aq_1);
        gfsk->am +=gfsk->am_shifter;
        if(gfsk->am >= M_2PI){
            gfsk->am -= M_2PI;
        }
        float ai2 = sinf(gfsk->am_2)*in;
        float aq2 = cosf(gfsk->am_2)*in;
        float ai_2 = calculate_lpf(gfsk->lpf_i_11,ai2);
        ai2 = calculate_lpf(gfsk->lpf_i_21,ai_2);
        float aq_2 = calculate_lpf(gfsk->lpf_q_11,aq2);
        aq2 = calculate_lpf(gfsk->lpf_q_21,aq_2);

        gfsk->am_2 +=gfsk->am_shifter_2;
        if(gfsk->am_2 >= M_2PI){
            gfsk->am_2 -= M_2PI;
        }




        float amp1 = sqrtf(ai*ai+aq*aq);
        float amp2 = sqrtf(ai2*ai2+aq2*aq2);


        int bval = 1;
        if(amp1>=amp2){
                bval = 0;
        }
        //printf("%d %f %f\n",bval,amp1,amp2);
        if((gfsk->prev_bval) != bval && (gfsk->prev_bval ==  gfsk->p2)){
              gfsk->dcount = (gfsk->dcount_max)>>1;
        }else{
              gfsk->dcount++;
        }

        if(gfsk->dcount >=gfsk->dcount_max){

            int sum = bval+gfsk->prev_bval+gfsk->p2;
            int val = 0;
            if(sum >=2){
                val = 1;
            }
            gfsk->frame = (gfsk->frame)<<1|val;
            int out = _on_rx(gfsk->frame,gfsk);
            if(out>=0){
                *obuffer = out;
                obuffer++;
                chars++;
            }
            gfsk->dcount = 0;
        }

        gfsk->p2 = gfsk->prev_bval;
        gfsk->prev_bval = bval;


    }
    return chars;
}

int run_gfsk_mod(struct gfsk_mod* gfsk,short* audio,int size){



    int done = (gfsk->d_index >= gfsk->d_end);
    for(short* i = audio;i<audio+size;i++){
        gfsk->count++;
        if(gfsk->count >= gfsk->dcount_max){
            if(done){
                gfsk->value = -1.0;
            }else{
                int datav = *(gfsk->d_index);
                int cmpr = 1<<(7-gfsk->bit_count);
                if(datav&cmpr){
                    gfsk->value = 1.0;
                }else{
                    gfsk->value = -1.0;
                }


                gfsk->bit_count++;
            }
            if(gfsk->bit_count >= 8){
                gfsk->bit_count = 0;
                gfsk->d_index++;
                if(gfsk->d_index >= gfsk->d_end){
                    done = 1;
                }
            }
            gfsk->count = 0;
        }

        float shifter = gfsk->value;
        float filter = calculate_lpf(gfsk->lpf_mod,shifter);
        float filter2 = calculate_lpf(gfsk->lpf_mod_2,filter);


        float osc = sinf(gfsk->am);
        gfsk->am +=(gfsk->am_shifter+(filter2*gfsk->mod_index));
        if(gfsk->am >= M_2PI){
            gfsk->am -= M_2PI;
        }

        *i = (short)((osc)*(gfsk->baseline));
    }
    return done;
}

void set_gfsk_data(struct gfsk_mod* gfsk, unsigned char* data,int length){

    if(gfsk->data)
    {
        free(gfsk->data);
    }
    int len = length<<1;
    gfsk->data = malloc(sizeof(char)*len);
    unsigned char* d = gfsk->data;
    for(unsigned char* i = data;i<data+length;i++){
        *d = *i;
        d++;
        *d = ~(*i);
        d++;
    }
    gfsk->d_index = gfsk->data;
    gfsk->d_end = gfsk->data+len;


}


