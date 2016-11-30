/*
	do_SSP_mode_LEDS.c	

*/
#include <asf.h>
#include <port.h>
#include "master.h"
#include "prototypes.h"
#include "..\..\FP\src\fp.h"

bool MIDI_in_SSP_mode = false; 
bool Android_in_SSP_mode = false;

extern struct FPtxQ_def FPledQ;

uint32_t volatile ck_SSP_modeCnt1;

void ck_SSP_mode(void)
{
		// check that BT is in SSP mode
	MIDI_in_SSP_mode = port_pin_get_input_level(WT41_MIDI_CARRIER_DETECT_PIN);
	Android_in_SSP_mode = 	port_pin_get_input_level(WT41_TABLET_CARRIER_DETECT_PIN);
	
	
//	return;			// LEDs are causing a bug somewhere - return for now




		// carrier detect LEDs logic
		if (FPledQ.length == 0)				// only send when nothing else to send
		{	uint8_t val=0;

ck_SSP_modeCnt1++;

			if (MIDI_in_SSP_mode)	val=2;
			if (Android_in_SSP_mode) val++;
			switch (val) 
			{	case 0:
				case 1:
					put_FP_ledQ(62);		// red - no midi carrier
					put_FP_ledQ(63 | FP_LED_OFF_FLAG);
					break;
				case 2:
					put_FP_ledQ(63);		// only midi carrier - orange
					put_FP_ledQ(62);
					break;
				case 3:							
					put_FP_ledQ(63);		// green - both carriers	// turn on 63
					put_FP_ledQ(62 | FP_LED_OFF_FLAG);	// turn off 62
					break;
			}
		}


}

