/**
 hw_treble.c


 HARDWARE
 
 
 
 */
#include "FP.h"
#include <compiler.h>
#include <board.h>
#include <conf_board.h>
#include <port.h>

void fp_board_init(void);

#define OFF			1
#define ON			0


void fp_board_init(void)
{


	/* Initialize board hardware */
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

 // outputs
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_RUN_PIN, &pin_conf);
	port_pin_set_output_level(LED_RUN_PIN, 1);
	port_pin_set_config(LED_CHIP_RST, &pin_conf);
	
	port_pin_set_config(FP_SWO0, &pin_conf);
	port_pin_set_config(FP_SWO1, &pin_conf);
	port_pin_set_config(FP_SWO2, &pin_conf);
	port_pin_set_config(FP_SWO3, &pin_conf);	
	port_pin_set_config(FP_SWO4, &pin_conf);
	port_pin_set_config(FP_SWO5, &pin_conf);
	port_pin_set_config(FP_SWO6, &pin_conf);
	port_pin_set_config(FP_SWO7, &pin_conf);
	port_pin_set_config(FP_SWO8, &pin_conf);
	
	port_pin_set_output_level(FP_SWO0, 0);
	port_pin_set_output_level(FP_SWO1, 0);
	port_pin_set_output_level(FP_SWO2, 0);
	port_pin_set_output_level(FP_SWO3, 0);
	port_pin_set_output_level(FP_SWO4, 0);
	port_pin_set_output_level(FP_SWO5, 0);
	port_pin_set_output_level(FP_SWO6, 0);
	port_pin_set_output_level(FP_SWO7, 0);
	port_pin_set_output_level(FP_SWO8, 0);

	/* Set inputs */

	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;
	port_pin_set_config(FP_SWI0, &pin_conf);
	port_pin_set_config(FP_SWI1, &pin_conf);	
	port_pin_set_config(FP_SWI2, &pin_conf);
	port_pin_set_config(FP_SWI3, &pin_conf);	
	port_pin_set_config(FP_SWI4, &pin_conf);
	port_pin_set_config(FP_SWI5, &pin_conf);	
	port_pin_set_config(FP_SWI6, &pin_conf);
	port_pin_set_config(FP_SWI7, &pin_conf);	
	port_pin_set_config(FP_SWI8, &pin_conf);	


}
