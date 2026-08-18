#include <xinu.h>
#include "tarea_controltyh.h"
#include "serial.h"


extern int control_tyh(void);

float temperatura;
float humedad;

int main(void)
{
	serial_init();
	sync_set(SEM_TYH, 1);  
	resume(create(control_tyh, 512, 20, "controltyh", 0));
	sleepms(1);
	while(1) {
		
		sync_wait(SEM_TYH);
			float t_local = temperatura;
			float h_local = humedad;
		sync_signal(SEM_TYH);		
	}

	return 0;
}
