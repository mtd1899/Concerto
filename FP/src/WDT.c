
#include <asf.h>

void configure_wdt(void);

//! [setup]
void configure_wdt(void)
{
	/* Create a new configuration structure for the Watchdog settings and fill
	 * with the default module settings. */
	//! [setup_1]
	struct wdt_conf config_wdt;
	//! [setup_1]
	//! [setup_2]
	wdt_get_config_defaults(&config_wdt);
	//! [setup_2]

	/* Set the Watchdog configuration settings */
	//! [setup_3]
	config_wdt.always_on      = false;
#if !((SAML21) || (SAMC21) || (SAML22))
	config_wdt.clock_source   = GCLK_GENERATOR_4;
#endif
	config_wdt.timeout_period = WDT_PERIOD_2048CLK;
	//! [setup_3]

	/* Initialize and enable the Watchdog with the user settings */
	//! [setup_4]
	wdt_set_config(&config_wdt);
	//! [setup_4]
}
