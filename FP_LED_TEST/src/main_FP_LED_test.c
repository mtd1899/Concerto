/*
 * FP(front panel) MAIN
 
 
 see "therory_of_operation_treble.h"
 
*/
#include <asf.h>
#include "therory_of_operation_fp.h"
#include "fp.h"
#include <port.h>



volatile bool transfer_complete_spi_slave = false;
struct spi_module spi_slave_instance;

extern uint8_t LED_Vals[3*24];		// brightness values 0-255
extern struct FPrxQ_def FPswQ;
extern struct FPtxQ_def FPledQ;
extern struct tc_module tc7_instance;

void configure_tc7(void);
void configure_tc7_callbacks(void);

void configure_spi_slave_callbacks(void);
void configure_spi_slave(void);
void put_FP_swQ (uint8_t val);

void configure_wdt(void);
void readRow(uint8_t swState);

void spi_transceive_one(struct spi_module *const module,uint8_t *tx_data,uint8_t *rx_data,uint16_t length);
void clock_dfll(void);
void configure_dfll_open_loop(void);

void fp_board_init(void);
uint8_t get_ledTimer(void);

uint8_t LED_I2C_Write(uint8_t c);
uint8_t LED_I2C_Read(uint8_t ack);
void LED_I2C_Stop();
void LED_I2C_Start();

void LEDs_Init(void);						// init all 3 ICs
void Do_LEDs(void);							// writes 1 byte per call

/*
	uint8_t velocityCmdRx = 0xf7;			// operates in either the velocity mode or switch closure mode

//  debug vars
 	uint32_t sysHz;
	uint32_t vel_val[8];
	uint8_t testCnt, send =0;				// operates in either the velocity mode or switch closure mode
	uint8_t noteOffset;
*/


enum status_code statusCode;
extern uint8_t ledTimer;
uint8_t swCnt;

uint8_t swState, test;

uint8_t addr = LED_1_24_ADDRESS;
uint8_t crtlReg = 2;
uint8_t ledCmd = 0; //0x55;
	
	

int main(void)
{



	/* Configure GCLK and clock sources according to conf_clocks.h */
	system_clock_init();
	fp_board_init();





// check restart cause
	enum system_reset_cause reset_cause = system_get_reset_cause();
	if (reset_cause == SYSTEM_RESET_CAUSE_WDT) {
//		port_pin_set_output_level(LED_0_PIN, LED_0_INACTIVE);
	}
	else {
//		port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
	}



	//	start 1ms timer
	configure_tc7();
	configure_tc7_callbacks();
	system_interrupt_enable_global();
	

	// reset LED drivers
	port_pin_set_output_level(LED_CHIP_RST, 1);			// reset is active high at processor
	ledTimer = 0;
	while(get_ledTimer());
	port_pin_set_output_level(LED_CHIP_RST, 0);	
		




//	clock_dfll();

//	configure_wdt();

	// setup MAX_TC_AVAIL timers
//	configure_vel_counters();



/*
	// setup spi										 **********************************
	configure_spi_slave();
	configure_spi_slave_callbacks();*/
	
	system_interrupt_enable_global();
/*
	
		LED_I2C_Start();			 // write 0 to mode reg
		LED_I2C_Write(addr);
		LED_I2C_Write(0);
		LED_I2C_Write(0);
		LED_I2C_Stop();*/
	

/*
		// write them back
		LED_I2C_Start();
		LED_I2C_Write(0x2);		// addr
		LED_I2C_Write(0x80);	// auto inc		
		for (i=0; i<0x3f; i++)
		{
			LED_I2C_Write(regs[i]);
		}
		LED_I2C_Stop();
*/

	LEDs_Init();						// init all 3 ICs
					
	while (1)
	{

		Do_LEDs();							// writes 1 byte per call



		uint8_t cnt;
		
		switch(ledTimer)
		{
//			case 64:
//			case 128:
			case 192:
//			case 255:
/*
				LED_Vals[cnt]=LED_Vals[cnt+24]=LED_Vals[cnt+48]=0;
				cnt++;
				if(cnt>23)cnt=0;
				LED_Vals[cnt]=LED_Vals[cnt+24]=LED_Vals[cnt+48]=99;				
				ledTimer++;
*/
				LED_Vals[cnt]=0;
//				LED_Vals[cnt+24]=0;
//				LED_Vals[cnt+48]=0;
				cnt++;
				if(cnt>62)
					cnt=0;
				LED_Vals[cnt]=99;
// 				LED_Vals[cnt+24]=99;
//				LED_Vals[cnt+48]=99;
				ledTimer++;

				break;			
			default:
				break;	
		}




/*
		// write them back
		LED_I2C_Start();
		LED_I2C_Write(0x2);		// addr
		LED_I2C_Write(0x80);	// auto inc		
		for (i=0; i<0x3f; i++)
		{
			LED_I2C_Write(regs[i]);
		}
		LED_I2C_Stop();
		*/


		
/*
				// PWMs
		LED_I2C_Start();
		LED_I2C_Write(0x2);		// addr
		LED_I2C_Write(0x8a);	// control
		for (i=PWM_REGISTER_START; i< PWM_REGISTER_START+24; i++)
		{
			LED_I2C_Write(regs[i]);
		}
		LED_I2C_Stop();
		
		
		

	}*/
	
	/*

		LED_I2C_Start();
		LED_I2C_Write(2);
		LED_I2C_Write(reg);
		LED_I2C_Write(ledCmd);
		LED_I2C_Stop();
*/

}
		
	while (1)						// Loop forever
	{
//		port_pin_toggle_output_level(TP41_PIN); // test point loop indicator

/*
		if (transfer_complete_spi_slave)
		{
			transfer_complete_spi_slave= false;
			if (FPswQ.length < FP_QUE_SIZE-1)  
			{	 swCnt++;
				 put_FP_swQ(swCnt);
			}
			
		}
			
			
			*/


				
//			wdt_reset_count();							 // looking good, prevent WDT from reseting uP





		readRow(swState);
		swState++;
		if (swState>8)swState=0;		
		switch(swState)
		{
			case 0:
				port_pin_set_output_level(FP_SWO8, 0);		// off
				port_pin_set_output_level(FP_SWO0, 1);		// on
				break;
			case 1:
				port_pin_set_output_level(FP_SWO0, 0);
				port_pin_set_output_level(FP_SWO1, 1);
				break;
			case 2:
				port_pin_set_output_level(FP_SWO1, 0);
				port_pin_set_output_level(FP_SWO2, 1);
				break;
			case 3:
				port_pin_set_output_level(FP_SWO2, 0);
				port_pin_set_output_level(FP_SWO3, 1);
				break;
			case 4:
				port_pin_set_output_level(FP_SWO3, 0);
				port_pin_set_output_level(FP_SWO4, 1);
				break;
			case 5:
				port_pin_set_output_level(FP_SWO4, 0);
				port_pin_set_output_level(FP_SWO5, 1);
				break;
			case 6:
				port_pin_set_output_level(FP_SWO5, 0);
				port_pin_set_output_level(FP_SWO6, 1);
				break;
			case 7:
				port_pin_set_output_level(FP_SWO6, 0);
				port_pin_set_output_level(FP_SWO7, 1);
				break;
			case 8:
				port_pin_set_output_level(FP_SWO7, 0);
				port_pin_set_output_level(FP_SWO8, 1);
				break;
				
			//default:	
		
		}


		if (port_pin_get_input_level(FP_SWI0) == 0)
		{
			test++;
		}
	}
}

uint8_t swDebounce[81];

static inline inc_switch(uint8_t FPswitch)
{
	switch (swDebounce[FPswitch])
	{	case 10:
		put_FP_swQ(FPswitch+1);			// turn on
		swDebounce[FPswitch]++;
		break;
		case 11:
		break;
		default:
		swDebounce[FPswitch]++;
	}
}

static inline dec_switch(uint8_t FPswitch)
{
	switch (swDebounce[FPswitch])
	{	case 1:
		put_FP_swQ(FP_SW_OPEN_FLAG + FPswitch+1);		// turn off
		swDebounce[FPswitch]--;
		break;
		case 0:
		break;
		default:
		swDebounce[FPswitch]--;
	}
}


void readRow(uint8_t swState)
{
uint8_t loc = swState*8;

			if (port_pin_get_input_level(FP_SWI0) == 0)
				inc_switch(swState*8);
			else
				dec_switch(swState*8);
							
			if (port_pin_get_input_level(FP_SWI1) == 0)
				inc_switch(swState*8+1);
			else
				dec_switch(swState*8+1);
				
			if (port_pin_get_input_level(FP_SWI2) == 0)
				inc_switch(swState*8+2);
			else
				dec_switch(swState*8+2);
				
			if (port_pin_get_input_level(FP_SWI3) == 0)
				inc_switch(swState*8+3);
			else
				dec_switch(swState*8+3);
				
			if (port_pin_get_input_level(FP_SWI4) == 0)
				inc_switch(swState*8+4);
			else
				dec_switch(swState*8+4);
				
			if (port_pin_get_input_level(FP_SWI5) == 0)
				inc_switch(swState*8+5);
			else
				dec_switch(swState*8+5);
				
			if (port_pin_get_input_level(FP_SWI6) == 0)
				inc_switch(swState*8+6);
			else
				dec_switch(swState*8+6);
				
			if (port_pin_get_input_level(FP_SWI7) == 0)
				inc_switch(swState*8+7);
			else
				dec_switch(swState*8+7);
				
			if (swState==8)
			{
				if (port_pin_get_input_level(FP_SWI8) == 0)
					inc_switch(72);
				else
					dec_switch(72);
			}			
}