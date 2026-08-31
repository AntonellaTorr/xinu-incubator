


#include <xinu.h>
#include <stdint.h>
#include "ultrasound.h"
#include "estado.h"

int agua(void)
{
    init_ultrasound(9, 10); // trig en D9, echo en D10

    while (1) {
        int distance = ultrasound_get_distance(9, 10);
        if (distance >= 0) {
            estado.distancia_agua = distance;
        
        } else {
        }
        sleepms(100);
    }

}
   


