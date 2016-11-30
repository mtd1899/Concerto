/*
	music.c
	
	1. process the key values in the keyQ
	2. creates a midi command/music stream
	2. loads the midi stream into midiQ  (note midi sercomm3 connects to a BT module or wired

*/

#include <asf.h>
#include <stdio.h>
#include <port.h>

#include "master.h"
#include "prototypes.h"
#include "..\..\FP\src\fp.h"

extern struct usart_module MIDI_usart_instance, ANDROID_usart_instance;
extern struct trebleRxQ_def trebleRxQ1, trebleRxQ2, trebleRxQ3;
extern struct usartQdef midiQ, androidQ;
extern struct keyQ_def keyQ;
extern bool MIDI_usart_rx_done;

uint8_t lastSW;
uint8_t volatile rx_buffer_MIDI[3];
	
void do_music_mode(void)
{	uint8_t nextSW;

	// handle midi in from MRU  -- USART DATA
	if (MIDI_usart_rx_done)
	{	
		usart_read_buffer_job(&MIDI_usart_instance,(uint8_t *)rx_buffer_MIDI, 1);
		put_rx_Q(&midiQ, rx_buffer_MIDI, 1);		// put in Q for processing
		
		MIDI_usart_rx_done = false;
	}
	else
	{	uint8_t code = usart_get_job_status(&MIDI_usart_instance,USART_TRANSCEIVER_RX);
		if (code != STATUS_BUSY) 
		{ 	usart_read_buffer_job(&MIDI_usart_instance,(uint8_t *)rx_buffer_MIDI, 1);
			MIDI_usart_rx_done = false;
		}
	}


	// handle the FP switch inputs
		
		get_FP_swQ(&nextSW);
//TEST***************************************************************
		if (nextSW != FP_SW_NO_CHANGE)
		{
				if (nextSW>0)
				{	if (nextSW<80)
					{	// switch pushed
						put_FP_ledQ (lastSW| FP_LED_OFF_FLAG);		 // turn off last push
						lastSW = nextSW;
						put_FP_ledQ(nextSW);						// turn on this one
					} 											
					else
					{	// switch released - ignore
	
					}
				}
		}
//TEST***************************************************************


	
	// handle Android commands - could add Android input into FP switch Q
	
	
}							 

static inline uint8_t midi_tx_Q_length (void)
{
	return (midiQ.tx_in - midiQ.tx_out);
}

	
void do_music_notes(void)
{	uint8_t	tx_buffer[2], midiStr[5];

	if (((midiQ.tx_in - midiQ.tx_out)+3 < MIDI_Q_SIZE) && (keyQ.length>=1)) 		   // MUST CHECK THAT IT WILL FIT BEFORE CALLING put_tx_Q
	{
		key_get_keyQ(tx_buffer);			// get key to process
		if (tx_buffer[1]>1) 					// process key	   (treble will send a velocity of 1 to turn off note)
 		{
			midiStr[0] = 0x90;
			midiStr[1] = tx_buffer[0];
			midiStr[2] = tx_buffer[1];
		}
		else
		{
			midiStr[0] = 0x80;
			midiStr[1] = tx_buffer[0];
			midiStr[2] = 0;						// velocity of zero
		}
		put_tx_Q(&midiQ, midiStr, 3);
		
		#ifdef DEBUG_MIDI_TO_TABLET
				char myStr[15],myStrChar[10];
				sprintf(myStr, "%02x",midiStr[0]);
				sprintf(myStrChar, "%02x",midiStr[1]);
				strcat(myStr,",");
				strcat(myStr,myStrChar);
				strcat(myStr,",");
				sprintf(myStrChar, "%02x",midiStr[2]);
				strcat(myStr,myStrChar);
				strcat(myStr,"\r\n");
				put_tx_Q(&androidQ,(uint8_t *) myStr,strlen(myStr));
		#endif


 	}
}
