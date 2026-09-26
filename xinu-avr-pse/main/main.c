
#include <xinu.h>
#include "tarea_controltyh.h"
#include "tarea_controlagua.h"
#include "serial.h"
#include "estado.h"
#include "twi.h"
#include "wifi.h"
#include <stdlib.h>
int main(void)

{
	int n;
	char c;

	serial_init();

	sleepms(2000);

	wifi_init_server();

	n = wait_connection();
	cipsend_one_byte(n, 'A');
	while(1) {

		c = wait_byte();
		cipsend_one_byte(n, c);


	}


	/*	while (1) {

		c = wait_byte();
	
		if (c == '1') {
	
			int t = (int)estado.temperatura;
	
			cipsend_one_byte(n, t);
		}
	}
*/
  



	/*
    twi_init();
	resume(create(tyh, 256, 20, "tyh", 0));
	resume(create(agua, 128, 20, "agua", 0));
	serial_init(); //HABILITAR LAS INTERRUPCIONES DE SERIAL

    for (;;) {
		//ACA IRIA CODIGO WIFI 
		serial_put_string("Temperatura: ");
		serial_print_float(estado.temperatura);
		serial_put_string("Distancia al agua: ");
		serial_print_float(estado.distancia_agua);
		serial_put_string(" cm\n");
		sleep(1);
	}
	return 0;*/
}

