#ifndef I2C_H
#define I2C_H

#define SCL_HI 0x00000040
#define SCL_LO 0x00400000
#define SDA_HI 0x00000080
#define SDA_LO 0x00800000

void i2c_delay(void);
void i2c_set_scl(void);
void i2c_clr_scl(void);
void i2c_set_sda(void);
void i2c_clr_sda(void);
int i2c_get_sda(void);
int i2c_get_scl(void);
void i2c_start_condition(void);
void i2c_stop_condition(void);
int i2c_wait_ack(void);
int i2c_wait_nack(void);
void i2c_send_char(unsigned char data);
void i2c_send_ack(void);
void i2c_send_nack(void);
char i2c_read_char(void);
int i2c_wait_scl_hi(void);
int i2c_wait_sda_hi(void);

#endif