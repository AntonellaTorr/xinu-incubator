#include <xinu.h>
#include <stdint.h>
#include <math.h>
#include "twi.h"
#include "sht31.h"
#include "tarea_controltyh.h"
#include "serial.h"

extern float sht31_temperature;
extern float sht31_humidity;

//Se conecta scl a A5 y sda a A4

int control_tyh(void){
    int loopCnt = 0;
    int enableHeater = 0;
    float t, h;

    twi_init();
    sleepms(5000);


    if (!sht31_begin()) {
        while(1) {sleepms(1000);}
    }

    while (1) {
        if (sht31_read_both(&t, &h)) {
            sync_wait(SEM_TYH);
                temperatura = t;
                humedad = h;
            sync_signal(SEM_TYH);
        } else {
        }

        sleepms(1000);

        if (loopCnt >= 30) {
            enableHeater = !enableHeater;
            sht31_heater(enableHeater);
            loopCnt = 0;
        }

        loopCnt++;
    }
}