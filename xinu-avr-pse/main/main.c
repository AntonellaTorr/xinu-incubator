#include <xinu.h>
#include "tarea_controltyh.h"
#include "serial.h"


extern int control_tyh(void);

float temperatura;
float humedad;

int main(void)
{
	serial_init();

	sync_set(SEM_TYH, 1);   /* arranca "libre", antes de resume */


	resume(create(control_tyh, 256, 20, "controltyh", 0));


	while(1) {
		sleepms(1);
		sync_wait(SEM_TYH);
			float t_local = temperatura;
			float h_local = humedad;
		sync_signal(SEM_TYH);		
		/*serial_put_string("T: ");
		serial_print_float(t_local);
		serial_put_string("H: ");
		serial_print_float(h_local);*/
	}

	return 0;
}
