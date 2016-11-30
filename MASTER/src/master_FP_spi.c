/*
 
 SPI_MASTER-TREBLE.C
 
*/
#include <asf.h>
#include "master.h"
#include "prototypes.h"
#include <spi.h>
#include "..\..\FP\src\fp.h"


// local functions
void _get_FP_ledQ(uint8_t *ptr);
void _put_mFP_swQ (void);

// init functions
void configure_spi_master_to_FP_callbacks(void);
void configure_spi_master_to_FP(void);

uint8_t FP_tx_buffer[2];
uint8_t FP_rx_buffer[2];

extern uint8_t volatile spiFreqTimer;


struct FPrxQ_def FPswQ;
struct FPtxQ_def FPledQ;

volatile bool transfer_complete_spi_master_to_FP;

struct spi_module spi_master_to_FP_instance;
struct spi_slave_inst FP_slave_instance;

void do_FP_SPI(void)
 {
	
	if (transfer_complete_spi_master_to_FP && spiFreqTimer>1)  // waiting the spiFreqTimer to be < 0 if it was signed
																// NOTE - added the spiFreqTimer to delay the transfers to 50Hz.  
																
																// the CS turns off before the clock was done, on final clk pulse, after the rising, before the falling
																// seems to work ok but leaves a concern
	{
//TEST BY TURNING AN LED MATCHING THE SWITCH
		if (FPledQ.length >0) 
			_get_FP_ledQ(FP_tx_buffer);
		else FP_tx_buffer[0] = FP_LED_NO_CHANGE;
			
	// start the next transfer
		spi_select_slave(&spi_master_to_FP_instance, &FP_slave_instance, true);				 //! [select_slave]
//			port_pin_set_output_level(TREBLE_CS0,0);
		transfer_complete_spi_master_to_FP = false;
		spi_transceive_buffer_job(&spi_master_to_FP_instance,FP_tx_buffer,FP_rx_buffer ,1);
		spiFreqTimer = 1;	// limit xfers to 50-100 per second
	}
}


uint32_t volatile put_mFP_swQCnt1, put_mFP_swQCnt2;
void _put_mFP_swQ (void)
{
	 put_mFP_swQCnt1++;
	 
	if (FPswQ.length < FP_QUE_SIZE-1)
	{
		put_mFP_swQCnt2++;
		FPswQ.val[FPswQ.buff_in] = FP_rx_buffer[0];
		FPswQ.buff_in++;
		FPswQ.buff_in = FPswQ.buff_in & (FP_QUE_SIZE-1);			// wrap at 
		FPswQ.length = FPswQ.buff_in - FPswQ.buff_out;	// can be negative
		FPswQ.length = FPswQ.length & (FP_QUE_SIZE-1);				// make sure positive
	}
}

void put_FP_ledQ (uint8_t val)
{
	if (FPledQ.length < FP_QUE_SIZE-1)
	{
		FPledQ.val[FPledQ.buff_in] = val;
		FPledQ.buff_in++;
		FPledQ.buff_in = FPledQ.buff_in & (FP_QUE_SIZE-1);			// wrap at 
		FPledQ.length = FPledQ.buff_in - FPledQ.buff_out;	// can be negative
		FPledQ.length = FPledQ.length & (FP_QUE_SIZE-1);				// make sure positive
	}
}

void _get_FP_ledQ(uint8_t *ptr)				 // only used in this file
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

void get_FP_swQ(uint8_t *ptr)				// pull FP switch data
{
	if (FPswQ.length >0)
	{
		*ptr = FPswQ.val[FPswQ.buff_out];
		FPswQ.buff_out++;
		FPswQ.buff_out = FPswQ.buff_out & (FP_QUE_SIZE-1);
		FPswQ.length = FPswQ.buff_in - FPswQ.buff_out;			// can be negative
		FPswQ.length = FPswQ.length & (FP_QUE_SIZE-1);		// make sure positive
	}
	else *ptr = FP_SW_NO_CHANGE;
}


//! [callback]
static void spi_master_to_FP_callback(struct spi_module *const module)
{
	// finish this transfer
	transfer_complete_spi_master_to_FP = true;
	if(FP_rx_buffer[0]!=FP_SW_NO_CHANGE) _put_mFP_swQ();
	spi_select_slave(&spi_master_to_FP_instance, &FP_slave_instance, false);				 //! [deselect_slave]

}


//! [conf_callback]
void configure_spi_master_to_FP_callbacks(void)
{
	spi_register_callback(&spi_master_to_FP_instance, spi_master_to_FP_callback,SPI_CALLBACK_BUFFER_TRANSCEIVED);
	spi_enable_callback(&spi_master_to_FP_instance, SPI_CALLBACK_BUFFER_TRANSCEIVED);
}
//! [conf_callback]


//! [configure_spi]
void configure_spi_master_to_FP(void)
{
	struct spi_config config_spi_master;
	struct spi_slave_inst_config slave_dev_config;

	spi_slave_inst_get_config_defaults(&slave_dev_config);
																// setup 3 slave cs
	slave_dev_config.ss_pin = MASTERtoFP_SPI_CS_PIN;
	spi_attach_slave(&FP_slave_instance, &slave_dev_config);

	spi_get_config_defaults(&config_spi_master);
	config_spi_master.generator_source = GCLK_GENERATOR_3;							// generator 3 has a 10 prescaler
	config_spi_master.mode_specific.master.baudrate = 10000;  
	
	config_spi_master.mux_setting = MASTERtoFP_SPI_SERCOM_MUX_SETTING;
	config_spi_master.pinmux_pad0 = MASTERtoFP_SPI_SERCOM_PINMUX_PAD0;
	config_spi_master.pinmux_pad1 = PINMUX_UNUSED;;
	config_spi_master.pinmux_pad2 = MASTERtoFP_SPI_SERCOM_PINMUX_PAD2;
	config_spi_master.pinmux_pad3 = MASTERtoFP_SPI_SERCOM_PINMUX_PAD3;
	spi_init(&spi_master_to_FP_instance, MASTERtoFP_SPI_MODULE, &config_spi_master);
	spi_enable(&spi_master_to_FP_instance);

	FP_tx_buffer[0]	= FP_LED_ALL_OFF;
}
//! [configure_spi]
