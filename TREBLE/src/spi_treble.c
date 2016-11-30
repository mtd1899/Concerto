/*
 
 SPI_TREBLE.C
 
*/
#include <asf.h>
#include <spi.h>
#include <spi_interrupt.h>
#include "treble.h"

extern volatile bool transfer_complete_spi_slave;
extern struct spi_module spi_slave_instance;
extern uint8_t tx_note[];
extern uint8_t buffer_rx[];

uint8_t get_keyQ(void);


//! [configure_spi]
void configure_spi_slave(void)
{
//! [config]
	struct spi_config config_spi_slave;
//! [config]
	/* Configure, initialize and enable SERCOM SPI module */
//! [conf_defaults]
	spi_get_config_defaults(&config_spi_slave);
//! [conf_defaults]
//! [conf_spi_slave_instance]
	config_spi_slave.mode = SPI_MODE_SLAVE;
//! [conf_spi_slave_instance]
//! [conf_preload]
	config_spi_slave.mode_specific.slave.preload_enable = true;
//! [conf_preload]
//! [conf_format]
	config_spi_slave.mode_specific.slave.frame_format = SPI_FRAME_FORMAT_SPI_FRAME;
//! [conf_format]
//! [mux_setting]
/*	config_spi_slave.mux_setting = EXT1_SPI_SERCOM_MUX_SETTING;
	//! [mux_setting]
	config_spi_slave.pinmux_pad0 = EXT1_SPI_SERCOM_PINMUX_PAD0;
	config_spi_slave.pinmux_pad1 = EXT1_SPI_SERCOM_PINMUX_PAD1;
	config_spi_slave.pinmux_pad2 = EXT1_SPI_SERCOM_PINMUX_PAD2;
	config_spi_slave.pinmux_pad3 = EXT1_SPI_SERCOM_PINMUX_PAD3;
	spi_init(&spi_slave_instance, EXT1_SPI_MODULE, &config_spi_slave);
	//! [init]	*/
	config_spi_slave.mux_setting = TREBLE_SPI_SERCOM_MUX_SETTING;
	//! [mux_setting]
	/* Configure pad 0 for data in */
	//! [di]
	config_spi_slave.pinmux_pad0 = TREBLE_SPI_SERCOM_PINMUX_PAD0;
	//! [di]
	/* Configure pad 1 as unused */
	//! [ss]
	config_spi_slave.pinmux_pad1 = TREBLE_SPI_SERCOM_PINMUX_PAD1;
	//! [ss]
	/* Configure pad 2 for data out */
	//! [do]
	config_spi_slave.pinmux_pad2 = TREBLE_SPI_SERCOM_PINMUX_PAD2;
	//! [do]
	/* Configure pad 3 for SCK */
	//! [sck]
	config_spi_slave.pinmux_pad3 = TREBLE_SPI_SERCOM_PINMUX_PAD3;
	//! [sck]
	//! [init]
	spi_init(&spi_slave_instance, TREBLE_SPI_MODULE, &config_spi_slave);
	//! [init]

//! [enable]
	spi_enable(&spi_slave_instance);
//! [enable]

}
//! [configure_spi]


uint8_t called;

void doADCstuff(void);

void spi_transceive_one(
		struct spi_module *const module,
		uint8_t *tx_data,
		uint8_t *rx_data,
		uint16_t length)
{
	Assert(module);
	Assert(tx_data);
/*

	if (length == 0) {
		return STATUS_ERR_INVALID_ARG;
	}

	if (!(module->receiver_enabled)) {
		return STATUS_ERR_DENIED;
	}

	// Check if the SPI is busy transmitting or slave waiting for TXC* /
	if (module->status == STATUS_BUSY) {
		return STATUS_BUSY;
	}
*/

	// Write parameters to the device instance */
	module->remaining_tx_buffer_length = length;
	module->remaining_rx_buffer_length = length;
	module->rx_buffer_ptr = rx_data;
	module->tx_buffer_ptr = tx_data;
	module->status = STATUS_BUSY;

	module->dir = SPI_DIRECTION_BOTH;

	// Get a pointer to the hardware module instance */
	SercomSpi *const hw = &(module->hw->SPI);
//		hw->INTENSET.reg = (SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY | SPI_INTERRUPT_FLAG_RX_COMPLETE | SPI_INTERRUPT_FLAG_TX_COMPLETE);
	hw->INTENSET.reg = (0 | SPI_INTERRUPT_FLAG_TX_COMPLETE);

}


struct que {
	uint8_t val [32];		// don't change - needs to roll over
	uint8_t buff_in;					// buffer pointers
	uint8_t buff_out;
};

struct que recQ;
/*

enum status_code put_rec_Q (uint8_t *ptr)
{
	if (((32-1) & (recQ.buff_in + 1)) != recQ.buff_out){
		recQ.val[recQ.buff_in] = *ptr;
		recQ.buff_in++;
		recQ.buff_in = recQ.buff_in & (32-1);			// wrap at KEY_QUE_SIZE
	}
	else return STATUS_ERR_DENIED;
	return STATUS_OK;
}
*/


uint8_t spi_read_byte(void)
{
	uint8_t received_data = (spi_slave_instance.hw->USART.DATA.reg & SERCOM_SPI_DATA_MASK);

	return received_data;
}

extern 	uint8_t velocityCmdRx;
extern struct keyQ_def keyQ;

void spi_slave_callbacks (void)
{	
//	port_pin_set_output_level(TP41_PIN,0);
//	port_pin_toggle_output_level(TP41_PIN);	

	velocityCmdRx = 	*buffer_rx;
	transfer_complete_spi_slave= true;

	if (( keyQ.buff_in - keyQ.buff_out) != 0)			// key in Q
	{
		tx_note[0] = get_keyQ();
	}
	else											// empty
	{
		tx_note[0] = 0x80;
	}
	spi_transceive_one(&spi_slave_instance, tx_note,buffer_rx,1);
}

void configure_spi_slave_callbacks(void)
{
	spi_register_callback(&spi_slave_instance, spi_slave_callbacks,SPI_CALLBACK_BUFFER_TRANSMITTED);
	spi_enable_callback(&spi_slave_instance, SPI_CALLBACK_BUFFER_TRANSMITTED);

}

/* from Datasheet
Slave
When configured as a slave (CTRLA.MODE is 0x2), the SPI interface will remain inactive, with the MISO line tri-stated as
long as the _SS pin is pulled high. Software may update the contents of DATA at any time, as long as the Data Register
Empty flag in the Interrupt Status and Clear register (INTFLAG.DRE) is set.
When _SS is pulled low and SCK is running, the slave will sample and shift out data according to the transaction mode
set. When the contents of TxDATA have been loaded into the shift register, INTFLAG.DRE is set, and new data can be
written to DATA. Similar to the master, the slave will receive one character for each character transmitted. On the same
clock cycle as the last data bit of a character is received, the character will be transferred into the two-level receive buffer.
The received character can be retrieved from DATA when INTFLAG.RCX is set.


NOTE - USE THIS
When the master pulls the _SS line high, the transaction is done and the Transmit Complete Interrupt flag in the Interrupt
Flag Status and Clear register (TXC) is set.


Once DATA is written, it takes up to three SCK clock cycles before the content of DATA is ready to be loaded into the
shift register. When the content of DATA is ready to be loaded, this will happen on the next character boundary. As a
consequence, the first character transferred in a SPI transaction will not be the content of DATA. This can be avoided by
using the preloading feature.
Refer to “Preloading of the Slave Shift Register” on page 375.
When transmitting several characters in one SPI transaction, the data has to be written to DATA while there are at least
three SCK clock cycles left in the current character transmission. If this criteria is not met, then the previous character
received will be transmitted.
After the DATA register is empty, it takes three CLK_SERCOM_APB cycles for INTFLAG.DRE to be set.
*/
