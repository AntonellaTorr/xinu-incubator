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

int tyh(void){
    int loopCnt = 0;
    int enableHeater = 0;
   
    sleepms(5000);
    
    // Verificar si el sensor responde
    if (!sht31_begin()) {
        while(1) {
            // Loop infinito de error
            sleepms(1000);
        }
    }
    serial_put_string("OK\n");
    uint16_t estado = sht31_read_status();
    serial_put_uint16_hex(estado);
    serial_put_string("\n");

    
    float t,h;
    while (1) {
        if (sht31_read_both(&t, &h)) {

            
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