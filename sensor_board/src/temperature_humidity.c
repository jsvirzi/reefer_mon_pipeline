#include "i2c.h"
#include "temperature_humidity.h"

#define MEASURE_HUMIDITY 0xe5
#define MEASURE_TEMPERATURE 0xe3
#define MEASURE_TEMPERATURE_FROM_HUMIDITY 0xe0

int read_temperature_and_humidity(int *temperature, int *humidity) {
	int msb, lsb, res;
	i2c_start_condition();
	i2c_send_char(0x80);
	i2c_wait_ack();
	i2c_send_char(MEASURE_HUMIDITY);
	i2c_wait_ack();
	i2c_start_condition();
	i2c_send_char(0x81);
	i2c_wait_ack();
	if(i2c_wait_scl_hi() == 0) return -1;
	msb = i2c_read_char();
	i2c_send_ack();
	lsb = i2c_read_char();
	i2c_send_nack();
	i2c_stop_condition();
	res = (msb << 8) | lsb;
	*humidity = res;

	i2c_start_condition();
	i2c_send_char(0x80);
	i2c_wait_ack();
	i2c_send_char(MEASURE_TEMPERATURE_FROM_HUMIDITY);
	i2c_wait_ack();
	i2c_start_condition();
	i2c_send_char(0x81);
	i2c_wait_ack();
	msb = i2c_read_char();
	i2c_send_ack();
	lsb = i2c_read_char();
	i2c_send_nack();
	i2c_stop_condition();
	res = (msb << 8) | lsb;
	*temperature = res;
	
	return 0;
}

int read_humidity() {
	i2c_start_condition();
	i2c_send_char(0x80);
	i2c_wait_ack();
	i2c_send_char(MEASURE_HUMIDITY);
	i2c_wait_ack();
	i2c_start_condition();
	i2c_send_char(0x81);
	i2c_wait_ack();
	if(i2c_wait_scl_hi() == 0) return -1;
	int msb = i2c_read_char();
	i2c_send_ack();
	int lsb = i2c_read_char();
	i2c_send_nack();
	i2c_stop_condition();
	int res = (msb << 8) | lsb;
	return res;
}

int read_temperature() {
	i2c_start_condition();
	i2c_send_char(0x80);
	i2c_wait_ack();
	i2c_send_char(MEASURE_TEMPERATURE);
	i2c_wait_ack();
	i2c_start_condition();
	i2c_send_char(0x81);
	i2c_wait_ack();
	if(i2c_wait_scl_hi() == 0) return -1;
	int msb = i2c_read_char();
	i2c_send_ack();
	int lsb = i2c_read_char();
	i2c_send_nack();
	i2c_stop_condition();
	int res = (msb << 8) | lsb;
	return res;
}

#if 0
/* no hold master mode. not working */
int read_temperature() {
	i2c_start_condition();
	i2c_send_char(0x80);
	i2c_wait_ack();
	i2c_send_char(0xf3);
	i2c_wait_ack();
	i2c_start_condition();
	i2c_send_char(0x81);
	i2c_wait_nack();
	i2c_send_char(0x81);
	i2c_wait_nack();
	i2c_send_char(0x81);
	i2c_wait_nack();
	status = i2c_read_char();
	i2c_send_ack();
	status = i2c_read_char();
	i2c_send_nack();
	i2c_stop_condition();
}
#endif

int read_temperature_sensor_firmware() {
	int status;
	i2c_start_condition();
	i2c_send_char(0x80);
	i2c_wait_ack();
	i2c_send_char(0x84);
	i2c_wait_ack();
	i2c_send_char(0xb8);
	i2c_wait_ack();
	i2c_start_condition();
	i2c_send_char(0x81);
	i2c_wait_ack();
	status = i2c_read_char();
	i2c_wait_ack();
	i2c_wait_nack();
	i2c_stop_condition();
}

int read_temperature_sensor_serial_number() {
	int status;
	i2c_start_condition();
	i2c_send_char(0x80);
	i2c_wait_ack();
	i2c_send_char(0xfc);
	i2c_wait_ack();
	i2c_send_char(0xc9);
	i2c_wait_ack();
	i2c_start_condition();
	i2c_send_char(0x81);
	i2c_wait_ack();
	status = i2c_read_char();
	i2c_send_ack();
	status = i2c_read_char();
	i2c_send_ack();
	status = i2c_read_char();
	i2c_send_ack();
	status = i2c_read_char();
	i2c_send_ack();
	status = i2c_read_char();
	i2c_send_ack();
	status = i2c_read_char();
	i2c_send_nack();
	i2c_stop_condition();
	
}

int read_temperature_user_register() {
	int status;
	i2c_start_condition();
	i2c_send_char(0x80);
	i2c_wait_ack();
	i2c_send_char(0xe7);
	i2c_wait_ack();
	i2c_start_condition();
	i2c_send_char(0x81);
	i2c_wait_ack();
	status = i2c_read_char();
	i2c_send_nack();
	i2c_stop_condition();
}

