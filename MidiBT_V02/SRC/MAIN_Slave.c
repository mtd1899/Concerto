/**
FROM C:
 
 MAIN.C	   - for SLAVE
 
 + */

/* OVERVIEW
	automatically finds and connects to a unpaired WT41
	
	after being paired will only connect to that WT41 using it's ID
	
	pushing SW0 button on the xplained PCB will remove the pairing
	
	translates a 31200 serial Midi stream to 115200 logic stream
	
	using 2 ring Qs (256 byte)  midiToBlueTooth & midiToBlueTooth
	
	SERCOMM4 
		- communicates with the MIDI channel at 31200
		- recieves and loads the midiToBlueTooth buffer
		- unloads the blueToothToMidi buffer and transmitts
	SERCOMM5
		- communicates with the WT41 Bluetooth channel at 115200 
		- recieves and loads the blueToothToMidi buffer
		- unloads the midiToBlueTooth buffer and transmitts

	PA16 Reset_BT
	
*/

#include <asf.h>
#include <conf_clocks.h>
#include <stdio.h>
#include <string.h>
#include "serial_buffers.h"
#include "tc_stuff.h"
#include <tc.h>
#include <usart_interrupt.h>
#include "MidiBT_pcb.h"

void BT_slave_init(void);
void init(void);
void reset_BT(void);
void wait_10msSec(uint32_t);
enum status_code check_called(void);

extern char pair_addr[20];
extern struct tc_module tc_instance, tc_instance_led;
uint8_t tx_data, failed, loopcnt, ckStatus;
uint8_t sysState;
struct bufferDef *buffPtr;
	uint32_t clockhz;
	char str1[50], str2[20];
	uint8_t string2[] = "1\r\n";
	uint8_t cnt, cnt2 = 0;

	extern struct bufferDef buffer;

	uint8_t cmd;
	struct system_clock_source_osc32k_config config_32;
	uint32_t src[8], count, tcCount, send10;
	uint8_t statRet;
	extern uint8_t ovFlow;
	char str[100];
extern char cStr[];	
extern uint32_t msCounter,ledTimer;	
bool slave, in_SSP_mode, BTpaired;

int main(void)
{	
	sysState = INIT_STATE;
	init();

	init_serial_buffers();
	reset_BT();
	
	BT_slave_init();

	slave = true;

	port_pin_set_output_level(Green_LED_PIN,1);				// leds off
	port_pin_set_output_level(Yellow_LED_PIN,1);
	port_pin_set_output_level(Red_LED_PIN,1);	


	while (1)  	  // master has taken control
	{


 		check_usarts();

		while (port_pin_get_input_level(UNPAIR_PUSH_BUTTON) == false)
		{																 
			port_pin_set_output_level(Green_LED_PIN,0);	
			port_pin_set_output_level(Yellow_LED_PIN,0);
			port_pin_set_output_level(Red_LED_PIN,0);
			ckStatus = unpair_WT41();
			if (ckStatus == STATUS_OK) 	break;
		}

		if (port_pin_get_input_level(WT41_CARRIER_DETECT_PIN) == false)
		{	// NO CARRIER
			check_WT41_paired();
			if (BTpaired)												// flash yellow
			{
				port_pin_set_output_level(Yellow_LED_PIN,0);
				port_pin_set_output_level(Red_LED_PIN,1);
				port_pin_set_output_level(Green_LED_PIN,1);
				wait_10msSec(5);		  // check_usarts(); also called during wait
				port_pin_set_output_level(Yellow_LED_PIN,1);
				wait_10msSec(50);	 			
			
			}
			else														// flash red
			{
				port_pin_set_output_level(Yellow_LED_PIN,1);
				port_pin_set_output_level(Red_LED_PIN,0);
				port_pin_set_output_level(Green_LED_PIN,1);
				wait_10msSec(5);		  // check_usarts(); also called during wait
				port_pin_set_output_level(Red_LED_PIN,1);
				wait_10msSec(50);	 			
			}
		}
 		else			 // CARRIER DETECTED							// flash green led
 		{
			port_pin_set_output_level(Yellow_LED_PIN,1);
			port_pin_set_output_level(Red_LED_PIN,1);
			port_pin_set_output_level(Green_LED_PIN,0);
			wait_10msSec(5);		  // check_usarts(); also called during wait
			port_pin_set_output_level(Green_LED_PIN,1);
			wait_10msSec(50);	 			
 		}

 	}
}

	
