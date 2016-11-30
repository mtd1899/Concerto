/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <adc.h>
#include <asf.h>
#include <compiler.h>
#include <board.h>
#include <conf_board.h>
#include <port.h>

void configure_adc(void);
void configure_port_pins(void);
void configure_adc_callbacks(void);
void adc_complete_callback(struct adc_module *const module);

extern struct adc_module adc_instance;
//extern struct adc_config config_adc;
extern volatile bool adc_read_done;

void adc_complete_callback(
struct adc_module *const module)
{
	adc_read_done = true;
}

struct adc_config config_adc;

void configure_adc(void)
{
	adc_get_config_defaults(&config_adc);
	//! [setup_config_defaults]
	config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV8;
	config_adc.reference       = ADC_REFERENCE_INTVCC1;
	config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN10;		  // start at AD0
	config_adc.resolution      = ADC_RESOLUTION_8BIT;
	adc_init(&adc_instance, ADC, &config_adc);
	adc_enable(&adc_instance);
}

void configure_adc_callbacks(void)
{
	//! [setup_register_callback]
	adc_register_callback(&adc_instance, adc_complete_callback, ADC_CALLBACK_READ_BUFFER);
	//! [setup_register_callback]
	//! [setup_enable_callback]
	adc_enable_callback(&adc_instance, ADC_CALLBACK_READ_BUFFER);
	//! [setup_enable_callback]
}
//! [setup]
