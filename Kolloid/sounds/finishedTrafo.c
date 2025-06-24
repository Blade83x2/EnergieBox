#include <wiringPi.h>
#include <softTone.h>
#include <stdio.h>

// PIN 17
#define BuzPin 0

#define CL1 131
#define CL2 147
#define CL3 165
#define CL4 175
#define CL5 196
#define CL6 221
#define CL7 248

#define CM1 262
#define CM2 294
#define CM3 330
#define CM4 350
#define CM5 393
#define CM6 441
#define CM7 495

#define CH1 525
#define CH2 589
#define CH3 661
#define CH4 700
#define CH5 786
#define CH6 882
#define CH7 990

int song_3[] = {CM1, CM1, CM4, CM1, CM4};
int beat_3[] = {1, 1, 2, 1, 2};

int main(void) {
    int i;
    if (wiringPiSetup() == -1) {
        printf("setup wiringPi failed !");
        return -1;
    }

    if (softToneCreate(BuzPin) == -1) {
        printf("setup softTone failed !");
        return -1;
    }
    for (i = 0; i < sizeof(song_3) / 4; i++) {
        softToneWrite(BuzPin, song_3[i]);
        delay(beat_3[i] * 500);
    }
    return 0;
}
