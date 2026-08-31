
#include <xinu.h>
#include "tarea_controltyh.h"
#include "tarea_controlagua.h"
#include "serial.h"
#include "estado.h"
#include "twi.h"
int main(void)
{
	serial_init();
    twi_init();
	resume(create(tyh, 256, 20, "tyh", 0));
	resume(create(agua, 128, 20, "agua", 0));

    for (;;) {
		serial_put_string("Temperatura: ");
		serial_print_float(estado.temperatura);
		serial_put_string("Distancia al agua: ");
		serial_print_float(estado.distancia_agua);
		serial_put_string(" cm\n");
		sleep(1);
	}
	return 0;
}

