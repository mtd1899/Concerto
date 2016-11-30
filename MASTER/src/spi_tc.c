/*

// tc_SPI.c

TIMER COUNTER
  
  
   */


#include <asf.h>
#include "conf_qs_tc_timer.h"

void configure_tc(void);
void configure_tc_callbacks(void);
void tc_callback_to_toggle_led(struct tc_module *const module_inst);

extern uint8_t timerCnt;
extern bool tc_SPI_callback_occurred;

extern struct spi_module spi_master_to_treble_instance;

//! [module_inst]
struct tc_module tc7_SPI;
extern struct spi_slave_inst treble_slave_instance[3];

//! [module_inst]

//! [callback_funcs]
void tc_SPI_callback(struct tc_module *const module_inst)
{	static bool toggle;

	tc_SPI_callback_occurred = true;

}
//! [callback_funcs]

//! [setup]
void configure_tc_SPI(void)
{
	//! [setup_config]
	struct tc_config config_tc;
	//! [setup_config]
	//! [setup_config_defaults]
	tc_get_config_defaults(&config_tc);
	//! [setup_config_defaults]

	//! [setup_change_config]
	config_tc.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc.clock_source = GCLK_GENERATOR_1;
	//measured at period 100, DIV64 200ms with scope
	//measured at period 10, DIV64 21.5ms with scope
	//measured at period 0, DIV64 2ms with scope
	
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;
	config_tc.counter_8_bit.period = 16;	  	//measured at period 16, DIV1 520us with scope
	config_tc.counter_8_bit.compare_capture_channel[0] = 50;
	config_tc.counter_8_bit.compare_capture_channel[1] = 54;
	//! [setup_change_config]
			

	//! [setup_set_config]
	tc_init(&tc7_SPI, TC7, &config_tc);

	//! [setup_set_config]

	//! [setup_enable]
	tc_enable(&tc7_SPI);
	//! [setup_enable]
}

void configure_tc_SPI_callbacks(void)
{
	//! [setup_register_callback]
	tc_register_callback(&tc7_SPI, tc_SPI_callback,TC_CALLBACK_OVERFLOW);

	//! [setup_enable_callback]
	tc_enable_callback(&tc7_SPI, TC_CALLBACK_OVERFLOW);

}
//! [setup]