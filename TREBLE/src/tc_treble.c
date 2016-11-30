/*

// tc_treble.c

TIMER COUNTER
  
  
   */


#include <asf.h>
#include "conf_qs_tc_timer.h"
#include "tc.h"
#include "treble.h"

void configure_tc7(void);
void configure_tc7_callbacks(void);
void tc7_callback(struct tc_module *const module_inst);
void configure_vel_counters(void);	


struct tc_module tc7_instance, tc_vel_instance[MAX_TC_AVAIL+1]; //, *tc7ptr, *tc0ptr, *tc1ptr;
					  

void configure_vel_counters(void)
{
//	uint8_t i, test;
	//! [setup_config]
	struct tc_config config_tc;
	//! [setup_config]
	//! [setup_config_defaults]
	tc_get_config_defaults(&config_tc);
	//! [setup_config_defaults]

	//! [setup_change_config]
	config_tc.counter_size = TC_COUNTER_SIZE_16BIT;
	config_tc.clock_source = GCLK_GENERATOR_1;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;
//	config_tc.counter_16_bit.period = 0;							   // running at 32500 cnts / sec -- 31 us per cnt
	config_tc.counter_16_bit.compare_capture_channel[0] = 50;
	config_tc.counter_16_bit.compare_capture_channel[1] = 54;
	//! [setup_change_config]

	//! [setup_set_config]
	tc_init(tc_vel_instance, TC0, &config_tc);
	tc_init(tc_vel_instance+1, TC1, &config_tc);
	tc_init(tc_vel_instance+2, TC2, &config_tc);
	tc_init(tc_vel_instance+3, TC3, &config_tc);
	tc_init(tc_vel_instance+4, TC4, &config_tc);
	tc_init(tc_vel_instance+5, TC5, &config_tc);
	tc_init(tc_vel_instance+6, TC6, &config_tc);
	if (MAX_TC_AVAIL == 8) tc_init(tc_vel_instance+7, TC7, &config_tc);
								/*
tc7ptr = &tc7_instance;
tc0ptr = tc_vel_instance;
tc1ptr = tc_vel_instance+1;

tc1ptr = tc0ptr - tc7ptr;	  */


	tc_enable(tc_vel_instance);
	tc_enable(tc_vel_instance+1);
	tc_enable(tc_vel_instance+2);
	tc_enable(tc_vel_instance+3);
	tc_enable(tc_vel_instance+4);
	tc_enable(tc_vel_instance+5);
	tc_enable(tc_vel_instance+6);
	if (MAX_TC_AVAIL == 8) tc_enable(tc_vel_instance+7);
	
	
}

#if MAX_TC_AVAIL !=8			

		void configure_tc7_callbacks(void)
		{
			//! [setup_register_callback]
			tc_register_callback(&tc7_instance, tc7_callback,TC_CALLBACK_OVERFLOW);

			//! [setup_enable_callback]
			tc_enable_callback(&tc7_instance, TC_CALLBACK_OVERFLOW);
		}


		//! [callback_funcs]
		uint32_t cnt=0, sum=0, avg=0;
		extern uint32_t vel_val[8];
			
		void tc7_callback(
				struct tc_module *const module_inst)
		{
			port_pin_toggle_output_level(TP43_PIN);					// running output for debugging



/*
		while (!spi_is_ready_to_write(&spi_slave_instance)) {}

		while(spi_transceive_wait(&spi_slave_instance, send, buffer_rx) != STATUS_OK) {
			// Wait for transfer from the master 
		}
		
		
		
		

		//	tc_set_count_value (tc_vel_instance,0);
			cnt++;
		//	sum +=    vel_val  ; 
			//avg = sum/cnt;
			if (vel_val[0] > 15000)
			{
				vel_val[0] =0;
				tc_stop_counter(tc_vel_instance);
				sum = cnt;
			} 
			if (cnt > 10)
			{			cnt=0;
				vel_val[0] = 0;
				tc_start_counter (tc_vel_instance);
			}*/
	
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
				

#endif

//! [setup]