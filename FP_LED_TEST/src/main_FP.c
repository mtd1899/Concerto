/*
 * FP(front panel) MAIN
 
 
 see "therory_of_operation_treble.h"
 
*/
#include <asf.h>
#include "therory_of_operation_fp.h"
#include "fp.h"
#include <port.h>



//volatile bool transfer_complete_spi_slave = false;
volatile bool transfer_complete_spi_rx = false;
volatile bool transfer_complete_spi_tx = false;

struct spi_module spi_slave_instance;
uint8_t  tx_buffer[2];
uint8_t  rx_buffer[2];

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

void clock_dfll(void);
void configure_dfll_open_loop(void);

void fp_board_init(void);
uint8_t get_ledTimer(void);

uint8_t LED_I2C_Write(uint8_t c);
uint8_t LED_I2C_Read(uint8_t ack);
void LED_I2C_Stop(void);
void LED_I2C_Start(void);

void LEDs_Init(void);						// init all 3 ICs
void Do_LEDs(void);							// writes 1 byte per call

void Do_Switches(void);						// handle FP switches

void get_FP_swQ(uint8_t *ptr);
void put_FP_ledQ (void);

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
	
uint32_t cntErrors;
		
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
	

	LEDs_Init();						// init all 3 ICs
					
		uint8_t cnt;

	while (1)
	{

		Do_LEDs();							// writes 1 byte per call




		
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
	}
}

