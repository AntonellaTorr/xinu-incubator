/**********************************************************************
 *
 * Filename:    serial.h
 * 
 * API para la aplicacion embebida 
 * META : ocultar el hardware a la aplicacion 
 *
 **********************************************************************/
 #include <stdint.h>  
#ifndef _SERIAL_H
#define _SERIAL_H


void serial_init(void);
void serial_put_char(char c);
char serial_get_char(void);
void serial_put_number(int val);
void serial_put_string(const char* s);
void serial_put_two_digits(uint8_t n);
void serial_print_float(float num);

#endif /* _SERIAL_H */
