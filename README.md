# flipperzero-i2ctools

```c
typedef struct 
{
  unsigned int PIN0:1;
  unsigned int PIN1:1;
  unsigned int PIN2:1;
  unsigned int PIN3:1;
  unsigned int PIN4:1;
  unsigned int PIN5:1;
  unsigned int PIN6:1;
  unsigned int PIN7:1;
} PORT;

/* TODO: Example address shown, but the proper address */
#define PORT0 *(volatile PORT *)0x1234

/* Define the port used for I2C data and clk as shown above to access them pin wise */
#define I2C_DATA PORT0.PIN0
#define I2C_CLK  PORT0.PIN1

#define HIGH 1
#define LOW  0

/* I2C Start - bit bang */
void I2C_START(void)
{
    /* I2C Start condition, data line goes low when clock is high */
    I2C_DATA = HIGH;
    I2C_CLK = HIGH;
    I2C_DATA = LOW;
    I2C_CLK = LOW;
}

/* I2C Stop - bit bang */
void I2C_STOP (void)
{
    /* I2C Stop condition, clock goes high when data is low */
    I2C_CLK = LOW;
    I2C_DATA = LOW;
    I2C_CLK = HIGH;
    I2C_DATA = HIGH;
}

/* I2C Write - bit bang */
void I2C_WRITE(unsigned char data)
{
	unsigned char outBits;
	unsigned char inBit;
	
 	/* 8 bits */
	for(outBits = 0; outBits < 8; outBits++) 
	{
	    if(data & 0x80)
		    I2C_DATA = 1;
		else
		    I2C_DATA = 0;
      	data  <<= 1;
		/* Generate clock for 8 data bits */
		SCLK = HIGH;
		SCLK = LOW;					
	}
	
	/* Generate clock for ACK */
	I2C_CLK = HIGH;
        /* Wait for clock to go high, clock stretching */
        while(I2C_CLK);
        /* Clock high, valid ACK */
	inBit = I2C_DATA;
	I2C_CLK = LOW;					
}

unsigned char I2C_READ (void)
{
	unsigned char inData, inBits;

	inData = 0x00;
	/* 8 bits */
	for(inBits = 0; inBits < 8; inBits++)
	{
		inData <<= 1;
		I2C_CLK = HIGH;
      	inData |= I2C_DATA;
		I2C_CLK = LOW;					
	}

   return inData;
}

/* Examble for writing to I2C Slave */
void writeI2CSlave (unsigned char data)	
{
    /* Start */
  	I2C_START();
	/* Slave address */
   	I2C_WRITE(0xAA)
	/* Slave control byte */
   	I2C_WRITE(0xBB);
	/* Slave data */
   	I2C_WRITE(data);
	/* Stop */
   	I2C_STOP();
}

/* Examble for reading from I2C Slave */
unsigned char readI2CSlave(unsigned char data)
{
   	unsigned char inData;

	/* Start */
  	I2C_START(); 
	/* Slave address */
   	I2C_WRITE(0xAA);
	/* Slave control byte */
   	I2C_WRITE(data);
	/* Stop */
   	I2C_STOP();
	
	/* Start */
   	I2C_START();
	/* Slave address + read */
   	I2C_WRITE(0xAA | 1);
	/* Read */
	inData = I2C_READ();

   	return inData;                 
}
```