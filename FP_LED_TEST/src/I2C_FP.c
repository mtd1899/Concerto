/*
I2C_FP.c


	I2C connects to the led drive ICs PCA9956
	
*/

// CODE originally FROM http://codinglab.blogspot.com/2008/10/i2c-on-avr-using-bit-banging.html

/*
// Port for the I2C
#define I2C_DDR DDRD
#define I2C_PIN PIND
#define I2C_PORT PORTD

// Pins to be used in the bit banging
#define I2C_CLK 0
#define I2C_DAT 1
*/

#include <asf.h>
#include "fp.h"

void get_FP_ledQ(uint8_t *ptr);
extern struct FPtxQ_def FPledQ;

uint8_t LED_I2C_Write(uint8_t c);
uint8_t LED_I2C_Read(uint8_t ack);
void LEDs_Init(void);
void I2C_WriteBit(bool bval);
void Do_LEDs(void);
void delay(uint32_t delayCntt);
void LED_I2C_Stop(void);
void LED_I2C_Start(void);
void LED_I2C_Init(void);
bool I2C_ReadBit(void);


#define LED_FP_SDA 	PIN_PB03	
#define LED_FP_SCL 	PIN_PB02

#define I2C_CLOCK_HI() 	port_pin_set_output_level(LED_FP_SCL,1);
#define I2C_CLOCK_LO()	port_pin_set_output_level(LED_FP_SCL,0);

volatile uint32_t delayCnt;

void delay(uint32_t delayCntt)						// some delay for clock
{	delayCnt = delayCntt;
	while (delayCntt>0) delayCntt--;
}


void LED_I2C_Init(void)
{
	/* Initialize board hardware */
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	// SDA
	pin_conf.direction    = PORT_PIN_DIR_OUTPUT_WTH_READBACK;
	port_pin_set_config(LED_FP_SDA, &pin_conf);
	port_pin_set_output_level(LED_FP_SDA, 1);
	// SCL 
	pin_conf.direction    = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_FP_SCL, &pin_conf);
	port_pin_set_output_level(LED_FP_SCL, 1);
	
}
	

void I2C_WriteBit(bool bval)
{
	port_pin_set_output_level(LED_FP_SDA,bval);
    I2C_CLOCK_HI();

    delay(1);

    I2C_CLOCK_LO();
    delay(1);


}
bool lastI2C_ReadBit;

bool I2C_ReadBit(void)
{
    port_pin_set_output_level(LED_FP_SDA,1); //I2C_DATA_HI();

    I2C_CLOCK_HI();
    delay(1);

    lastI2C_ReadBit = port_pin_get_input_level(LED_FP_SDA);

    I2C_CLOCK_LO();
    delay(1);

    return lastI2C_ReadBit;
}

/*
// START / STOP
Both data and clock lines remain HIGH when the bus is not busy. A HIGH-to-LOW
transition of the data line while the clock is HIGH is defined as the START condition (S). A
LOW-to-HIGH transition of the data line while the clock is HIGH is defined as the STOP
condition*/

// Send a START Condition
//
void LED_I2C_Start(void)
{
    // set both to high at the same time
	port_pin_set_output_level(LED_FP_SDA,1);	//I2C_DDR &= ~ ((1 << I2C_DAT) | (1 << I2C_CLK));
	port_pin_set_output_level(LED_FP_SCL,1);
    delay(1);

	port_pin_set_output_level(LED_FP_SDA,0);	// I2C_DATA_LO()
    delay(1);
	I2C_CLOCK_LO();
    delay(1);
}

// Send a STOP Condition
//
void LED_I2C_Stop(void)
{
	port_pin_set_output_level(LED_FP_SDA,0);
    delay(1);    
	
	I2C_CLOCK_HI();
    delay(1);

	port_pin_set_output_level(LED_FP_SDA,1);	//I2C_DATA_HI();
    delay(1);
}


// write a byte to the I2C slave device
//
uint8_t LED_I2C_Write(uint8_t c)
{
    for (uint8_t i = 0; i < 8; i++)
    {			
		if(c & 0x80)I2C_WriteBit(true);
		else I2C_WriteBit(false);
        c <<= 1;
    }

    return I2C_ReadBit();
    //return 0;
}


// read a byte from the I2C slave device
//
uint8_t LED_I2C_Read(uint8_t ack)
{
    uint8_t res = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        res <<= 1;
        res |= I2C_ReadBit();
    }

    if (ack > 0)
    {
        I2C_WriteBit(0);
    }
    else
    {
        I2C_WriteBit(1);
    }

    delay(1);

    return res;
}

/*
void LEDs_Write(uint8_t address)
{		// PWMs
		LED_I2C_Start();
		LED_I2C_Write(address);		// addr
		LED_I2C_Write(0x8a);	// control
		for (uint8_t i = PWM_REGISTER_START; i< PWM_REGISTER_START+24; i++)
		{
			LED_I2C_Write(regs[i]);
		}
		LED_I2C_Stop();
}*/

uint8_t LED_Vals[3*24];		// brightness values 0-255
uint8_t regs[100];
void LEDs_Init(void)						// init all 3 ICs
{
	uint8_t i;

	LED_I2C_Init();
	// read the default values
	LED_I2C_Start();
	LED_I2C_Write(3);		// addr + read bit
	for (i=0; i<0x3e; i++) regs[i]=LED_I2C_Read(1);
	regs[i]=LED_I2C_Read(0);
	LED_I2C_Stop();

	// load IREF_REGISTERs to max
	for (i=IREF_REGISTER_START; i<IREF_REGISTER_START+24; i++) regs[i]= 0xff;

	// write them back
	LED_I2C_Start();
	LED_I2C_Write(LED_1_24_ADDRESS);		// addr
	LED_I2C_Write(0x80);	// auto inc		
	for (i=0; i<0x3f; i++) 	LED_I2C_Write(regs[i]);
	LED_I2C_Stop();		

	// write them back
	LED_I2C_Start();
	LED_I2C_Write(LED_25_48_ADDRESS);		// addr
	LED_I2C_Write(0x80);	// auto inc		
	for (i=0; i<0x3f; i++) 	LED_I2C_Write(regs[i]);
	LED_I2C_Stop();		

	// write them back
	LED_I2C_Start();
	LED_I2C_Write(LED_49_61_ADDRESS);		// addr
	LED_I2C_Write(0x80);	// auto inc		
	for (i=0; i<0x3f; i++) 	LED_I2C_Write(regs[i]);
	LED_I2C_Stop();
	
	LED_I2C_Start();
	LED_I2C_Write(3);		// addr + read bit
	for (i=0; i<0x3e; i++) regs[i]=LED_I2C_Read(1);
	regs[i]=LED_I2C_Read(0);
	LED_I2C_Stop();
			
}

uint8_t ledState=0, ledAddr = LED_1_24_ADDRESS, ledByteCnt=0, *ledValPtr;

uint8_t entryCnt;



void Do_LEDs(void)							// writes 1 byte per call
{	uint8_t i, ledCmd;
	if (FPledQ.length>0)
	{
		get_FP_ledQ(&ledCmd);
		if ((ledCmd & FP_LED_OFF_FLAG_MASK) > 64)
		{	
			ledCmd = 0;
		}
		switch(ledCmd)
		{
			case 0:
				break;
			case FP_LED_ALL_OFF:
				for (i=0; i<64; i++)
				{
					LED_Vals[i]=0;
				}
				break;
			case FP_LED_NO_CHANGE:
				break;
			default:
				if (ledCmd & FP_LED_OFF_FLAG)
				{	ledCmd ^= FP_LED_OFF_FLAG; 
					LED_Vals[ledCmd-1]=0;
				}
				else if (ledCmd<64)
				{	LED_Vals[ledCmd-1]=LED_BRIGHTNESS;
				}
		}
	}


	entryCnt++;
	if (ledState == 0)
	{							// PWMs
		LED_I2C_Start();
		LED_I2C_Write(ledAddr);		// addr
		LED_I2C_Write(0x8a);	// control
		if(ledAddr==LED_1_24_ADDRESS) ledValPtr = LED_Vals;
		ledState=1;
		ledByteCnt=0;
	}
	else
	{	LED_I2C_Write(*ledValPtr);
		ledByteCnt++;
		ledValPtr++;
		if (ledByteCnt == 24)
		{	LED_I2C_Stop();
			ledState=0;
			switch (ledAddr)
			{
				case LED_1_24_ADDRESS:
					ledAddr = LED_25_48_ADDRESS;
					break;
				case LED_25_48_ADDRESS:
					ledAddr = LED_49_61_ADDRESS;
					break;
				case LED_49_61_ADDRESS:
					ledAddr = LED_1_24_ADDRESS;
					break;
				default:
					ledAddr = LED_1_24_ADDRESS;
			}
		}
	}
}




/*
ORIGINAL CODE

// Port for the I2C
#define I2C_DDR DDRD
#define I2C_PIN PIND
#define I2C_PORT PORTD

// Pins to be used in the bit banging
#define I2C_CLK 0
#define I2C_DAT 1

#define I2C_DATA_HI()\
I2C_DDR &= ~ (1 << I2C_DAT);\
I2C_PORT |= (1 << I2C_DAT);
#define I2C_DATA_LO()\
I2C_DDR |= (1 << I2C_DAT);\
I2C_PORT &= ~ (1 << I2C_DAT);

#define I2C_CLOCK_HI()\
I2C_DDR &= ~ (1 << I2C_CLK);\
I2C_PORT |= (1 << I2C_CLK);
#define I2C_CLOCK_LO()\
I2C_DDR |= (1 << I2C_CLK);\
I2C_PORT &= ~ (1 << I2C_CLK);

void I2C_WriteBit(uint8_t c)
{
	if (c > 0)
	{
		I2C_DATA_HI();
	}
	else
	{
		I2C_DATA_LO();
	}

	I2C_CLOCK_HI();
	delay(1);

	I2C_CLOCK_LO();
	delay(1);

	if (c > 0)
	{
		I2C_DATA_LO();
	}

	delay(1);
}

uint8_t I2C_ReadBit()
{
	I2C_DATA_HI();

	I2C_CLOCK_HI();
	delay(1);

	uint8_t c = I2C_PIN;

	I2C_CLOCK_LO();
	delay(1);

	return (c >> I2C_DAT) & 1;
}

// Inits bitbanging port, must be called before using the functions below
//
void I2C_Init()
{
	I2C_PORT &= ~ ((1 << I2C_DAT) | (1 << I2C_CLK));

	I2C_CLOCK_HI();
	I2C_DATA_HI();

	delay(1);
}

// Send a START Condition
//
void I2C_Start()
{
	// set both to high at the same time
	I2C_DDR &= ~ ((1 << I2C_DAT) | (1 << I2C_CLK));
	delay(1);

	I2C_DATA_LO();
	delay(1);

	I2C_CLOCK_LO();
	delay(1);
}

// Send a STOP Condition
//
void I2C_Stop()
{
	I2C_CLOCK_HI();
	delay(1);

	I2C_DATA_HI();
	delay(1);
}

// write a byte to the I2C slave device
//
uint8_t I2C_Write(uint8_t c)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		I2C_WriteBit(c & 128);

		c <<= 1;
	}

	//return I2C_ReadBit();
	return 0;
}


// read a byte from the I2C slave device
//
uint8_t I2C_Read(uint8_t ack)
{
	uint8_t res = 0;

	for (uint8_t i = 0; i < 8; i++)
	{
		res <<= 1;
		res |= I2C_ReadBit();
	}

	if (ack > 0)
	{
		I2C_WriteBit(0);
	}
	else
	{
		I2C_WriteBit(1);
	}

	delay(1);

	return res;
}

*/












