
#include <asf.h>
//#include <delay.h>
#include <conf_clocks.h>
#include <stdio.h>
#include <string.h>

#include "master.h"
#include "prototypes.h"
// initialization
void configure_ANDROID_usart(void);
void configure_MIDI_usart(void);
void configure_ANDROID_usart_callbacks(void);
void configure_MIDI_usart_callbacks(void);
void init_usart_Qs(struct usartQdef *usartQptr);


void _MIDI_usart_write_callback(struct usart_module *const usart_module);
void _MIDI_usart_read_callback(struct usart_module *const usart_module);
void _ANDROID_usart_write_callback(struct usart_module *const usart_module);
void _ANDROID_usart_read_callback(struct usart_module *const usart_module);

// local functions
enum status_code _usart_write_SERCOM(struct usart_module *const module, const uint8_t tx_data);


struct usart_module MIDI_usart_instance, ANDROID_usart_instance;
struct usartQdef midiQ, androidQ;

extern bool MIDI_in_SSP_mode, Android_in_SSP_mode;


static inline uint8_t midi_tx_Q_length (void)
{
	return (midiQ.tx_in - midiQ.tx_out);
}

static inline uint8_t midi_rx_Q_length (void)
{
	return (midiQ.rx_in - midiQ.rx_out);
}
static inline uint8_t android_rx_Q_length (void)
{
	return (androidQ.rx_in - androidQ.rx_out);
}
static inline uint8_t android_tx_Q_length (void)
{
	return (androidQ.tx_in - androidQ.tx_out);
}

/*-----------------------------------------------------------------------------
PUT INTO Qs
------------------------------------------------------------------------------*/
enum status_code put_tx_Q (struct usartQdef *usartQptr, uint8_t *dataPtr, uint8_t dataLen)  // puts byte in the USART transmit Q
{	uint8_t i;
	if ((midiQ.tx_in - midiQ.tx_out) < (MIDI_Q_SIZE-dataLen))
	{
		for (i=0; i<dataLen; i++) 
		{	usartQptr->tx[usartQptr->tx_in] = *(dataPtr+i);
			usartQptr->tx_in++;
		}
			
		return STATUS_OK;													                    
	}
	else return STATUS_BUSY;
}

enum status_code put_rx_Q (struct usartQdef *usartQptr, uint8_t *dataPtr, uint8_t dataLen)        // puts byte in the USART transmit Q
{	uint8_t i;
	if ((midiQ.rx_in - midiQ.rx_out) < (MIDI_Q_SIZE-dataLen)) 
	{
		for (i=0; i<dataLen; i++) usartQptr->rx[usartQptr->rx_in++] = *(dataPtr+i);
		return STATUS_OK;
	}
	else return STATUS_BUSY;
}

/*-----------------------------------------------------------------------------
INIT/SET Qs TO 0
------------------------------------------------------------------------------*/
void init_usart_Qs(struct usartQdef *usartQptr)
{	uint8_t i;
	for (i=0; i<255; i++)
	{
		usartQptr->tx[i] = usartQptr->rx[i] = 0;
	}
	usartQptr->tx_in = 0;
	usartQptr->tx_out = 0;
	usartQptr->rx_in = 0;
	usartQptr->rx_out = 0;
}



//enum status_code _usart_write_SERCOM(struct usart_module *const module, const uint8_t tx_data);
//! [setup]
void configure_ANDROID_usart(void)
{
	//! [setup_config]
	struct usart_config config_usart;
	//! [setup_config]
	//! [setup_config_defaults]
	usart_get_config_defaults(&config_usart);
	//! [setup_config_defaults]

	//! [setup_change_config] SERCOM
	config_usart.baudrate    = 115200;  // midi
	config_usart.generator_source = GCLK_GENERATOR_4;
	config_usart.mux_setting = USART_RX_3_TX_2_XCK_3;
	config_usart.pinmux_pad0 = PINMUX_UNUSED;  // unused
	config_usart.pinmux_pad1 = PINMUX_UNUSED;  // unused
	config_usart.pinmux_pad2 = PINMUX_PB10D_SERCOM4_PAD2;
	config_usart.pinmux_pad3 = PINMUX_PB11D_SERCOM4_PAD3;	
	//! [setup_change_config]

	//! [setup_set_config]
	while (usart_init(&ANDROID_usart_instance,	SERCOM4, &config_usart) != STATUS_OK) {
	}
	//! [setup_set_config]

	//! [setup_enable]
	usart_enable(&ANDROID_usart_instance);
	//! [setup_enable]
}
//! [setup]
void configure_MIDI_usart(void)
{	    
	//! [setup_config]
	struct usart_config config_usart;
	//! [setup_config]
	//! [setup_config_defaults]
	usart_get_config_defaults(&config_usart);
	//! [setup_config_defaults]

	//! [setup_change_config] SERCOM
	config_usart.baudrate    = 115200;
	config_usart.generator_source = GCLK_GENERATOR_4; 
	config_usart.mux_setting = USART_RX_1_TX_0_XCK_1; // USART_RX_3_TX_2_XCK_3; //USART_RX_1_TX_0_XCK_1
	config_usart.pinmux_pad0 = PINMUX_PB16C_SERCOM5_PAD0;  // TX
	config_usart.pinmux_pad1 = PINMUX_PB17C_SERCOM5_PAD1;  // RX
	config_usart.pinmux_pad2 = PINMUX_PB22D_SERCOM5_PAD2;	// UNUSED
	config_usart.pinmux_pad3 = PINMUX_PB23D_SERCOM5_PAD3;	// UNUSED
	//! [setup_change_config]

	//! [setup_set_config]
	while (usart_init(&MIDI_usart_instance, SERCOM5, &config_usart) != STATUS_OK) {
	}
	//! [setup_set_config]

	//! [setup_enable]
	usart_enable(&MIDI_usart_instance);
	//! [setup_enable]
}


bool MIDI_usart_tx_done = true;
bool MIDI_usart_rx_done = true;
bool ANDROID_usart_tx_done = true;
bool ANDROID_usart_rx_done = true;

uint8_t ANDROID_usartTimeout=0;				
uint8_t MIDI_usartTimeout=0;

void _MIDI_usart_read_callback(struct usart_module *const usart_module)
{
	MIDI_usart_rx_done = true;
	uint8_t received_data = *((usart_module->rx_buffer_ptr)-1);
	if (MIDI_in_SSP_mode)						// write only if in_SSP_mode
	{
		MIDI_usartTimeout = 0;									// reset it - data moving
		system_interrupt_disable_global();
		if ((midiQ.rx_in + 1) != midiQ.rx_out) midiQ.rx[midiQ.rx_in++] = received_data;
		system_interrupt_enable_global();
	}

}

void _ANDROID_usart_read_callback(struct usart_module *const usart_module)
{
	ANDROID_usart_rx_done = true;
	
	uint8_t received_data = *((usart_module->rx_buffer_ptr)-1);
	if (Android_in_SSP_mode)						// write only if in_SSP_mode
	{
		ANDROID_usartTimeout = 0;									// reset it - data moving
		system_interrupt_disable_global();
		if ((androidQ.rx_in + 1) != androidQ.rx_out) androidQ.rx[androidQ.rx_in++] = received_data;
		system_interrupt_enable_global();
	}
}

void _MIDI_usart_write_callback(struct usart_module *const usart_module)
{
	//	port_pin_set_output_level(PIN_PB16, 1);
	MIDI_usart_tx_done = true;
}

void _ANDROID_usart_write_callback(struct usart_module *const usart_module)
{
	//	port_pin_set_output_level(PIN_PB16, 1);
	ANDROID_usart_tx_done = true;
}


void configure_MIDI_usart_callbacks(void)
{
	//! [setup_register_callbacks]
	usart_register_callback(&MIDI_usart_instance,_MIDI_usart_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_register_callback(&MIDI_usart_instance,_MIDI_usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	//! [setup_register_callbacks]

	//! [setup_enable_callbacks]
	usart_enable_callback(&MIDI_usart_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&MIDI_usart_instance, USART_CALLBACK_BUFFER_RECEIVED);
	//! [setup_enable_callbacks]
}

void configure_ANDROID_usart_callbacks(void)
{
	//! [setup_register_callbacks]
	usart_register_callback(&ANDROID_usart_instance,_ANDROID_usart_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_register_callback(&ANDROID_usart_instance,_ANDROID_usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	//! [setup_register_callbacks]

	//! [setup_enable_callbacks]
	usart_enable_callback(&ANDROID_usart_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&ANDROID_usart_instance, USART_CALLBACK_BUFFER_RECEIVED);
	//! [setup_enable_callbacks]
}

/*
enum status_code _usart_write_SERCOM(struct usart_module *const module, const uint8_t tx_data)
{
	/ Sanity check arguments   /
	Assert(module);
	Assert(module->hw);

	/   Get a pointer to the hardware module instance   /
	SercomUsart  const usart_hw = &(module->hw->USART);

	if (!(usart_hw->INTFLAG.reg & SERCOM_USART_INTFLAG_TXC))
		return STATUS_BUSY;

	/   Check that the transmitter is enabled   /
	if (!(module->transmitter_enabled)) {
		return STATUS_ERR_DENIED;
	}

	/   Wait until synchronization is complete   /
	_usart_wait_for_sync(module);

	/   Write data to USART module   /
	usart_hw->DATA.reg = tx_data;

	return STATUS_OK;

}*/

uint8_t tx_data;
	
void TX_usart_Qs(void)											// TX function - call often
{	enum status_code status_code;
	uint8_t *txPtr;
	
	/* write from the Q to the WT41
	
			// note - only writing one at a time - should slow speed down  
			the baud rate here is 115200 but into the sound card its 31250 

	*/
	txPtr = &tx_data;
	
	if (android_tx_Q_length()>0)	{
		tx_data = androidQ.tx[androidQ.tx_out];
		status_code  = usart_write_buffer_job(&ANDROID_usart_instance, txPtr,1);
		if (status_code == STATUS_OK)
		{
			androidQ.tx_out++;			// write ok, inc ptr
		}
	}


	if (midi_tx_Q_length()>0)	{
		tx_data = midiQ.tx[midiQ.tx_out];
		status_code  = usart_write_buffer_job(&MIDI_usart_instance, txPtr,1);
		if (status_code == STATUS_OK) {
			midiQ.tx_out++;			// write ok, inc ptr
		}
	}
}
				