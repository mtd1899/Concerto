/*			 prototypes.h 

function prototypes and descriptions

*/
// OPERATIONAL FUNCTION CALLS
	// BT (Blue Tooth) MIDI and Android
			void TX_usart_Qs(void);	// transmits values in Qs - call often
 									// MUST BE CALLED DURING NORMAL OPERATION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// FP (Frount Panel)
			void do_FP_SPI(void);  // cycles the FP SPI communication - loading and unloading the FP LED and Switch Qs
									// MUST BE CALLED DURING NORMAL OPERATION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TREBLE & BASS
			void move_trebleRxQ_to_keyQ(struct trebleRxQ_def *qPtr);  // processes the async data in trebleRxQs - and loads the keyQ into sync data, ie, note and velocity
								/*	  data is received in 3 Qs, one from each treble processor, the data is async between the Qs
									  this routine combines the information into one Q for the music routines to process.
								*/									
									// MUST BE CALLED DURING NORMAL OPERATION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// INITIALIZATION
		// init.c
		void init(void);	//	performs all initialization


// MUSIC NOTES
		// music.c
		void do_music_notes(void);		// handle music note from the treble CPUs
							/*        currently dummy module that reads a note and sends a midi note on/off command
							*/
		void key_get_keyQ(uint8_t *ptr); // pulls data from the treble&bass key Q					
							/*		retrieve key data to process by the music algorithms
							  */
		enum status_code put_tx_Q (struct usartQdef *usartQptr, uint8_t *dataPtr, uint8_t dataLen); // puts byte in the USART transmit Q	
		enum status_code put_rx_Q (struct usartQdef *usartQptr, uint8_t *dataPtr, uint8_t dataLen);	 /*
									sends to both midi and android Qs
								*/
// MUSIC MODES
		// music.c
		void do_music_mode(void);		// handle music mode
							/*        currently dummy module the take a FP button push and turns on an LED
							*/
		void get_FP_swQ(uint8_t *ptr);	// pull FP switch data for processing
							/* value & 0x7f is the switch number
							if FP_SW_OPEN_FLAG	0x80 bit is set the switch has been released
							*/

		void put_FP_ledQ (uint8_t val);		//puts val in the ledQ - where the communication routines send it to the front panel
							/*	val = the LED # to turn on
								val = LED # +  FP_LED_OFF_FLAG to turn off
								val = FP_LED_ALL_OFF will turn off all LEDs
								see FP.h for the commands */
		

// Timer/Counter (RTC)
		void wait_10msSec(uint32_t wait);	// wait = number of 10ms to wait.  example: wait_10msSec(100) waits 1 second
							/* cycles the FPI - SPI and the BT routines while waiting 
							*/
							
// Blue Tooth
		void ck_SSP_mode(void);			// sets state bool vars & runs LEDs
