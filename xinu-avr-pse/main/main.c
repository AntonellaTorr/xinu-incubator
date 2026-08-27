
#include <xinu.h>
#include "tarea_controltyh.h"
#include "serial.h"
#include "estado.h"
int main(void)
{
	serial_init();
    twi_init();
	resume(create(tyh, 512, 20, "controltyh", 0));

    for (;;) {
		serial_put_string("Temperatura: ");
		serial_print_float(estado.temperatura);
		sleep(1);
	}
	return 0;
}

