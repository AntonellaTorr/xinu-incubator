/**********************************************************************
 * serial.c - Driver del UART del ATmega328P
 *
 * META: Ocultar el hardware a la aplicacion
 * Configuracion: 9600bps, 8 data bits, 1 stop bit, sin paridad
 **********************************************************************/

 #include <stdint.h>
 #include <avr/interrupt.h>
 
 typedef struct {
     uint8_t status_control_a; /* UCSR0A: Control y Estado A */
     uint8_t status_control_b; /* UCSR0B: Control y Estado B */
     uint8_t status_control_c; /* UCSR0C: Control y Estado C */
     uint8_t _reserved;        /* Espacio reservado */
     uint8_t baud_rate_l;     /* UBRR0L: Baud rate low */
     uint8_t baud_rate_h;     /* UBRR0H: Baud rate high */
     uint8_t data_es;         /* UDR0:   E/S de datos */
 } volatile uart_t;
 
 /* Puntero mapeado a los registros USART0 (0xC0 en memoria SRAM) */
 uart_t *puerto_serial = (uart_t *)(0xC0);
 
 #define USART_BAUDRATE     9600
 #define BAUD_PRESCALE      (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
 
 /* Mascaras de bits de configuracion */
 #define CHARACTER_SIZE_0   0x02 /* UCSZ00 (UCSR0C) */
 #define CHARACTER_SIZE_1   0x04 /* UCSZ01 (UCSR0C) */
 #define TRANSMITTER_ENABLE 0x08 /* TXEN0  (UCSR0B) */
 #define RECEIVER_ENABLE    0x10 /* RXEN0  (UCSR0B) */
 #define READY_TO_WRITE     0x20 /* UDRE0  (UCSR0A) */
 #define READY_TO_READ      0x80 /* RXC0   (UCSR0A) */
 #define INTERRUPT_ENABLE   0x80 /* RXCIE0 (UCSR0B) */
 
 /* Buffer circular para recepcion por interrupcion */
 #define BUFFER_SIZE 32  
 static volatile uint8_t buffer[BUFFER_SIZE];
 static volatile uint8_t head = 0;
 static volatile uint8_t tail = 0;
 
 void serial_init(void) {
     /* Configurar velocidad */
     puerto_serial->baud_rate_h = (uint8_t)(BAUD_PRESCALE >> 8);
     puerto_serial->baud_rate_l = (uint8_t)(BAUD_PRESCALE);
 
     /* Habilitar RX, TX e interrupcion de recepcion */
     puerto_serial->status_control_b = RECEIVER_ENABLE | TRANSMITTER_ENABLE | INTERRUPT_ENABLE;
 
     /* Formato: 8 bits de datos, 1 stop bit, sin paridad */
     puerto_serial->status_control_c = CHARACTER_SIZE_1 | CHARACTER_SIZE_0;
 
     sei();
 }
 
 /* Transmision basica de un caracter */
 void serial_put_char(char c) {
     while (!(puerto_serial->status_control_a & READY_TO_WRITE))
         ;
     puerto_serial->data_es = c;
 }
 
 /* Recepcion bloqueante desde el buffer circular */
 char serial_get_char(void) {
     while (tail == head)
         ;
     char c = buffer[tail];
     tail = (tail + 1) % BUFFER_SIZE;
     return c;
 }
 
 /* Comprueba si hay datos pendientes en el ring buffer */
 int serial_new_data(void) {
     return (head != tail);
 }
 
 /* Envio de cadena simple terminada en null */
 void serial_put_string(const char *s) {
     while (*s) {
         serial_put_char(*s++);
     }
 }
 
 /* Envio de cadena terminando con retorno de carro y salto de linea (\r\n) */
 void serial_put_str(const char *str) {
     serial_put_string(str);
     serial_put_char('\r');
     serial_put_char('\n');
 }
 
 /* Envio de cadenas almacenadas en memoria FLASH (PROGMEM) */
 void serial_put_str2(const __flash char m[]) {
     while (*m) {
         serial_put_char(*m++);
     }
     serial_put_char('\r');
     serial_put_char('\n');
 }
 
 /* Envio de dos digitos decimales (ej: 05, 12, 99) */
 void serial_put_two_digits(uint8_t n) {
     serial_put_char('0' + ((n / 10) % 10));
     serial_put_char('0' + (n % 10));
 }
 
 /* Envio de enteros (maneja valores positivos, negativos y cero) */
 void serial_put_number(long num) {
     char buf[11]; // Soporta hasta -2147483648
     int i = 0;
 
     if (num == 0) {
         serial_put_char('0');
         return;
     }
 
     if (num < 0) {
         serial_put_char('-');
         num = -num;
     }
 
     while (num > 0) {
         buf[i++] = (num % 10) + '0';
         num /= 10;
     }
 
     while (i--) {
         serial_put_char(buf[i]);
     }
 }
 
 /* Envio de flotantes con precision fija de 2 decimales */
 void serial_print_float(float value) {
     if (value < 0) {
         serial_put_char('-');
         value = -value;
     }
 
     long entero = (long)value;
     long decimals = (long)((value - (float)entero) * 100.0f);
 
     serial_put_number(entero);
     serial_put_char('.');
     serial_put_two_digits((uint8_t)decimals);
     serial_put_char('\r');
     serial_put_char('\n');
 }
 
 /* Envio de entero de 16 bits en formato hexadecimal (ej: 0x0A3F) */
 void serial_put_uint16_hex(uint16_t num) {
     const char hex_digits[] = "0123456789ABCDEF";
     char buf[4];
 
     for (int i = 3; i >= 0; i--) {
         buf[i] = hex_digits[num & 0x0F];
         num >>= 4;
     }
 
     serial_put_char('0');
     serial_put_char('x');
     for (int i = 0; i < 4; i++) {
         serial_put_char(buf[i]);
     }
 }
 
 /* Rutina de Servicio de Interrupcion por recepcion UART */
 ISR(USART_RX_vect) {
     uint8_t data = puerto_serial->data_es;
     uint8_t next_head = (head + 1) % BUFFER_SIZE;
 
     if (next_head != tail) {
         buffer[head] = data;
         head = next_head;
     }
 }
