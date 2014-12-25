#define JSV_I2C_ISR_BUSY 0x00008000
#define JSV_I2C_ISR_TC 0x00000040
#define JSV_I2C_ISR_TXE 0x00000001
#define JSV_I2C_ISR_TXIS 0x00000002
#define JSV_I2C_ISR_RNE 0x00000004
#define JSV_I2C_CR2_NACK 0x00008000
#define JSV_I2C_CR2_STOP 0x00004000
#define JSV_I2C_CR2_START 0x00002000
#define JSV_I2C_CR2_NBYTES_OFFSET 16
#define JSV_I2C_CR2_NBYTES_MASK 0x000F0000
#define JSV_I2C_CR2_RELOAD 0x01000000
#define JSV_I2C_CR2_AUTOEND 0x02000000
#define JSV_I2C_CR1_PE 0x00000001
#define JSV_I2C_CR1_ANFOFF 0x00001000
#define JSV_I2C_CR1_DNF_OFFSET 8
#define JSV_I2C_CR1_DNF_MASK 0x00000F00
#define JSV_I2C_CR2_SADD_MASK 0x000003FF
/* the following offset is +1 because we are in 7-bit mode */
#define JSV_I2C_CR2_SADD_OFFSET 1
#define JSV_I2C_CR2_RDWRN 0x00000400
#define JSV_I2C_ICR_NACKCF 0x00000010
#define JSV_I2C_ICR_STOPCF 0x00000020

int read_i2c_register(int addr, int reg, int nbytes, unsigned char *buff) {
	
	int idx;
	volatile long unsigned int status;

	hi2c1.Instance->ISR = JSV_I2C_ISR_TXE | JSV_I2C_ISR_TXIS; /* clear TX flags if they are there */
	hi2c1.Instance->ICR = JSV_I2C_ICR_NACKCF | JSV_I2C_ICR_STOPCF;
	
#ifdef CONFIG_I2C_CR1
	
/* CR1 */
	status = hi2c1.Instance->CR1;
	status &= ~JSV_I2C_CR1_PE; /* PE = 0 */
	hi2c1.Instance->CR1 = status;	
	status &= ~JSV_I2C_CR1_DNF_MASK; /* DNF = 0 No Digital Filter */
	status |= JSV_I2C_CR1_ANFOFF; /* ANFOFF = 1 No Analog Filter */
	hi2c1.Instance->CR1 = status;
	status |= JSV_I2C_CR1_PE; /* PE = 1 */
	hi2c1.Instance->CR1 = status;

#endif

	status = hi2c1.Instance->CR2;
	status &= ~JSV_I2C_CR2_AUTOEND; /* AUTOEND = 0 */
	status &= ~JSV_I2C_CR2_RELOAD; /* RELOAD = 0 */
	status &= ~JSV_I2C_CR2_NBYTES_MASK;
	status |= (1 << JSV_I2C_CR2_NBYTES_OFFSET); /* single byte, namely reg address */
	status &= ~JSV_I2C_CR2_RDWRN; /* write */
	status &= ~JSV_I2C_CR2_SADD_MASK;
	status |= (addr << JSV_I2C_CR2_SADD_OFFSET);
	status |= JSV_I2C_CR2_START;
	hi2c1.Instance->CR2 = status;
	
	while((hi2c1.Instance->ISR & JSV_I2C_ISR_TXE) == 0);
/* the compiler accepts the following code, but it doesn't work */
//	status = hi2c1.Instance->ISR;	
//	while((status & JSV_I2C_ISR_TXE) == 0)
//		status = hi2c1.Instance->ISR;	
	hi2c1.Instance->TXDR = reg;

//	jsv delay_ms(100);
	delay_ms(2);
//	int j = 0;
//	for(idx=0;idx<300000;++idx) ++j;
	
	// status = hi2c1.Instance->CR2;
	status |= JSV_I2C_CR2_AUTOEND; /* AUTOEND = 1 */
	status &= ~JSV_I2C_CR2_RELOAD; /* RELOAD = 0 */
	status &= ~JSV_I2C_CR2_NBYTES_MASK;
	status |= (nbytes << JSV_I2C_CR2_NBYTES_OFFSET);
	status |= JSV_I2C_CR2_RDWRN; /* read */
	status |= JSV_I2C_CR2_START; /* restart */
	hi2c1.Instance->CR2 = status;
	
	for(idx=0;idx<nbytes;++idx) {
		while((hi2c1.Instance->ISR & JSV_I2C_ISR_RNE) == 0);
/* the compiler accepts the following code, but it doesn't work */
//		status = hi2c1.Instance->ISR;
//		while((status & JSV_I2C_ISR_RNE) == 0)
//			status = hi2c1.Instance->ISR;
		buff[idx] = hi2c1.Instance->RXDR;
	}

}

int write_i2c_register(int addr, int reg, int nbytes, unsigned char *buff) {
	
	int idx;
	volatile long unsigned int status;

	hi2c1.Instance->ISR = JSV_I2C_ISR_TXE | JSV_I2C_ISR_TXIS; /* clear TX flags if they are there */
	hi2c1.Instance->ICR = JSV_I2C_ICR_NACKCF | JSV_I2C_ICR_STOPCF;
// jsv	if(hi2c1.Instance->ISR & JSV_I2C_ISR_RNE) status = hi2c1.Instance->RXDR; /* clear the receive channel */
	
#ifdef CONFIG_I2C_CR1
	
/* CR1 */
	status = hi2c1.Instance->CR1;
	status &= ~JSV_I2C_CR1_PE; /* PE = 0 */
	hi2c1.Instance->CR1 = status;	
	status &= ~JSV_I2C_CR1_DNF_MASK; /* DNF = 0 No Digital Filter */
	status |= JSV_I2C_CR1_ANFOFF; /* ANFOFF = 1 No Analog Filter */
	hi2c1.Instance->CR1 = status;
	status |= JSV_I2C_CR1_PE; /* PE = 1 */
	hi2c1.Instance->CR1 = status;

#endif

	status = hi2c1.Instance->CR2;
	status |= JSV_I2C_CR2_AUTOEND; /* AUTOEND = 0 */
	status &= ~JSV_I2C_CR2_RELOAD; /* RELOAD = 0 */
	status &= ~JSV_I2C_CR2_NBYTES_MASK;
	status |= ((1 + nbytes) << JSV_I2C_CR2_NBYTES_OFFSET); /* total bytes to write. 1 for register address */
	status &= ~JSV_I2C_CR2_RDWRN; /* write */
	status &= ~JSV_I2C_CR2_SADD_MASK;
	status |= (addr << JSV_I2C_CR2_SADD_OFFSET);
	status |= JSV_I2C_CR2_START;
	hi2c1.Instance->CR2 = status;
	
	while((hi2c1.Instance->ISR & JSV_I2C_ISR_TXE) == 0);
	hi2c1.Instance->TXDR = reg;
	while(nbytes--) {
		while((hi2c1.Instance->ISR & JSV_I2C_ISR_TXE) == 0);
		hi2c1.Instance->TXDR = *buff++;
	}

#if 0
	delay_ms(100);
//	int j = 0;
//	for(idx=0;idx<300000;++idx) ++j;
	
	// status = hi2c1.Instance->CR2;
	status |= JSV_I2C_CR2_AUTOEND; /* AUTOEND = 1 */
	status &= ~JSV_I2C_CR2_RELOAD; /* RELOAD = 0 */
	status &= ~JSV_I2C_CR2_NBYTES_MASK;
	status |= (nbytes << JSV_I2C_CR2_NBYTES_OFFSET);
	status &= ~JSV_I2C_CR2_RDWRN; /* write */
	status |= JSV_I2C_CR2_START; /* restart */
	hi2c1.Instance->CR2 = status;
	
	for(idx=0;idx<nbytes;++idx) {
		while((hi2c1.Instance->ISR & JSV_I2C_ISR_TXE) == 0);
		hi2c1.Instance->TXDR = buff[idx];
	}
#endif
	
}

