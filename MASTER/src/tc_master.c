/*

// tc_treble.c

TIMER COUNTER
  
  
   */


#include <asf.h>
#include "conf_qs_tc_timer.h"
#include "master.h"
#include "prototypes.h"

void tc_callback(struct tc_module *const module_inst);
void configure_tc_callbacks(void);
void configure_tc(void);



uint32_t volatile msCounter;
uint8_t volatile spiFreqTimer;

struct tc_module tc_instance;	//! [module_inst]


void wait_10msSec(uint32_t wait)
{
	msCounter = wait;
	while (msCounter) 
	{	TX_usart_Qs();
		do_FP_SPI();
	}
}



//! [callback_funcs]
void tc_callback(
		struct tc_module *const module_inst)
{
//	tc_callback_occurred = true;
	port_pin_toggle_output_level(RUN_LED);
	
	spiFreqTimer--;
	if(msCounter>0)	msCounter--;
}
//! [callback_funcs]

//! [setup]
void configure_tc(void)
{																				    
	//! [setup_config]
	struct tc_config config_tc;
	//! [setup_config]
	//! [setup_config_defaults]
	tc_get_config_defaults(&config_tc);
	//! [setup_config_defaults]

	//! [setup_change_config]
	config_tc.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc.clock_source = GCLK_GENERATOR_4;	

	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1024;	 // takes 8M down to 7813
	config_tc.counter_8_bit.period = 117;	  	// calculated and measured at 10ms with scope
//	config_tc.counter_8_bit.compare_capture_channel[0] = 50;
	//config_tc.counter_8_bit.compare_capture_channel[1] = 54;
	//! [setup_change_config]
			

	//! [setup_set_config]
	tc_init(&tc_instance, CONF_TC_MODULE, &config_tc);
	//! [setup_set_config]

	//! [setup_enable]
	tc_enable(&tc_instance);
	//! [setup_enable]
}

void configure_tc_callbacks(void)
{
	//! [setup_register_callback]
	tc_register_callback(&tc_instance, tc_callback,TC_CALLBACK_OVERFLOW);

	//! [setup_enable_callback]
	tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);

}
//! [setup]