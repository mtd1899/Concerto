/**
 
init_MidiPCB.C
 
 + */



#include <asf.h>
#include <conf_clocks.h>
#include <stdio.h>
#include <string.h>
#include "serial_buffers.h"
#include "tc_stuff.h"
#include <tc.h>
#include <usart_interrupt.h>
#include "MidiBT_pcb.h"

//enum status_code _usart_write_buffer( struct usart_module *const module,uint8_t *tx_data,uint16_t length);
//enum status_code _usart_read_buffer(struct usart_module *const module,uint8_t *rx_data,uint16_t length);

void usart_read_callback_SERCOM5(struct usart_module *const usart_module);
void usart_write_callback_SERCOM5(struct usart_module *const usart_module);
void usart_read_callback_SERCOM4(struct usart_module *const usart_module);
void usart_write_callback_SERCOM4(struct usart_module *const usart_module);


void configure_usart(void);
void configure_usart_callbacks(void);

void configure_tc4(void);
void configure_tc4_callbacks(void);


void wait_sec(uint32_t wait);
void wait_10msSec(uint32_t wait);

//! [module_inst]
struct usart_module midi_SERCOM4, WT41_SERCOM5;
//! [module_inst]

//! [setup]

extern 	uint32_t clockhz, msCounter;
extern struct bufferDef serialQ;
extern char cStr[];
extern bool BTinSSPmode;

volatile uint8_t rx_buffer_SERCOM4[MAX_RX_BUFFER_LENGTH],rx_buffer_SERCOM5[MAX_RX_BUFFER_LENGTH];
void reset_BT(void);

// Reset BT module
void reset_BT(void)
{
	port_pin_set_output_level(Reset_BT_PIN,0);

	msCounter=10;
	while (msCounter){}

	port_pin_set_output_level(Reset_BT_PIN,1);
	port_pin_set_output_level(Green_LED_PIN,1);
	
	msCounter=50;
	while (msCounter){}
}
	

void usart_read_callback_SERCOM4(struct usart_module *const usart_module)
{
	uint8_t received_data = *(usart_module->rx_buffer_ptr);

	system_interrupt_disable_global();
	if ((serialQ.midi_in + 1) != serialQ.midi_out) serialQ.midiToBlueTooth[serialQ.midi_in++] = received_data;
	system_interrupt_enable_global();

}

void usart_write_callback_SERCOM5(struct usart_module *const usart_module)
{
//	port_pin_toggle_output_level(LED_0_PIN);
}

void usart_read_callback_SERCOM5(struct usart_module *const usart_module)
{
	uint8_t received_data = *(usart_module->rx_buffer_ptr);
	if (((serialQ.bt_in + 1) != serialQ.bt_out))
	{	
		serialQ.blueToothToMidi[serialQ.bt_in++] = received_data;

	}
}

void usart_write_callback_SERCOM4(struct usart_module *const usart_module)
{
//	port_pin_toggle_output_level(LED_0_PIN);
}
//! [callback_funcs]

//! [setup]


void configure_usart_callbacks(void)
{
	//! [setup_register_callbacks]
	usart_register_callback(&WT41_SERCOM5, usart_write_callback_SERCOM5, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_register_callback(&WT41_SERCOM5,usart_read_callback_SERCOM5, USART_CALLBACK_BUFFER_RECEIVED);
	//! [setup_register_callbacks]

	//! [setup_enable_callbacks]
	usart_enable_callback(&WT41_SERCOM5, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&WT41_SERCOM5, USART_CALLBACK_BUFFER_RECEIVED);
	//! [setup_enable_callbacks]

	//! [setup_register_callbacks]
	usart_register_callback(&midi_SERCOM4, usart_write_callback_SERCOM4, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_register_callback(&midi_SERCOM4,usart_read_callback_SERCOM4, USART_CALLBACK_BUFFER_RECEIVED);
	//! [setup_register_callbacks]

	//! [setup_enable_callbacks]
	usart_enable_callback(&midi_SERCOM4, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&midi_SERCOM4, USART_CALLBACK_BUFFER_RECEIVED);
	//! [setup_enable_callbacks]

}
//! [setup]

void init(void) ;

void init(void) 
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	system_clock_init();
	
	/* Configure outputs, set high */
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(Red_LED_PIN, &pin_conf);
	port_pin_set_output_level(Red_LED_PIN, 1);
	port_pin_set_config(Yellow_LED_PIN, &pin_conf);
	port_pin_set_output_level(Yellow_LED_PIN, 1);
	port_pin_set_config(Green_LED_PIN, &pin_conf);
	port_pin_set_output_level(Green_LED_PIN, 1);
	port_pin_set_config(Reset_BT_PIN, &pin_conf);
	port_pin_set_output_level(Reset_BT_PIN, 1);	

	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;			
	port_pin_set_config(UNPAIR_PUSH_BUTTON, &pin_conf);
	port_pin_set_config(WT41_CARRIER_DETECT_PIN, &pin_conf);
	


	configure_tc4();
	configure_tc4_callbacks();	
	

	configure_usart_SERCOM5();
	configure_usart_SERCOM4();
	init_serial_buffers();

	
	configure_usart_callbacks();
		

	WT41_SERCOM5.hw->USART.INTENCLR.reg = SERCOM_USART_INTFLAG_TXC; // disable TX	
	midi_SERCOM4.hw->USART.INTENCLR.reg = SERCOM_USART_INTFLAG_TXC; // disable TX
	system_interrupt_enable_global();

	usart_read_buffer_job(&WT41_SERCOM5,(uint8_t *)rx_buffer_SERCOM5, MAX_RX_BUFFER_LENGTH);
	usart_read_buffer_job(&midi_SERCOM4,(uint8_t *)rx_buffer_SERCOM4, MAX_RX_BUFFER_LENGTH);
	
	wait_10msSec(20);	// wait .2 sec above to complete

// prime the pump
	uint8_t string[] = "at\r\n";
	usart_write_buffer_wait(&WT41_SERCOM5, string, sizeof(string));
	usart_write_buffer_wait(&midi_SERCOM4, string, sizeof(string));


}


uint16_t *nPtr, next;

//enum status_code usart_write_wait(struct usart_module *const module,	const uint16_t tx_data)
enum status_code usart_write_SERCOM(struct usart_module *const module, const uint8_t tx_data);

enum status_code usart_write_SERCOM(struct usart_module *const module, const uint8_t tx_data)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	/* Get a pointer to the hardware module instance */
	SercomUsart *const usart_hw = &(module->hw->USART);

	if (!(usart_hw->INTFLAG.reg & SERCOM_USART_INTFLAG_TXC))return STATUS_BUSY;

	/* Check that the transmitter is enabled */
	if (!(module->transmitter_enabled)) {
		return STATUS_ERR_DENIED;
	}

	/* Wait until synchronization is complete */
	_usart_wait_for_sync(module);

	/* Write data to USART module */
	usart_hw->DATA.reg = tx_data;

	return STATUS_OK; 

}
