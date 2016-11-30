/*

// tc_treble.c

TIMER COUNTER
  
  
   */


#include <asf.h>
#include "conf_qs_tc_timer.h"
#include "tc.h"
#include "FP.h"

void configure_tc7(void);
void configure_tc7_callbacks(void);
void tc7_callback(struct tc_module *const module_inst);
uint8_t get_ledTimer(void);

struct tc_module tc7_instance;

uint8_t timer100ms, ledTimer; 


uint8_t get_ledTimer(void)
{	return ledTimer;
}
	

void configure_tc7_callbacks(void)
{
	//! [setup_register_callback]
	tc_register_callback(&tc7_instance, tc7_callback,TC_CALLBACK_OVERFLOW);

	//! [setup_enable_callback]
	tc_enable_callback(&tc7_instance, TC_CALLBACK_OVERFLOW);
}



void tc7_callback(
		struct tc_module *const module_inst)
{

	ledTimer++;
	
	if (ledTimer == 0)	port_pin_set_output_level(LED_RUN_PIN, 0);
	if (ledTimer == 40) port_pin_set_output_level(LED_RUN_PIN, 1);


	
	if (ledTimer == 100)						// do 100ms stuff
	{
		if (timer100ms)  timer100ms--;
		
	}

//	port_pin_toggle_output_level(FP_SWO0);		
		
}								 
//! [callback_funcs]
									
//! [setup]
void configure_tc7(void)
{
	//! [setup_config]
	struct tc_config config_tc;
	//! [setup_config]
	//! [setup_config_defaults]
	tc_get_config_defaults(&config_tc);
	//! [setup_config_defaults]

	//! [setup_change_config]
		config_tc.counter_size = TC_COUNTER_SIZE_8BIT;
		config_tc.clock_source = GCLK_GENERATOR_1;						// running at 8Mhz
		config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV64;
		config_tc.counter_8_bit.period = 124;						// measured 1khz with scope
	//! [setup_change_config]

	//! [setup_set_config]
	tc_init(&tc7_instance, TC7, &config_tc);
	//! [setup_set_config]

	//! [setup_enable]
	tc_enable(&tc7_instance);
	//! [setup_enable]
}
				

