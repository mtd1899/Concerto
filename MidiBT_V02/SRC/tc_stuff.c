#include <tc.h>
#define CONF_TC_MODULE_TC3 TC3


#include <asf.h>
//#include <tc_interrupt.h>
#include <tc.h>
#include "tc_stuff.h"
#include "MidiBT_pcb.h"




//! [module_inst]
struct tc_module tc_instance, tc_instance_led;
uint8_t ovFlow, led_ovFlow;


//! [module_inst]

void tc_callback( struct tc_module *const module_inst)
{
//	port_pin_toggle_output_level(Yellow_LED_PIN);
	tc_set_count_value(&tc_instance,0);
	ovFlow = true;
}


void configure_tc3(void)			// configured to run continuously
{
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	config_tc.counter_size = TC_COUNTER_SIZE_16BIT;			//TC_COUNTER_SIZE_8BIT;
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_forApp;    //  TC_CLOCK_PRESCALER_DIV1024;
	//config_tc.counter_16_bit.			//.period = 500;
	config_tc.counter_16_bit.compare_capture_channel[0] = 100;
//	config_tc.counter_16_bit.compare_capture_channel[1] = 100;
/*	config_tc.counter_8_bit.period = 100;
	config_tc.counter_8_bit.compare_capture_channel[0] = 50;
	config_tc.counter_8_bit.compare_capture_channel[1] = 54;
*/	
	tc_init(&tc_instance, CONF_TC_MODULE_TC3, &config_tc);
	tc_enable(&tc_instance);

//	tc_init(&tc_instance_led, CONF_TC_MODULE_TC3, &config_tc);
//	tc_enable(&tc_instance_led);


}
void configure_tc3_callbacks(void)
{
//	tc_register_callback(&tc_instance, tc_callback, TC_CALLBACK_OVERFLOW);
	tc_register_callback(&tc_instance, tc_callback, TC_CALLBACK_CC_CHANNEL0);
//	tc_register_callback(&tc_instance_led, tc_callback_to_toggle_green, TC_CALLBACK_CC_CHANNEL1);
//	tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
//	tc_enable_callback(&tc_instance_led, TC_CALLBACK_CC_CHANNEL1);
}

