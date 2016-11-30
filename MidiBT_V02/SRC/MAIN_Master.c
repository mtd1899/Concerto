/**
 
 MAIN.C            FOR MASTER
 
 + */

/*
potential mods

RING 0 00:07:80:a1:7c:f7 1 RFCOMM							   for slave

NO CARRIER 0 ERROR c0c RFC_L2CAP_LINK_LOSS						for both


*/



/* OVERVIEW
	automatically finds and connects to a unpaired WT41
	
	after being paired will only connect to that WT41 using it's ID
	
	pushing SW0 button on the xplained PCB will remove the pairing
	
	translates a 31200 serial Midi stream to 115200 logic stream
	
	using 2 ring Qs (256 byte)  midiToBlueTooth & midiToBlueTooth
	
	SERCOMM4 
		- communicates with the MIDI channel at 31200
		- recieves and loads the midiToBlueTooth serialQ
		- unloads the blueToothToMidi serialQ and transmitts
	SERCOMM5
		- communicates with the WT41 Bluetooth channel at 115200 
		- recieves and loads the blueToothToMidi serialQ
		- unloads the midiToBlueTooth serialQ and transmitts

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

void BT_master_init(void);
void init(void);
void reset_BT(void);
void exit_SSP_WT41(void);
void wait_sec(uint32_t wait);
void wait_10msSec(uint32_t wait);
_PTR	 _EXFUN(memmem, (const _PTR, size_t, const _PTR, size_t));
void check_unpairPB(void);


bool BTpaired, BTinSSPmode;
uint8_t tx_data, failed, loopcnt, test, ckStatus;
uint8_t sysState;


	uint32_t clockhz;
	char str1[50], str2[20];
	uint8_t string2[] = "1\r\n";
	uint8_t cnt, cnt2 = 0;

	extern struct bufferDef serialQ;

	uint8_t cmd;
	uint32_t src[8], count, tcCount, send10;
	uint8_t statRet;
	char str[100];
	
uint32_t msCounter,ledTimer; //, usartTimeout;
	
uint32_t val;
extern char cStr[];


void check_unpairPB(void)			// looks to see if unpair button is pushed
{	
	while (port_pin_get_input_level(UNPAIR_PUSH_BUTTON) == false)
	{
		port_pin_set_output_level(Green_LED_PIN,0);
		port_pin_set_output_level(Yellow_LED_PIN,0);
		port_pin_set_output_level(Red_LED_PIN,0);
		ckStatus = unpair_WT41();
		if (ckStatus == STATUS_OK) 	
		{	
			sysState = INIT_STATE;
			break;
		}
	}
}

int main(void)
{	

	init();

	port_pin_set_output_level(Red_LED_PIN,0);
	init_serial_buffers();


	reset_BT();
	BT_master_init();
	
	sysState = INIT_STATE;

	if (port_pin_get_input_level(WT41_CARRIER_DETECT_PIN) != true) check_unpairPB();

	port_pin_set_output_level(Green_LED_PIN,1);	
	port_pin_set_output_level(Yellow_LED_PIN,1);
	port_pin_set_output_level(Red_LED_PIN,0);	

	if (port_pin_get_input_level(WT41_CARRIER_DETECT_PIN) != true) check_unpairPB();

	while(!BTpaired && !(port_pin_get_input_level(WT41_CARRIER_DETECT_PIN) == true))   // if carrier detected the WT41 autocall has taken place;
	{
		switch(sysState)
		{
			case INIT_STATE:							
			// check WT41 responding
				if (check_WT41_responding() != STATUS_OK) break;
				else sysState = 	CK_PAIR_STATE;	
		
			case CK_PAIR_STATE:							// check if paired
				if (check_WT41_paired() == STATUS_OK) 
				{
					sysState = PAIRED_STATE;
				}
				else
				{	sysState = PAIRING_STATE;
					break;
				} 
			
			case PAIRED_STATE:							// paired & waiting for iWrap autocall to take place
				port_pin_set_output_level(Yellow_LED_PIN,0);
				port_pin_set_output_level(Red_LED_PIN,1);
				port_pin_set_output_level(Green_LED_PIN,1);
				BTpaired = true;
				break;
					
			
			case INQUIRY_STATE:							// not paired	 - find a slave to pair with
				port_pin_set_output_level(Yellow_LED_PIN,1);
				port_pin_set_output_level(Red_LED_PIN,0);
				port_pin_set_output_level(Green_LED_PIN,1);
				
				test = inquiry_WT41();

				if (test != STATUS_OK) 	break;
				sysState = PAIRING_STATE;
					
			case PAIRING_STATE:							// pair with slave
				test = pair_WT41();

				if (test == STATUS_OK)
				{
					sysState = PAIRED_STATE;
					load_iWrap_Cmd((char*)WT41_AUTOCALL);		// start calling
					wait_10msSec(5);
					BTpaired = true;
				}
				else
				{
					sysState = INQUIRY_STATE;
				}
				break;
		}
	}

	init_serial_buffers(); //for testing
			
	while (1)
	{
		if (port_pin_get_input_level(WT41_CARRIER_DETECT_PIN) == false)
		{	// NO CARRIER
			BTinSSPmode =  false;
			// flash yellow
			port_pin_set_output_level(Yellow_LED_PIN,0);
			port_pin_set_output_level(Red_LED_PIN,1);
			port_pin_set_output_level(Green_LED_PIN,1);
			wait_10msSec(5);		  // check_usarts(); also called during wait
			port_pin_set_output_level(Yellow_LED_PIN,1);
			wait_10msSec(50);	 		// checks usarts	
			check_unpairPB();							// mod 10/3/16
		}
 		else			 // CARRIER DETECTED							// flash green led
 		{
			BTinSSPmode =  true;
			port_pin_set_output_level(Yellow_LED_PIN,1);
			port_pin_set_output_level(Red_LED_PIN,1);
			port_pin_set_output_level(Green_LED_PIN,0);
			wait_10msSec(5);		  // check_usarts(); also called during wait
			port_pin_set_output_level(Green_LED_PIN,1);
			wait_10msSec(50);	 			
 		}

 	}
}

	

	
