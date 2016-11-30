/**
 hw_treble.c


 HARDWARE
 
 
 
 */

#include <compiler.h>
#include <board.h>
#include <conf_board.h>
#include <port.h>

#define LED_PIN	PIN_PA00
#define OFF			1
#define ON			0


void treble_board_init(void)
{

	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	/* Configure LEDs as outputs, turn them off */
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_PIN, &pin_conf);
	port_pin_set_output_level(LED_PIN, OFF);

}
