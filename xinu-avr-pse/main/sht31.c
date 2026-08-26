#include <xinu.h>
#include <stdint.h>
#include <stddef.h>
#include <util/delay.h>
#include <math.h> // Para la función NAN
#include "sht31.h"
#include "twi.h"
#include "serial.h"

// Eliminada la declaración conflictiva 'uint8_t *response_data;'
// El buffer global para guardar los datos recibidos (máximo 6 bytes)
uint8_t response_data[6]; 

float sht31_temperature = 0.0f;
float sht31_humidity = 0.0f;
uint8_t sht31_buffer[6];

// Declaración estática de la función CRC
static uint8_t crc8(const uint8_t *data, int len);

/**
 * Función de callback para 'twi_read' cuando se lee el status (3 bytes).
 * Es fundamental COPIAR los datos recibidos (data) al buffer global (response_data).
 * @param adress: Dirección I2C del dispositivo.
 * @param data: Puntero a los datos recibidos (3 bytes: status H, status L, CRC).
 */
void leer(uint8_t adress, uint8_t *data) {
    // Corregido: Se copian 3 bytes para la lectura de status
    for (int i = 0; i < 3; i++) {
        response_data[i] = data[i];
    }
}

/**
 * Función de callback para 'twi_read' cuando se leen temperatura y humedad (6 bytes).
 * @param adress: Dirección I2C del dispositivo.
 * @param data: Puntero a los datos recibidos (6 bytes: T H, T L, CRC, H H, H L, CRC).
 */
void leerb(uint8_t address, uint8_t *data) {
    // Corregido: Se copian 6 bytes para la lectura de medición
    for (int i = 0; i < 6; i++) {
        response_data[i] = data[i];
    }
}

//envio comanod espero 20 y dsps read / si no funciona write, read y dsps espero 20 

uint16_t sht31_read_status() {
    uint8_t cmd[2];
    cmd[0] = (SHT31_READSTATUS >> 8) & 0xFF;
    cmd[1] = SHT31_READSTATUS & 0xFF;

    // 1. Envío del comando de lectura de status
    twi_write(SHT31_DEFAULT_ADDR, cmd, 2, NULL);
//    sleepms(2);
    
    // 2. Lectura de 3 bytes (Status MSB, Status LSB, CRC)
    twi_read(SHT31_DEFAULT_ADDR, 3, leer);
 //   sleepms(2);
    
    // Eliminado: El delay no es necesario después de un comando de lectura de status.

    uint16_t stat = response_data[0];
    stat <<= 8;
    stat |= response_data[1];
    
    // Opcional: Verificar CRC de status aquí si es necesario
    
    return stat;
}

int sht31_begin() {
    sht31_reset();
    // Verifica si el sensor está respondiendo (el status no debe ser 0xFFFF si está presente)
    return sht31_read_status() != 0xFFFF;   
}

void sht31_reset(void) {
    uint8_t cmd[2];
    cmd[0] = (SHT31_SOFTRESET >> 8) & 0xFF;
    cmd[1] = SHT31_SOFTRESET & 0xFF;
    twi_write(SHT31_DEFAULT_ADDR, cmd, 2, NULL);
    _delay_ms(10); // Tiempo necesario para que el sensor se resetee
}

void sht31_clear_status(void) {
    uint8_t cmd[2];
    cmd[0] = (SHT31_CLEARSTATUS >> 8) & 0xFF;
    cmd[1] = SHT31_CLEARSTATUS & 0xFF;
    twi_write(SHT31_DEFAULT_ADDR, cmd, 2, NULL);
    _delay_ms(10);
}

int sht31_heater(int h) {
    uint8_t cmd[2];
    uint16_t command = (h ? SHT31_HEATEREN : SHT31_HEATERDIS);
    cmd[0] = (command >> 8) & 0xFF;
    cmd[1] = command & 0xFF;
    twi_write(SHT31_DEFAULT_ADDR, cmd, 2, NULL);
    _delay_ms(1);
    return 1;
}


int sht31_read_both(float *temperature_out, float *humidity_out) {

    if (!sht31_read_temp_hum()) {
        *temperature_out = NAN;
        *humidity_out = NAN;
        return 0;
    }

    *temperature_out = sht31_temperature;
    *humidity_out = sht31_humidity;
    return 1;
}


int sht31_read_temp_hum(void) {

    uint8_t cmd[2];
    // Comando para medición de disparo único, alta precisión
    cmd[0] = (SHT31_MEAS_HIGHREP >> 8) & 0xFF;
    cmd[1] = SHT31_MEAS_HIGHREP & 0xFF;

    // 1. Enviar comando
    twi_write(SHT31_DEFAULT_ADDR, cmd, 2, NULL);

    // 2. Esperar medición (20ms para alta precisión)
    _delay_ms(20); 

    // 3. Leer 6 bytes: T (2), CRC (1), H (2), CRC (1)
    twi_read(SHT31_DEFAULT_ADDR, 6, leerb);
    _delay_ms(2); // Retardo pequeño opcional

    // 4. Copiar datos al buffer de trabajo
    for (uint8_t i = 0; i < 6; i++)
        sht31_buffer[i] = response_data[i];

    // 5. Verificar CRC
    if (crc8(sht31_buffer, 2) != sht31_buffer[2])
        return 0; // Error de CRC en Temperatura

    if (crc8(sht31_buffer + 3, 2) != sht31_buffer[5])
        return 0; // Error de CRC en Humedad

    // 6. Conversión
    uint16_t rawT = ((uint16_t)sht31_buffer[0] << 8) | sht31_buffer[1];
    uint16_t rawH = ((uint16_t)sht31_buffer[3] << 8) | sht31_buffer[4];

    // Fórmulas de la hoja de datos
    sht31_temperature = -45.0f + 175.0f * ((float)rawT / 65535.0f);
    sht31_humidity    = 100.0f * ((float)rawH / 65535.0f);

    // DEBUG: Imprime valores crudos
    serial_put_string("\r\nDEBUG rawT=");
    serial_put_number(rawT);
    serial_put_string(" rawH=");
    serial_put_number(rawH);
    serial_put_string("\r\n");
    
    // DEBUG: Imprime valores convertidos
    serial_put_string("DEBUG temp=");
    serial_print_float(sht31_temperature);
    serial_put_string(" hum=");
    serial_print_float(sht31_humidity);
    serial_put_string("\r\n");


    return 1;
}

float sht31_read_temperature(void) {
    if (!sht31_read_temp_hum())
        return NAN;

    return sht31_temperature;
}

float sht31_read_humidity(void) {
    if (!sht31_read_temp_hum())
        return NAN;

    return sht31_humidity;
}

int sht31_is_heater_enabled(void) {
    uint16_t regValue = sht31_read_status();

    if (regValue & (1 << SHT31_REG_HEATER_BIT))
        return 1;   // enabled

    return 0;       // disabled
}


static uint8_t crc8(const uint8_t *data, int len) {
    /*
     * CRC-8 formula from SHT31 datasheet
     * Polynomial: 0x31 (x^8 + x^5 + x^4 + 1)
     * Init value: 0xFF
     * Final XOR: 0x00
     */

    const uint8_t POLYNOMIAL = 0x31;
    uint8_t crc = 0xFF;

    for (int j = 0; j < len; j++) {
        crc ^= data[j];

        for (int i = 0; i < 8; i++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ POLYNOMIAL;
            else
                crc <<= 1;
        }
    }

    return crc;
}
