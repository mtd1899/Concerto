/* 
 SPI_FP.C
 
*/
#include <asf.h>
#include <spi.h>
#include <spi_interrupt.h>
#include "FP.h"

void put_FP_swQ (uint8_t val);
void put_FP_ledQ (void);
void get_FP_swQ(uint8_t *ptr);
void get_FP_ledQ(uint8_t *ptr);
void configure_spi_slave_callbacks(void);
void configure_spi_slave(void);


extern volatile bool transfer_complete_spi_tx,transfer_complete_spi_rx;
extern struct spi_module spi_slave_instance;
//! [buffer]
uint8_t tx_buffer[2];
uint8_t rx_buffer[2];


struct FPrxQ_def FPswQ;
struct FPtxQ_def FPledQ;


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

	//! [init]	*/
	config_spi_slave.mux_setting = FP_SPI_SERCOM_MUX_SETTING;
	//! [mux_setting]
	/* Configure pad 0 for data in */
	//! [di]
	config_spi_slave.pinmux_pad0 = FP_SPI_SERCOM_PINMUX_PAD0;
	//! [di]
	/* Configure pad 1 as unused */
	//! [ss]
	config_spi_slave.pinmux_pad1 = FP_SPI_SERCOM_PINMUX_PAD1;
	//! [ss]
	/* Configure pad 2 for data out */
	//! [do]
	config_spi_slave.pinmux_pad2 = FP_SPI_SERCOM_PINMUX_PAD2;
	//! [do]
	/* Configure pad 3 for SCK */
	//! [sck]
	config_spi_slave.pinmux_pad3 = FP_SPI_SERCOM_PINMUX_PAD3;
	//! [sck]
	//! [init]
	spi_init(&spi_slave_instance, FP_SPI_MODULE, &config_spi_slave);
	//! [init]

//! [enable]
	spi_enable(&spi_slave_instance);
//! [enable]

}
//! [configure_spi]


uint8_t called;

void doADCstuff(void);

uint32_t txCnt;
/*
void spi_transceive_one( struct spi_module *const module,uint8_t *tx_data,uint8_t *rx_data);

void spi_transceive_one(
		struct spi_module *const module,
		uint8_t *tx_data,
		uint8_t *rx_data)
{
	Assert(module);
	Assert(tx_data);
/ *

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
* /

	// Write parameters to the device instance * /
	module->remaining_tx_buffer_length = 1;
	module->remaining_rx_buffer_length = 1;
	module->rx_buffer_ptr = rx_data;
	module->tx_buffer_ptr = tx_data;

	module->status = STATUS_BUSY;

	module->dir = SPI_DIRECTION_BOTH;

	// Get a pointer to the hardware module instance * /
	SercomSpi *const hw = &(module->hw->SPI);
//		hw->INTENSET.reg = (SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY | SPI_INTERRUPT_FLAG_RX_COMPLETE | SPI_INTERRUPT_FLAG_TX_COMPLETE);
//	hw->INTENSET.reg = (0 | SPI_INTERRUPT_FLAG_TX_COMPLETE);
  	hw->INTENSET.reg = (0 | SPI_INTERRUPT_FLAG_RX_COMPLETE);

}*/

void put_FP_swQ (uint8_t val)
{
	if (FPswQ.length < FP_QUE_SIZE-1)
	{
		FPswQ.val[FPswQ.buff_in] = val;
		FPswQ.buff_in++;
		FPswQ.buff_in = FPswQ.buff_in & (FP_QUE_SIZE-1);			// wrap at KEY_QUE_SIZE
		FPswQ.length = FPswQ.buff_in - FPswQ.buff_out;				// can be negative
		FPswQ.length = FPswQ.length & (FP_QUE_SIZE-1);				// make sure positive
	}
}

void put_FP_ledQ (void)
{
	FPledQ.val[FPledQ.buff_in] =  rx_buffer[0];
	FPledQ.buff_in++;
	FPledQ.buff_in = FPledQ.buff_in & (FP_QUE_SIZE-1);			// wrap at KEY_QUE_SIZE
	FPledQ.length = FPledQ.buff_in - FPledQ.buff_out;	// can be negative
	FPledQ.length = FPledQ.length & (FP_QUE_SIZE-1);				// make sure positive
}

void get_FP_ledQ(uint8_t *ptr)				 // only used in this file
{
	if (FPledQ.length >0)
	{
		*ptr = FPledQ.val[FPledQ.buff_out];
		FPledQ.buff_out++;
		FPledQ.buff_out = FPledQ.buff_out & (FP_QUE_SIZE-1);
		FPledQ.length = FPledQ.buff_in - FPledQ.buff_out;			// can be negative
		FPledQ.length = FPledQ.length & (FP_QUE_SIZE-1);		// make sure positive
	}
}

void get_FP_swQ(uint8_t *ptr)
{
	if (FPswQ.length >0)
	{
		*ptr = FPswQ.val[FPswQ.buff_out];
		FPswQ.buff_out++;
		FPswQ.buff_out = FPswQ.buff_out & (FP_QUE_SIZE-1);
		FPswQ.length = FPswQ.buff_in - FPswQ.buff_out;			// can be negative
		FPswQ.length = FPswQ.length & (FP_QUE_SIZE-1);		// make sure positive
	}
}

uint8_t spi_read_byte(void);
uint8_t spi_read_byte(void)
{
	uint8_t received_data = (spi_slave_instance.hw->USART.DATA.reg & SERCOM_SPI_DATA_MASK);

	return received_data;
}

uint8_t valrx[2000];
uint32_t valrxCnt;

/*
static void spi_slave_callbacks (struct spi_module *const module)
{
	// finish this transfer
	transfer_complete_spi_slave= true;
	if ((rx_buffer[0]!=FP_LED_NO_CHANGE) && (rx_buffer[0]!=0) && (FPledQ.length < FP_QUE_SIZE-1)) put_FP_ledQ();
	if (valrxCnt<2000)
	{
		valrx[valrxCnt] = rx_buffer[0];
		valrxCnt++;
	}
}*/
static void spi_rx_callbacks (struct spi_module *const module)
{
	// finish this transfer
	transfer_complete_spi_rx= true;
/*
	if ((rx_buffer[0]!=FP_LED_NO_CHANGE) && (rx_buffer[0]!=0) && (FPledQ.length < FP_QUE_SIZE-1)) put_FP_ledQ();
	if (valrxCnt<2000)
	{
		valrx[valrxCnt] = rx_buffer[0];
		valrxCnt++;
	}*/
}
static void spi_tx_callbacks (struct spi_module *const module)
{	
	// finish this transfer
	transfer_complete_spi_tx= true;
}



void configure_spi_slave_callbacks(void)
{
/*
	spi_register_callback(&spi_slave_instance, spi_slave_callbacks,SPI_CALLBACK_BUFFER_TRANSCEIVED);
	spi_enable_callback(&spi_slave_instance, SPI_CALLBACK_BUFFER_TRANSCEIVED);
	*/
	spi_register_callback(&spi_slave_instance, spi_tx_callbacks,SPI_CALLBACK_BUFFER_TRANSMITTED);
	spi_enable_callback(&spi_slave_instance, SPI_CALLBACK_BUFFER_TRANSMITTED);
	

	spi_register_callback(&spi_slave_instance, spi_rx_callbacks,SPI_CALLBACK_BUFFER_RECEIVED);
	spi_enable_callback(&spi_slave_instance, SPI_CALLBACK_BUFFER_RECEIVED);
		
	transfer_complete_spi_tx= true;
	transfer_complete_spi_rx= false;
//	transfer_complete_spi_slave = true;
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
