/*

// tc_treble.c

TIMER COUNTER
  
  
   */


#include <asf.h>
#include "MidiBT_pcb.h"

void configure_tc4(void);
void configure_tc4_callbacks(void);
void tc_callback_to_toggle_led(struct tc_module *const module_inst);
void check_usarts(void);

uint32_t msCounter,ledTimer; //, usartTimeout;
extern bool tc_callback_occurred;

#define CONF_TC_MODULE TC4


void wait_sec(uint32_t wait)
{
	msCounter = wait*100;
	while (msCounter) check_usarts();
}

void wait_10msSec(uint32_t wait)
{
	msCounter = wait;
	while (msCounter) check_usarts();
}


//! [module_inst]
struct tc_module tc_instance_tc4;
//! [module_inst]

//! [callback_funcs]
extern char pair_addr[20];


void tc_callback_to_toggle_led(
		struct tc_module *const module_inst)
{
//	tc_callback_occurred = true;
//	port_pin_toggle_output_level(Yellow_LED_PIN);
	if (msCounter>0) msCounter--;
	//if(usartTimeout<SERIAL_COMM_QUITE) usartTimeout++;
	ledTimer++;

}
//! [callback_funcs]

//! [setup]
void configure_tc4(void)
{
	//! [setup_config]
	struct tc_config config_tc;
	//! [setup_config]
	//! [setup_config_defaults]
	tc_get_config_defaults(&config_tc);
	//! [setup_config_defaults]

	//! [setup_change_config]
	config_tc.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc.clock_source = GCLK_GENERATOR_1;			// 12M
	
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1024;
	config_tc.counter_8_bit.period = 138;	  	// calculated from 12M and measured at 10ms with scope

	//! [setup_set_config]
	tc_init(&tc_instance_tc4, CONF_TC_MODULE, &config_tc);
	//! [setup_set_config]

	//! [setup_enable]
	tc_enable(&tc_instance_tc4);
	//! [setup_enable]
}

void configure_tc4_callbacks(void)
{
	//! [setup_register_callback]
	tc_register_callback(&tc_instance_tc4, tc_callback_to_toggle_led,TC_CALLBACK_OVERFLOW);

	//! [setup_enable_callback]
	tc_enable_callback(&tc_instance_tc4, TC_CALLBACK_OVERFLOW);

}
//! [setup]