
#include <xinu.h>

#include <stdint.h>
#include <math.h>
#include "twi.h"
#include "sht31.h"
#include "serial.h"

extern float sht31_temperature;
extern float sht31_humidity;

//Se conecta scl a A5 y sda a A4

int main(void){
    int loopCnt = 0;
    int enableHeater = 0;
    
    
    serial_init();

    twi_init();
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
//    while(1);
    //sensor ok
    
    while (1) {
     float t, h;
        if (sht31_read_both(&t, &h)) {
            //definir comportoamiento del heater
        } else {
            //  No se pudo leer el sensor, manejar error
        }
        
		sleep(1);
        
        // Cada 30 iteraciones (~30 segundos) alternar heater
        if (loopCnt >= 30) {
            enableHeater = !enableHeater;
            sht31_heater(enableHeater);            
            loopCnt = 0;
        }
        
        loopCnt++;
    }
        

}
