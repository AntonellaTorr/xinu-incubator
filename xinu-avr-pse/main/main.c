
#include <xinu.h>
#include "tarea_controltyh.h"
#include "serial.h"

int main(void)
{
	serial_init();
    twi_init();
	resume(create(tyh, 512, 20, "controltyh", 0));

    for (;;);
	return 0;
}

