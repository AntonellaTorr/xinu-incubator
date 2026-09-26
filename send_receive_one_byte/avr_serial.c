/**********************************************************************
 *
 * serial.c - Driver del UART del atmega328p
 *
 * Configuracion: 9600bps, 8bits data, 1bit stop, sin bit de paridad
 *
 **********************************************************************/

 #include <stdint.h> /* para los tipos de datos. Ej.: uint8_t */
 #include <avr/interrupt.h>
 
 typedef struct
 {
         uint8_t status_control_a;    /* ucsr0a USART Control and Status A */
         uint8_t status_control_b;    /* ucsr0b USART Control and Status B */
         uint8_t status_control_c;    /* ucsr0c USART Control and Status C */
         uint8_t _reserved;           /* espacio sin utilizar */
         uint8_t baud_rate_l;         /* ubrr0l baud rate low */
         uint8_t baud_rate_h;         /* ubrr0h baud rate high */
         uint8_t data_es;             /* udr0 i/o data */
 } volatile uart_t;
 
 /* puntero a la estructura de los registros del periferico */
 uart_t *puerto_serial = (uart_t *) (0xc0);
 
 #define USART_BAUDRATE 9600
 #define BAUD_PRESCALE (((F_CPU/(USART_BAUDRATE*16UL)))-1)
 /* F_CPU viene como parametro en el Makefile */
 
 /* Mascaras de bits de configuracion */
 #define CHARACTER_SIZE_0   0x02 /* UCSZ00 (UCSR0C) */
 #define CHARACTER_SIZE_1   0x04 /* UCSZ01 (UCSR0C) */
 #define TRANSMITTER_ENABLE 0x08 /* TXEN0  (UCSR0B) */
 #define RECEIVER_ENABLE    0x10 /* RXEN0  (UCSR0B) */
 #define READY_TO_WRITE     0x20 /* UDRE0  (UCSR0A) */
 #define READY_TO_READ      0x80 /* RXC0   (UCSR0A) */
 #define INTERRUPT_ENABLE   0x80 /* RXCIE0 (UCSR0B) */
 
 #define BUFFER_SIZE 32
 static volatile uint8_t buffer[BUFFER_SIZE];
 static volatile uint8_t head = 0; /* donde se pone el nuevo dato que llega por serial */
 static volatile uint8_t tail = 0; /* de donde get_char saca el proximo dato */
 
 void serial_init(void)
 {
     /* El manual del atmega328p tiene un ejemplo. Adecuarla a C y
        la estructura de datos */
 
     /* Configurar los registros High y Low con BAUD_PRESCALE */
     /* Configurar un frame de 8bits, sin bit de paridad, 1 bit de stop */
     /* Activar la recepcion, transmision e interrupcion de recepcion */
 
     /* Set baud rate */
     puerto_serial->baud_rate_h = (uint8_t)(BAUD_PRESCALE >> 8);
     puerto_serial->baud_rate_l = (uint8_t)BAUD_PRESCALE;
 
     /* Enable receiver, transmitter and RX interrupt */
     puerto_serial->status_control_b = RECEIVER_ENABLE | TRANSMITTER_ENABLE | INTERRUPT_ENABLE;
 
     /* Set frame format: 8 data bits, 1 stop bit, sin paridad */
     puerto_serial->status_control_c = CHARACTER_SIZE_1 | CHARACTER_SIZE_0;
 
     sei();
 }
 
 /* enviar un byte a traves del dispositivo inicializado */
 void serial_put_char(char c)
 {
     /* Se debe esperar verificando el bit UDREn del registro UCSRnA,
        hasta que el buffer este listo para recibir un dato a transmitir */
 
     /* Wait for empty transmit buffer */
     while (!(puerto_serial->status_control_a & READY_TO_WRITE))
         ;
 
     /* Put data into buffer, sends the data */
     puerto_serial->data_es = c;
 }
 
 /* Recepcion bloqueante desde el buffer circular */
 char serial_get_char(void)
 {
     /* Esperar hasta que haya datos en el buffer */
     while (tail == head)
         ;
 
     char c = buffer[tail];
     tail = (tail + 1) % BUFFER_SIZE;
     return c;
 }
 
 /* Comprueba si hay datos pendientes en el buffer circular.
  * OJO: la recepcion se maneja por interrupcion, asi que el ISR ya
  * consume UDR0 y limpia el flag RXC0 de hardware. Por eso, la unica
  * forma correcta de saber si hay datos sin leer es comparar head/tail,
  * NO consultar el bit READY_TO_READ del registro. */
 int serial_new_data(void)
 {
     return (head != tail);
 }
 
 /* Envio de cadenas almacenadas en memoria FLASH (PROGMEM) */
 void serial_put_str2(const __flash char m[])
 {
     while (*m) {
         serial_put_char(*m++);
     }
 
     serial_put_char('\r');
     serial_put_char('\n');
 }
 
 /* Envio de cadena terminando con retorno de carro y salto de linea (\r\n) */
 void serial_put_str(const char *str)
 {
     while (*str) {
         serial_put_char(*str);
         str++;
     }
 
     serial_put_char('\r');
     serial_put_char('\n');
 }
 
 ISR(USART_RX_vect)
 {
     uint8_t data = puerto_serial->data_es;
     uint8_t next_head = (head + 1) % BUFFER_SIZE;
 
     if (next_head != tail) {
         /* Hay lugar: guardar el dato */
         buffer[head] = data;
         head = next_head;
     }
     /* Si no hay lugar, se descarta el dato entrante (buffer lleno) */
 }