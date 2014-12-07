#include "stm32l0xx_hal.h"
#include "i2c.h"

int i2c_wait_scl_hi() {
	while(i2c_get_scl() == 0); /* wait for clock stretch to be over */	
}

int i2c_wait_sda_hi() {
	while(i2c_get_sda() == 0); /* wait for sda to float hi */	
}

void i2c_delay() {
	int i, j = 0;
	for(i=0;i<300;++i) ++j;
}

void i2c_set_scl() {GPIOB->BSRR = SCL_HI; i2c_delay(); }
void i2c_clr_scl() {GPIOB->BSRR = SCL_LO; i2c_delay(); }
void i2c_set_sda() {GPIOB->BSRR = SDA_HI; i2c_delay(); }
void i2c_clr_sda() {GPIOB->BSRR = SDA_LO; i2c_delay(); }
int i2c_get_sda() { 
	return ((GPIOB->IDR >> 7) & 1); 
}

int i2c_get_scl() { 
	return ((GPIOB->IDR >> 6) & 1); 
}

static int status;

void i2c_start_condition() {
	i2c_clr_scl();
	i2c_set_sda();
	while(i2c_get_sda() == 0); /* wait for SDA = 1 */
	i2c_set_scl();
	i2c_clr_sda();
}

void i2c_stop_condition() {
	i2c_clr_scl();
	i2c_clr_sda();
	i2c_set_scl();
	while(i2c_get_scl() == 0); /* wait for SCL = 1 */
	i2c_set_sda();
	while(i2c_get_sda() == 0); /* wait for SDA = 1 */
}

int i2c_wait_ack() {
	i2c_clr_scl();
	i2c_set_sda();
	i2c_set_scl();
	while(i2c_get_scl() == 0); /* wait for scl = 1 (clock stretching) */
	return (i2c_get_sda() == 0);
}

int i2c_wait_nack() {
	i2c_clr_scl();
	i2c_set_sda();
	i2c_set_scl();
	while(i2c_get_scl() == 0); /* wait for scl = 1 (clock stretching) */
	return (i2c_get_sda() == 1);
}

void i2c_send_char(unsigned char data) {
	unsigned char mask = 0x80;
	int i, x;
	for(i=0;i<8;++i) {
		x = (data & mask);
		i2c_clr_scl();
		if(x) i2c_set_sda();
		else i2c_clr_sda();
		i2c_set_scl();
		mask >>= 1;
	}
}

void i2c_send_ack() {
		i2c_clr_scl();
		i2c_clr_sda();
		i2c_set_scl();
}

void i2c_send_nack() {
		i2c_clr_scl();
		i2c_set_sda();
		i2c_set_scl();
}

char i2c_read_char() {
	int i;
	char acc = 0;
	
	i2c_clr_scl();
	i2c_set_sda();
	for(i=0;i<8;++i) {
		i2c_clr_scl();
		i2c_set_scl();
		acc = (acc << 1) | i2c_get_sda();
	}
	
	return acc;
}
