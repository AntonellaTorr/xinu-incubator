



#include <stdint.h>
#define __DELAY_BACKWARD_COMPATIBLE__
#define F_CPU 16000000UL
#include <util/delay.h>
#include <math.h>
#include "ultrasound.h"
#include "estado.h"
#include "serial.h"

int agua(void)
{
    init_ultrasound(9, 10); // trig en D9, echo en D10

    while (1) {
        int distance = ultrasound_get_distance(9, 10);
        if (distance >= 0) {
            estado.distancia_agua = distance;
        
        } else {
        }
        _delay_ms(100);
    }

}
   


