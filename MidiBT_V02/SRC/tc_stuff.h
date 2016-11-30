#define TC_CLOCK_PRESCALER_forApp  TC_CLOCK_PRESCALER_DIV1024
#define TC_CPS 12000000UL / 1024UL
#define TC_CP10ms TC_CPS / 100UL

void tc_callback( struct tc_module *const module_inst);
void tc_callback_to_toggle( struct tc_module *const module_inst);
void configure_tc3(void);
void configure_tc3_callbacks(void);
