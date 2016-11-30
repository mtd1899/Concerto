/*
 * TREBLE MAIN
 
 
 see "therory_of_operation_treble.h"
 
*/
#include <asf.h>
//#include "C:\Users\Mike\Documents\'WORKING\SPI_QUICK_START_SLAVE1\ConcertoConf.h"
#include "adc.h"
#include "therory_of_operation_treble.h"
#include "treble.h"
#include <port.h>

//! [buffer]
uint8_t tx_note[2];
uint8_t buffer_rx[2];

volatile bool transfer_complete_spi_slave = false;
struct spi_module spi_slave_instance;
struct adc_module adc_instance;
struct adc_config config_adc;
volatile bool adc_read_done = false;



extern struct tc_module tc7_instance, tc_vel_instance[MAX_TC_AVAIL+1];

#define ADC_SAMPLES 10				// reading 20 channels in 2 sets of 10



uint16_t adc_result_buffer[20];

struct treble_key_status trebKeys[20];

void configure_tc7(void);
void configure_tc7_callbacks(void);
void configure_spi_slave_callbacks(void);
void configure_spi_slave(void);
void configure_adc(void);
void configure_adc_callbacks(void);
void configure_vel_counters(void);
void key_closer(uint8_t, uint8_t);
void key_velocity(uint8_t, uint8_t);
void configure_wdt(void);
//void wdt_reset_count(void);

void spi_transceive_one(struct spi_module *const module,uint8_t *tx_data,uint8_t *rx_data,uint16_t length);
void clock_dfll(void);
void configure_dfll_open_loop(void);
	
	uint8_t velocityCmdRx = 0xf7;			// operates in either the velocity mode or switch closure mode

//  debug vars
 	uint32_t sysHz;
	uint32_t vel_val[8];
	uint8_t testCnt, send =0;				// operates in either the velocity mode or switch closure mode
	uint8_t noteOffset;


enum status_code statusCode;


int main(void)
{

	/* Configure GCLK and clock sources according to conf_clocks.h */
	system_clock_init();

	/* Initialize board hardware */
		struct port_config pin_conf;
		port_get_config_defaults(&pin_conf);

		/* Configure LEDs as outputs, turn them off */
		pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
		port_pin_set_config(LED_PIN, &pin_conf);
		port_pin_set_output_level(LED_PIN, 1);
		port_pin_set_config(TP41_PIN, &pin_conf);
		port_pin_set_config(TP42_PIN, &pin_conf);
		port_pin_set_config(TP43_PIN, &pin_conf);

		/* Set inputs */
		pin_conf.direction  = PORT_PIN_DIR_INPUT;
		pin_conf.input_pull = PORT_PIN_PULL_UP;
		port_pin_set_config(PROCESSOR_LOC0_PIN, &pin_conf);
		port_pin_set_config(PROCESSOR_LOC1_PIN, &pin_conf);
		port_pin_set_config(PROCESSOR_LOC2_PIN, &pin_conf);

	// determine location and set note offset			 port_pin_get_input_level(BUTTON_0_PIN) == false
	if(port_pin_get_input_level(PROCESSOR_LOC0_PIN) == 0) noteOffset = 21;
	else if(port_pin_get_input_level(PROCESSOR_LOC1_PIN) == 0) noteOffset = 41;
		else noteOffset = 61;

// check restart cause
	enum system_reset_cause reset_cause = system_get_reset_cause();
	if (reset_cause == SYSTEM_RESET_CAUSE_WDT) {
		port_pin_set_output_level(LED_0_PIN, LED_0_INACTIVE);
	}
	else {
		port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
	}

		

	//	configure_vel_counter();
	configure_tc7();
	configure_tc7_callbacks();

	system_interrupt_enable_global();

	clock_dfll();

	configure_wdt();

	// setup MAX_TC_AVAIL timers
	configure_vel_counters();

	// setup spi
	configure_spi_slave();
	configure_spi_slave_callbacks();
	
	// setup adc  (analog to digital converter
	configure_adc();
	configure_adc_callbacks();
 
	sysHz = system_cpu_clock_get_hz();
	sysHz = system_apb_clock_get_hz(GCLK_GENERATOR_1);
	
	system_interrupt_enable_global();
	

	// init trebKeys
	for (uint8_t i=0; i<20; i++)
	{
			trebKeys[i].on = false;
			trebKeys[i].lastAtoDval = 0;
			trebKeys[i].maxAtoDval = 100;
			trebKeys[i].minAtoDval = 100;
			trebKeys[i].velCalculating = false;
			trebKeys[i].state = KEY_OFF;
//			trebKeys[i].analogHigh = 100;
//			trebKeys[i].analogLow = 50;
			trebKeys[i].thresholdHigh = 60;	 // PULL FROM FLASH MEMORY SAVED AT CALIBRATION
			trebKeys[i].thresholdLow = 90;
			trebKeys[i].count = 0;
			trebKeys[i].tcPtr = NULL;
			trebKeys[i].sent = false;
			trebKeys[i].calcVelocity = 0;
	}
	
		
	// start scan 2nd group of Keys 
			// note   	config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN10;		  // start at AD10 in configure_adc()
			statusCode = adc_set_pin_scan_mode( &adc_instance, 10, 0);		// 2nd set of 10 channels
			adc_read_buffer_job(&adc_instance, adc_result_buffer+ADC_SAMPLES, ADC_SAMPLES);
		
	// AtoD & and note calcs are done in forground 
		//	(SPI work is in background driven by tc7
	
	
	
	tx_note[0] = 0x80;
	
	port_pin_set_output_level(TP41_PIN,1);

 	spi_transceive_one(&spi_slave_instance, tx_note,buffer_rx,1);

	buffer_rx[0] = 0xf7;

	 
	// spi_transceive_buffer_job(&spi_slave_instance, tx_note,buffer_rx,1);
//uint32_t txCnt=0;
bool doADCcnt = 0;


	// run ADCs one time before calcs start
  	adc_set_positive_input( &adc_instance,ADC_POSITIVE_INPUT_PIN0);
	statusCode = adc_set_pin_scan_mode( &adc_instance, ADC_SAMPLES, 0);	  // 1st set of 10 channels
	statusCode = adc_read_buffer_job(&adc_instance, adc_result_buffer, ADC_SAMPLES);
				
	adc_set_positive_input( &adc_instance,ADC_POSITIVE_INPUT_PIN0);
	statusCode = adc_set_pin_scan_mode( &adc_instance, ADC_SAMPLES, 0);	  // 1st set of 10 channels
	statusCode = adc_read_buffer_job(&adc_instance, adc_result_buffer, ADC_SAMPLES);


	while (1)						// Loop forever
	{
//		port_pin_toggle_output_level(TP41_PIN); // test point loop indicator

		if (transfer_complete_spi_slave)
		{
			transfer_complete_spi_slave= false;
				
			wdt_reset_count();							 // looking good, prevent WDT from reseting uP
		}
		
		if (adc_read_done == true)
		{
			adc_read_done=false;
			if (doADCcnt)
			{
				// start scan 1st group Keys - 20 keys are scanned in 2 sets of 10
				adc_set_positive_input( &adc_instance,ADC_POSITIVE_INPUT_PIN0);
				statusCode = adc_set_pin_scan_mode( &adc_instance, ADC_SAMPLES, 0);	  // 1st set of 10 channels
				statusCode = adc_read_buffer_job(&adc_instance, adc_result_buffer, ADC_SAMPLES);
				// process 2nd set of key reads
				if (velocityCmdRx != VELOCITY_MODE)
					key_closer(10, 20);	// operating in switch closure mode
				else key_velocity(10,20); // velocity mode
			}
			else
			{
				// start scan 2nd group of Keys - 20 keys are scanned in 2 sets of 10
				adc_set_positive_input( &adc_instance,ADC_POSITIVE_INPUT_PIN10);
				statusCode = adc_set_pin_scan_mode( &adc_instance, 10, 0);		// 2nd set of 10 channels
				statusCode = adc_read_buffer_job(&adc_instance, adc_result_buffer+ADC_SAMPLES, ADC_SAMPLES);
				// process 1st set of key reads
				if (velocityCmdRx != VELOCITY_MODE)	 
					key_closer(0, 10);	// operating in switch closure mode
				else key_velocity(0,10); // velocity mode
			}
			doADCcnt = !doADCcnt;
		}
	}
}


void configure_dfll_open_loop(void)
{
	//! [config_dfll_config]
	struct system_clock_source_dfll_config config_dfll;
	//! [config_dfll_config]
	//! [config_dfll_get_defaults]
	system_clock_source_dfll_get_config_defaults(&config_dfll);
	//! [config_dfll_get_defaults]

	//! [config_dfll_set_config]
	system_clock_source_dfll_set_config(&config_dfll);
	//! [config_dfll_set_config]
}
	
	
void clock_dfll(void)		 {
		/* Configure the DFLL in open loop mode using default values */
		//! [config_dfll_main]
		configure_dfll_open_loop();
		//! [config_dfll_main]

		/* Enable the DFLL oscillator */
		//! [enable_dfll_main]
		enum status_code dfll_status =
		system_clock_source_enable(SYSTEM_CLOCK_SOURCE_DFLL);

		if (dfll_status != STATUS_OK) {
			/* Error enabling the clock source */
		}
		//! [enable_dfll_main]

		/* Configure flash wait states before switching to high frequency clock */
		//! [set_sys_wait_states]
		system_flash_set_waitstates(2);
		//! [set_sys_wait_states]

		/* Change system clock to DFLL */
		//! [set_sys_clk_src]
		struct system_gclk_gen_config config_gclock_gen;
		system_gclk_gen_get_config_defaults(&config_gclock_gen);
		config_gclock_gen.source_clock    = SYSTEM_CLOCK_SOURCE_DFLL;
		config_gclock_gen.division_factor = 1;
	
		system_gclk_gen_set_config(GCLK_GENERATOR_0, &config_gclock_gen);
	
}



