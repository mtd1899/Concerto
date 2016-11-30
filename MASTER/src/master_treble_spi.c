/*
 
 SPI_MASTER-TREBLE.C
 
*/
#include <asf.h>
#include "master.h"
#include "prototypes.h"
#include <spi.h>

// init function calls
void configure_spi_master_to_treble_callbacks(void);
void configure_spi_master_to_treble(void);

// local function calls 
void _put_trebleRxQ (struct trebleRxQ_def *qPtr, uint8_t *ptr);
static void _spi_master_to_treble_callback(struct spi_module *const module);
void _key_load_keyQ (uint8_t key, uint8_t vel);
void _do_treble_spi(void);


extern volatile bool transfer_complete_spi_master_to_treble;

struct spi_module spi_master_to_treble_instance;	
struct spi_slave_inst treble_slave_instance[3];
struct trebleRxQ_def trebleRxQ1, trebleRxQ2, trebleRxQ3;			// this Qs collect the data coming from the treble board via SPI	
uint8_t trebleTxBuff[2] = {0xf7, 0};
uint8_t trebleRdBuff[3];
struct keyQ_def keyQ;								// contains the key data (after data is processed from the trebleRxQs			
uint8_t treble_cs_cnt = 0;


void key_get_keyQ(uint8_t *ptr)					// pulls data from the treble&bass key Q
{
	*ptr = keyQ.key[keyQ.buff_out];
	*(ptr+1) = keyQ.vel[keyQ.buff_out];
	keyQ.buff_out++;
	keyQ.buff_out = keyQ.buff_out & (KEY_QUE_SIZE-1);
	keyQ.length = keyQ.buff_in - keyQ.buff_out;			// can be negative
	keyQ.length = keyQ.length & (KEY_QUE_SIZE-1);		// make sure positive
}






uint32_t txCnt = 0;
uint8_t textCnt;
//! [callback]

uint32_t cnt7f;

void _do_treble_spi(void)			// runs the treble SPI
{
	
		// get treble values
	switch(treble_cs_cnt)			
	{
		case 1:		// finish treble 2, start treble 0
			// finish last read
			spi_select_slave(&spi_master_to_treble_instance, treble_slave_instance+2, false);	 //! [deselect_slave]
			if ((*trebleRdBuff != 0) && (*trebleRdBuff != 0x80) && (*trebleRdBuff != 0xff))
				 _put_trebleRxQ(&trebleRxQ3,trebleRdBuff);
			// start next read - 1st processor
			spi_select_slave(&spi_master_to_treble_instance, treble_slave_instance, true);
			spi_transceive_buffer_job(&spi_master_to_treble_instance, trebleTxBuff,trebleRdBuff,1);
			break;

		case 2:		// treble 1
			// finish last read
			spi_select_slave(&spi_master_to_treble_instance, treble_slave_instance, false);	 //! [deselect_slave]
			if ((*trebleRdBuff != 0) && (*trebleRdBuff != 0x80) && (*trebleRdBuff != 0xff))
				_put_trebleRxQ(&trebleRxQ1,trebleRdBuff);
		// start next read
			spi_select_slave(&spi_master_to_treble_instance, treble_slave_instance+1, true);
			spi_transceive_buffer_job(&spi_master_to_treble_instance, trebleTxBuff,trebleRdBuff,1);
			if (trebleRdBuff[0]==0x7f)	cnt7f++;
			break;

		case 3:		// treble 2
			// finish last read
			spi_select_slave(&spi_master_to_treble_instance, treble_slave_instance+1, false);	 //! [deselect_slave]
			if ((*trebleRdBuff != 0) && (*trebleRdBuff != 0x80) && (*trebleRdBuff != 0xff))
				_put_trebleRxQ(&trebleRxQ2,trebleRdBuff);
			// start next read
			spi_select_slave(&spi_master_to_treble_instance, treble_slave_instance+2, true);
			spi_transceive_buffer_job(&spi_master_to_treble_instance, trebleTxBuff,trebleRdBuff,1);
			treble_cs_cnt = 0;
			break;
	}	
}

// processes the async data in trebleRxQs - and loads the keyQ into sync data, ie, note and velocity
void move_trebleRxQ_to_keyQ(struct trebleRxQ_def *qPtr)
{		// sync to next key & load key Q  
		// moves data from 3 treble
	
	system_interrupt_enter_critical_section();  // critical because the trebleRxQs can be changed in a interrupt callback
	
	if (qPtr->length>=2)
	{
		if ((qPtr->val[qPtr->buff_out] > 0x80) && (qPtr->val[(qPtr->buff_out+1) & (REC_QUE_SIZE-1)] < 0x80) ) 			// if true it's a key #
		{		 											// put a key and velocity on the Q
			_key_load_keyQ(qPtr->val[qPtr->buff_out] & 0x7f, qPtr->val[(qPtr->buff_out + 1) & (REC_QUE_SIZE-1)] );			// load key, vel
			qPtr->buff_out = (qPtr->buff_out + 2) & (REC_QUE_SIZE-1);
			qPtr->length = qPtr->buff_in - qPtr->buff_out;	// update length
			qPtr->length = qPtr->length & (REC_QUE_SIZE-1);				// make sure positive
		}
		else						// 80 is a no key to send - through it away OR OUT OF SYNC
		{
			qPtr->buff_out = (qPtr->buff_out + 1) & (REC_QUE_SIZE-1);		// through it away
			qPtr->length = qPtr->buff_in - qPtr->buff_out;	// update length
			qPtr->length = qPtr->length & (REC_QUE_SIZE-1);				// make sure positive					}
		}
	}
	
	system_interrupt_leave_critical_section();
}



//! [conf_callback]
void configure_spi_master_to_treble_callbacks(void)
{
	spi_register_callback(&spi_master_to_treble_instance, _spi_master_to_treble_callback,SPI_CALLBACK_BUFFER_TRANSCEIVED);
	spi_enable_callback(&spi_master_to_treble_instance, SPI_CALLBACK_BUFFER_TRANSCEIVED);
}
//! [conf_callback]



//! [configure_spi]
void configure_spi_master_to_treble(void)
{
	struct spi_config config_spi_master;
	struct spi_slave_inst_config slave_dev_config;

	spi_slave_inst_get_config_defaults(&slave_dev_config);
																// setup 3 slave cs
	slave_dev_config.ss_pin = TREBLE_CS0;
	spi_attach_slave(&treble_slave_instance[0], &slave_dev_config);
	slave_dev_config.ss_pin = TREBLE_CS1;
	spi_attach_slave(&treble_slave_instance[1], &slave_dev_config);
	slave_dev_config.ss_pin = TREBLE_CS2;
	spi_attach_slave(&treble_slave_instance[2], &slave_dev_config);


	spi_get_config_defaults(&config_spi_master);
	config_spi_master.generator_source = GCLK_GENERATOR_1;
	config_spi_master.mode_specific.master.baudrate = 50000;  
	
	config_spi_master.mux_setting = MASTERtoTREBLE_SPI_SERCOM_MUX_SETTING;
	config_spi_master.pinmux_pad0 = MASTERtoTREBLE_SPI_SERCOM_PINMUX_PAD0;
	config_spi_master.pinmux_pad1 = PINMUX_UNUSED;
	config_spi_master.pinmux_pad2 = MASTERtoTREBLE_SPI_SERCOM_PINMUX_PAD2;
	config_spi_master.pinmux_pad3 = MASTERtoTREBLE_SPI_SERCOM_PINMUX_PAD3;

	spi_init(&spi_master_to_treble_instance, MASTERtoTREBLE_SPI_MODULE, &config_spi_master);
	spi_enable(&spi_master_to_treble_instance);

	trebleTxBuff[0]= SWITCH_CLOSE_MODE_COMMAND; // default

}
//! [configure_spi]

// local functions 
static void _spi_master_to_treble_callback(struct spi_module *const module)
{
	
	treble_cs_cnt++;
		
	transfer_complete_spi_master_to_treble = true;
//	port_pin_toggle_output_level(RUN_LED);					// TEST running output for debugging
	_do_treble_spi();

}
uint32_t volatile p7f;
void _put_trebleRxQ (struct trebleRxQ_def *qPtr, uint8_t *ptr)
{
	if (*ptr==0x7f)
	{	 p7f++;
	}
	if (qPtr->length < REC_QUE_SIZE-1)
	{
		qPtr->val[qPtr->buff_in] = *ptr;
		qPtr->buff_in++;
		qPtr->buff_in = qPtr->buff_in & (REC_QUE_SIZE-1);			// wrap at 
		qPtr->length = qPtr->buff_in - qPtr->buff_out;	// can be negative
		qPtr->length = qPtr->length & (REC_QUE_SIZE-1);				// make sure positive
	}
}

void _key_load_keyQ (uint8_t key, uint8_t vel)
{
	keyQ.key[keyQ.buff_in] = key;
	keyQ.vel[keyQ.buff_in] = vel;
	keyQ.buff_in++;
	keyQ.buff_in = keyQ.buff_in & (KEY_QUE_SIZE-1);			// wrap at KEY_QUE_SIZE
	keyQ.length = keyQ.buff_in - keyQ.buff_out;			// can be negative
	keyQ.length = keyQ.length & (KEY_QUE_SIZE-1);		// make sure positive
}