
#include <xinu.h>

#define __DELAY_BACKWARD_COMPATIBLE__
#define F_CPU 16000000UL

#include "tarea_controltyh.h"
#include "serial.h"
#include "estado.h"
#include "ultrasound.h"
#include <stdint.h>
#include <util/delay.h>
#include <math.h>
#include "tarea_controlagua.h"
int main(void)
{
	serial_init();
    twi_init();
	//resume(create(tyh, 512, 20, "tyh", 0));
	resume(create(agua, 256, 20, "agua", 0));

    for (;;) {
		/*serial_put_string("Temperatura: ");
		serial_print_float(estado.temperatura);
		serial_put_string("Distancia al agua: ");*/
		serial_print_float(estado.distancia_agua);
		serial_put_string(" cm\n");
		sleep(1);
	}
	return 0;
}

