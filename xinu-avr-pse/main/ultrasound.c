/*
 * ultrasound: controla el sensor HC-SR04 para medir distancias en cm
 */

 #define __DELAY_BACKWARD_COMPATIBLE__
 #define F_CPU 16000000UL
 #include <xinu.h>
 
 #include <util/delay.h>
 #include "ultrasound.h"
 #include "gpio.h"
 
 void init_ultrasound(int trig, int echo)
 {
     gpio_output(trig);
     gpio_input(echo);
     gpio_pin(trig, OFF);
 }
 
 int ultrasound_get_distance(int trig, int echo)
 {
     int duracion = 0;
     int timeout  = 0;
 
     _delay_us(2);
 
     gpio_pin(trig, ON);
     _delay_us(10);
     gpio_pin(trig, OFF);
 
     while (gpio_pin(echo, GET) == 0) {
         _delay_us(1);
         timeout++;
         if (timeout > 30000) return -1;
     }
 
     while (gpio_pin(echo, GET) == 1) {
         _delay_us(1);
         duracion++;
         if (duracion > 38000) break;
     }
 
   
     
     return duracion / 58.0;
 }