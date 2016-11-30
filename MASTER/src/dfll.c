#include <asf.h>
void configure_dfll_open_loop(void);
void clock_dfll(void);

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
