#include <stdint.h> /* para los tipos de datos. Ej.: uint8_t */
                 
#define SHT31_READSTATUS 0xF32D 
#define SHT31_DEFAULT_ADDR 0x44 /**< SHT31 Default Address */
#define SHT31_SOFTRESET 0x30A2
#define SHT31_CLEARSTATUS 0x3041 
#define SHT31_MEAS_HIGHREP_STRETCH 0x2C06 /**< Measurement High Repeatability with Clock Stretch Enabled */
#define SHT31_MEAS_MEDREP_STRETCH 0x2C0D /**< Measurement Medium Repeatability with Clock Stretch Enabled */
#define SHT31_MEAS_LOWREP_STRETCH 0x2C10 /**< Measurement Low Repeatability with Clock Stretch Enabled*/
#define SHT31_MEAS_HIGHREP 0x2400 /**< Measurement High Repeatability with Clock Stretch Disabled */
#define SHT31_MEAS_MEDREP 0x240B /**< Measurement Medium Repeatability with Clock Stretch Disabled */
#define SHT31_MEAS_LOWREP 0x2416 /**< Measurement Low Repeatability with Clock Stretch Disabled */
#define SHT31_HEATEREN 0x306D     /**< Heater Enable */
#define SHT31_HEATERDIS 0x3066    /**< Heater Disable */
#define SHT31_REG_HEATER_BIT 0x0d /**< Status Register Heater Bit */


//variable tipo uint16_t para el comando de lectura de estado, 2 bytes y un null
uint16_t sht31_read_status();   
void sht31_leer(uint8_t adress, uint8_t *data);
void sht31_reset(void);
int sht31_begin();
void leer(uint8_t adress, uint8_t *data);
void sht31_clear_status(void);
int sht31_read_temp_hum(void);
static uint8_t crc8(const uint8_t *data, int len);
int sht31_read_both(float *temperature_out, float *humidity_out);
int sht31_heater(int h);
float sht31_read_temperature(void);
float sht31_read_humidity(void);
int sht31_is_heater_enabled(void);