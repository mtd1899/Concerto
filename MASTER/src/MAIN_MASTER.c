/*
	MAIN function for the Master CPU
	
	see:
		-  Concerto 2016.docx for details on overall structure
		- FP.h for FP structure definitions
		- Master.h for all other structure definitions
	
	
	
	
*/
#include <asf.h>
#include <port.h>

#include "master.h"
#include "prototypes.h"
#include "..\..\FP\src\fp.h"

extern struct trebleRxQ_def trebleRxQ1, trebleRxQ2, trebleRxQ3;
extern struct usartQdef midiQ, androidQ;
extern struct FPtxQ_def FPledQ;

bool carrierDetectOnFlag=false;
volatile bool transfer_complete_spi_master_to_treble = false;


		 
 int main(void)
{	
//	while(1){}						//****************************************
		
	init();
												  

 	while (true) 
	{

		// check that BT is in SSP mode
		ck_SSP_mode();
		

		port_pin_toggle_output_level(LOOP_IND); // test point loop indicator

// !!!!!!!!!!!!!!!!!! DO FP SPI, WRITE LED AND READ SWITCHES		
		do_FP_SPI();

//	NEED A SANITY CHECK HERE, ALSO A WATCH DOG  - EACH CALLBACK SHOULD HAVE A ACTIVE BIT TO CHECK
			
		


		/*
		#ifdef DEBUG_MIDI_TO_TABLET
			while(freeRunningUpCounter10msec){}  // wait until timer counts down
			freeRunningUpCounter10msec = 100;
			_key_load_keyQ (0x33, 0xff);		
		#endif*/
		
		
		if (transfer_complete_spi_master_to_treble )
		{
			transfer_complete_spi_master_to_treble = false;
		}

		do_music_notes();				// do main music algorithms
		do_music_mode();
		 
		TX_usart_Qs();

		move_trebleRxQ_to_keyQ(&trebleRxQ1);
		move_trebleRxQ_to_keyQ(&trebleRxQ2);
		move_trebleRxQ_to_keyQ(&trebleRxQ3);

	} //! [inf_loop]
}//! [main_use_case]


