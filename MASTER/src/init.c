/**
 
init_MidiPCB.C
 
 + */



#include <asf.h>
#include <conf_clocks.h>
#include <stdio.h>
#include <string.h>
#include <tc.h>
#include <usart_interrupt.h>

#include "master.h"
#include "prototypes.h"
#include "..\..\FP\src\fp.h"

void configure_spi_master_to_treble_callbacks(void);
void configure_spi_master_to_treble(void);
void configure_spi_master_to_FP_callbacks(void);
void configure_spi_master_to_FP(void);
void configure_tc_callbacks(void);
void configure_tc(void);
void reset_BT(void);
void clock_dfll(void);
void init_usart_Qs(struct usartQdef *usartQptr);
void configure_ANDROID_usart(void);
void configure_MIDI_usart(void);
void configure_ANDROID_usart_callbacks(void);
void configure_MIDI_usart_callbacks(void);
enum status_code unpair_WT41(struct usartQdef *usartQptr);
	
extern struct FPrxQ_def FPswQ;
extern struct usart_module MIDI_usart_instance, ANDROID_usart_instance;
extern struct spi_module spi_master_to_treble_instance;	
extern struct spi_slave_inst treble_slave_instance[];
extern uint8_t trebleTxBuff[], trebleRdBuff[];
extern struct spi_module spi_master_to_FP_instance;
extern struct spi_slave_inst FP_slave_instance;
extern struct usartQdef midiQ, androidQ;
extern bool MIDI_in_SSP_mode, Android_in_SSP_mode;
extern uint8_t FP_tx_buffer[], FP_rx_buffer[];

uint16_t volatile length;
extern uint8_t volatile spiFreqTimer;


void init(void) 
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(RUN_LED, &pin_conf);
	port_pin_set_output_level(RUN_LED, 0);

	port_pin_set_config(LOOP_IND, &pin_conf);
		
	port_pin_set_config(Reset_BT_PIN, &pin_conf);
	port_pin_set_output_level(Reset_BT_PIN, 1);
	port_pin_set_config(MASTERtoFP_SPI_CS_PIN, &pin_conf);
	port_pin_set_output_level(MASTERtoFP_SPI_CS_PIN, 1);
	
	port_pin_set_config(TREBLE_CS0,&pin_conf);
 	port_pin_set_output_level(TREBLE_CS0, 1);
	port_pin_set_config(TREBLE_CS1,&pin_conf);
 	port_pin_set_output_level(TREBLE_CS1, 1);
	port_pin_set_config(TREBLE_CS2,&pin_conf);
 	port_pin_set_output_level(TREBLE_CS2, 1);


	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;			
	port_pin_set_config(WT41_TABLET_CARRIER_DETECT_PIN, &pin_conf);
	port_pin_set_config(WT41_MIDI_CARRIER_DETECT_PIN, &pin_conf);
	
	
	system_clock_init();
	

	// setup timer
	configure_tc();
	configure_tc_callbacks();
	system_interrupt_enable_global();


	// spi for treble
	configure_spi_master_to_treble();
	configure_spi_master_to_treble_callbacks();
	spi_select_slave(&spi_master_to_treble_instance, treble_slave_instance+2, true);
	spi_transceive_buffer_job(&spi_master_to_treble_instance, trebleTxBuff,trebleRdBuff,1);
	
	// spi for FP (front panel	)
	configure_spi_master_to_FP();
	configure_spi_master_to_FP_callbacks();
	// Start xfers;
	spiFreqTimer = 2;	// limit xfers to 50-100 per second
	FP_tx_buffer[0] = FP_LED_NO_CHANGE;
	spi_select_slave(&spi_master_to_FP_instance, &FP_slave_instance, true);
	spi_transceive_buffer_job(&spi_master_to_FP_instance, FP_tx_buffer,FP_rx_buffer,1);


	struct usartQdef *usartQptr;
	

	usartQptr = &midiQ;
	init_usart_Qs(usartQptr);
	usartQptr = &androidQ;
	init_usart_Qs(usartQptr);
	
	configure_MIDI_usart();
	configure_MIDI_usart_callbacks();

	configure_ANDROID_usart();
	configure_ANDROID_usart_callbacks();
	

	//ANDROID_usart_instance.hw->USART.INTENCLR.reg = SERCOM_USART_INTFLAG_TXC; // disable TX	
	//MIDI_usart_instance.hw->USART.INTENCLR.reg = SERCOM_USART_INTFLAG_TXC; // disable TX

	MIDI_in_SSP_mode =  true;				// let data go into rx q on callbacks during init
	Android_in_SSP_mode = 	true;

	reset_BT();
		
	uint8_t rx_buffer_M[2], rx_buffer_A[2];
	usart_read_buffer_job(&ANDROID_usart_instance,(uint8_t *)rx_buffer_A, 1);				// start RX jobs
	usart_read_buffer_job(&MIDI_usart_instance,(uint8_t *)rx_buffer_M, 1);
	
	wait_10msSec(3);	

// prime the pump

	uint8_t btStr[40] = "at\r\n";
//	put_tx_Q(&midiQ,btStr,strlen((char *)btStr));
//	put_tx_Q(&androidQ,btStr,strlen((char *)btStr));
	usart_write_buffer_wait(&ANDROID_usart_instance, btStr, strlen((char *)btStr));
	usart_write_buffer_wait(&MIDI_usart_instance, btStr, strlen((char *)btStr));  

	// GET BUTTON PUSHES FROM FP
	//	   if 2 buttons pushed - initialize		33 & 25 (21h & 19h)
		wait_10msSec(100);	//{do_FP_SPI();}				// wait for FP xfer

		bool hex19found = false;						// check if the 2 buttons are pushed
		bool hex21found = false;
		for(uint8_t i = 0; i<10; i++)
		{	if (FPswQ.val[i] == 0x19) hex19found = true;
			if (FPswQ.val[i] == 0x21) hex21found = true;
		}
		if (hex19found && hex21found)					// unpair the WT41's and send the initialization info
		{
			put_FP_ledQ(40);	// light the LEDs			
			put_FP_ledQ(32);
			put_FP_ledQ(24);
			put_FP_ledQ(16);
//			wait_10msSec(100);	// wait .1 sec above to be sent
			
			MIDI_in_SSP_mode =  true;				// lets data go into rx q on callbacks
			Android_in_SSP_mode = 	true;
			unpair_WT41(&midiQ);
			unpair_WT41(&androidQ);
	
			strcpy((char *)btStr,"at\r\nat\r\nset bt name ");		// rename
			strcat((char *)btStr,BT_NAME_MIDI_SLAVE);
			strcat((char *)btStr,"\r\n");
			put_tx_Q(&midiQ,btStr,strlen((char *)btStr));
			wait_10msSec(10);	// wait .1 sec above to be sent

			strcpy((char *)btStr,"at\r\nat\r\nset bt name ");		// rename
			length = strlen((char *)btStr);
			strcat((char *)btStr,BT_NAME_TABLET_SLAVE);
			length = strlen((char *)btStr);
			strcat((char *)btStr,"\r\n");
			put_tx_Q(&androidQ,btStr,strlen((char *)btStr));
			wait_10msSec(10);	// wait .1 sec above to be sent
	
			strcpy((char *)btStr,WT41_CARRIER_DETECT_CMD);		
			put_tx_Q(&midiQ,btStr,strlen((char *)btStr));
			put_tx_Q(&androidQ,btStr,strlen((char *)btStr));
			wait_10msSec(50);	// wait .5 sec above to be sent and responses received
	
		}																				 

	// BT
	clock_dfll(); // ramp up gen 0 speed - measured at a gain of x3.6 which would equal about 29MHz	 (did not go higher using the 12M clk)


		

// empty WT-41 ascii responses
/*
	usartQptr = &midiQ;
	init_usart_Qs(usartQptr);
	usartQptr = &androidQ;
	init_usart_Qs(usartQptr);
	MIDI_in_SSP_mode =  false;		// only when true lets data go into rx q on callbacks
	Android_in_SSP_mode = 	false;*/

	
			wait_10msSec(50);	// wait .5 sec above to be sent and responses received


	
}





