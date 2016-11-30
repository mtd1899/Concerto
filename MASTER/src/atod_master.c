/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <adc.h>
#include <asf.h>

extern struct adc_module adc_instance;
//extern struct adc_config config_adc;

//! [module_inst]
#include <compiler.h>
#include <board.h>
#include <conf_board.h>
#include <port.h>

void configure_adc(void);
void configure_port_pins(void);
void configure_adc_callbacks(void);
void adc_complete_callback(struct adc_module *const module);

//! [module_inst]


//! [job_complete_callback]
extern volatile bool adc_read_done;

void adc_complete_callback(
struct adc_module *const module)
{
	adc_read_done = true;
}


void configure_adc(void)
{
	//! [setup_config]
	struct adc_config config_adc;
	//! [setup_config]
	//! [setup_config_defaults]
	adc_get_config_defaults(&config_adc);
	//! [setup_config_defaults]
	config_adc.clock_source                  = GCLK_GENERATOR_1;	// 
	//! [setup_modify_conf]
	#if (!SAML21) && (!SAML22) && (!SAMC21)
	config_adc.gain_factor     = ADC_GAIN_FACTOR_DIV2;
	#endif
	config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV8;
	config_adc.reference       = ADC_REFERENCE_INTVCC1;
	#if (SAMC21)
	config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN5;
	#else
	config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN0;		  // start at AD0
	#endif
	config_adc.resolution      = ADC_RESOLUTION_8BIT;
	//! [setup_modify_conf]

	//! [setup_set_config]
	#if (SAMC21)
	adc_init(&adc_instance, ADC1, &config_adc);
	#else
	adc_init(&adc_instance, ADC, &config_adc);
	#endif
	//! [setup_set_config]

	//! [setup_enable]
	adc_enable(&adc_instance);
	//! [setup_enable]
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
