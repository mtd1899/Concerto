
#include <asf.h>
//#include <delay.h>
#include <conf_clocks.h>
#include <stdio.h>
#include <string.h>
#include "serial_buffers.h"

extern struct usart_module virtual_SERCOM3, midi_SERCOM4, WT41_SERCOM5;
extern volatile uint8_t rx_buffer_SERCOM3[], rx_buffer_SERCOM5[];

/*
//! [setup]
void configure_usart_SERCOM3(void)
{
//! [setup_config]
	struct usart_config config_usart;
//! [setup_config]
//! [setup_config_defaults]
	usart_get_config_defaults(&config_usart);
//! [setup_config_defaults]

//! [setup_change_config] SERCOM3
	//config_usart.use_external_clock = true;
	
	config_usart.baudrate    = 115200;
	//config_usart.ext_clock_freq = 12000000;
	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;  // unused
	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;  // unused
	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
//! [setup_change_config]

//! [setup_set_config]
	while (usart_init(&virtual_SERCOM3,
			EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {
	}
	virtual_SERCOM3.rx_buffer_ptr = &rx_buffer_SERCOM3;
	
//! [setup_set_config]

//! [setup_enable]
	usart_enable(&virtual_SERCOM3);
//! [setup_enable]
}
*/
//! [setup]
void configure_usart_SERCOM4(void)
{
	//! [setup_config]
	struct usart_config config_usart;
	//! [setup_config]
	//! [setup_config_defaults]
	usart_get_config_defaults(&config_usart);
	//! [setup_config_defaults]

	//! [setup_change_config] SERCOM
	config_usart.baudrate    = 31200;  // midi
	config_usart.generator_source = GCLK_GENERATOR_1;
	config_usart.mux_setting = USART_RX_3_TX_2_XCK_3;
	config_usart.pinmux_pad0 = PINMUX_UNUSED;  // unused
	config_usart.pinmux_pad1 = PINMUX_UNUSED;  // unused
	config_usart.pinmux_pad2 = PINMUX_PB10D_SERCOM4_PAD2;
	config_usart.pinmux_pad3 = PINMUX_PB11D_SERCOM4_PAD3;	
	//! [setup_change_config]

	//! [setup_set_config]
	while (usart_init(&midi_SERCOM4,
	SERCOM4, &config_usart) != STATUS_OK) {
	}
	//! [setup_set_config]

	//! [setup_enable]
	usart_enable(&midi_SERCOM4);
	//! [setup_enable]
}
//! [setup]
void configure_usart_SERCOM5(void)
{
	//! [setup_config]
	struct usart_config config_usart;
	//! [setup_config]
	//! [setup_config_defaults]
	usart_get_config_defaults(&config_usart);
	//! [setup_config_defaults]

	//! [setup_change_config] SERCOM
	config_usart.baudrate    = 115200;
	config_usart.generator_source = GCLK_GENERATOR_1; 
	config_usart.mux_setting = USART_RX_1_TX_0_XCK_1; // USART_RX_3_TX_2_XCK_3; //USART_RX_1_TX_0_XCK_1
	config_usart.pinmux_pad0 = PINMUX_PB16C_SERCOM5_PAD0;  // TX
	config_usart.pinmux_pad1 = PINMUX_PB17C_SERCOM5_PAD1;  // RX
	config_usart.pinmux_pad2 = PINMUX_PB22D_SERCOM5_PAD2;	// UNUSED
	config_usart.pinmux_pad3 = PINMUX_PB23D_SERCOM5_PAD3;	// UNUSED
	//! [setup_change_config]

	//! [setup_set_config]
	while (usart_init(&WT41_SERCOM5, SERCOM5, &config_usart) != STATUS_OK) {
	}
	//! [setup_set_config]

	//! [setup_enable]
	usart_enable(&WT41_SERCOM5);
	//! [setup_enable]
}
