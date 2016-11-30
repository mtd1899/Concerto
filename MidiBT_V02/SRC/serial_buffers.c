/******************************************************************************/
/*                                                                            */
/*       Qs for rx and tx

                                                                              */
/******************************************************************************/
#include <asf.h>
#include <stdio.h>
#include <string.h>
#include "serial_buffers.h"
#include "status_codes.h"
#include "MidiBT_pcb.h"

struct bufferDef serialQ;
char lastBT;

struct bufferDef *init_serial_buffers(void)
{	uint16_t i;
	for (i=0; i<255; i++)
	{
		serialQ.blueToothToMidi[i] = serialQ.midiToBlueTooth[i] = 0; 
	}
	serialQ.bt_in = 0;
	serialQ.bt_out = 0;
	serialQ.midi_in = 0;
	serialQ.midi_out = 0;
	return &serialQ;	
}
/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
enum status_code bt_to_midi_loadQ (uint8_t data)
{
	if ((serialQ.bt_in + 1) != serialQ.bt_out) serialQ.blueToothToMidi[serialQ.bt_in++] = lastBT = data;
		else return STATUS_ERR_DENIED;
	return STATUS_OK;	
}
enum status_code midi_to_bt_loadQ (uint8_t data)
{
	if ((serialQ.midi_in + 1) != serialQ.midi_out) serialQ.midiToBlueTooth[serialQ.midi_in++] = data;
		else return STATUS_ERR_DENIED;
	return STATUS_OK;
}


void load_iWrap_Cmd( char *str)
{
	uint8_t i;
	unsigned int len;
	len = strlen((const char*)str);
	for (i = 0; i < len; i++)
	{
		system_interrupt_disable_global();
		midi_to_bt_loadQ(*str);
		system_interrupt_enable_global();
		str++;	
	}
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/

uint16_t blen, mlen;

uint8_t bt_to_midi_length (void)
{	uint16_t temp = 0;
	if (serialQ.bt_in != serialQ.bt_out) 
	{	if (serialQ.bt_in > serialQ.bt_out) 
			temp = serialQ.bt_in - serialQ.bt_out;
		else
		{
			temp = 256 - serialQ.bt_out + serialQ.bt_in;
		}
	}
	blen = temp;
	return (temp);
}
uint8_t midi_to_bt_length (void)
{	uint16_t temp =0;
	if (serialQ.midi_in != serialQ.midi_out) 
	{
		if (serialQ.midi_in > serialQ.midi_out) 
			temp = serialQ.midi_in - serialQ.midi_out;
		else
		{
			temp = 256 - serialQ.midi_out + serialQ.midi_in;
		}
	}
	mlen= temp;
	return (temp);
}


enum status_code usart_write( struct usart_module *const module, const uint16_t tx_data)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	/* Get a pointer to the hardware module instance */
	SercomUsart *const usart_hw = &(module->hw->USART);

	/* Check that the transmitter is enabled */
	if (!(module->transmitter_enabled)) {
		return STATUS_ERR_DENIED;
	}

	#if USART_CALLBACK_MODE == true
	/* Check if the USART is busy doing asynchronous operation. */
	if (module->remaining_tx_buffer_length > 0) {
		return STATUS_BUSY;
	}

	#else
	/* Check if USART is ready for new data */
	if (!(usart_hw->INTFLAG.reg & SERCOM_USART_INTFLAG_DRE)) {
		/* Return error code */
		return STATUS_BUSY;
	}
	#endif

	/* Wait until synchronization is complete */
	_usart_wait_for_sync(module);

	/* Write data to USART module */
	usart_hw->DATA.reg = tx_data;

	//   while (!(usart_hw->INTFLAG.reg & SERCOM_USART_INTFLAG_TXC)) {
		/* Wait until data is sent */
	
	return STATUS_OK;
}

extern struct usart_module midi_SERCOM4, WT41_SERCOM5;

enum status_code usart_write_SERCOM(struct usart_module *const module, const uint8_t tx_data);
extern char pair_addr[20];
extern uint8_t sysState;

void check_usarts(void)
{	
	uint32_t length, lenMax=0;
	uint8_t tx_data;
	enum status_code status_code;	

	// write from the midi Q to the WT41 
	length = midi_to_bt_length();
	if (length > lenMax ) lenMax = length;
	if (length>0)	{
		tx_data = serialQ.midiToBlueTooth[serialQ.midi_out];
		status_code  = usart_write_SERCOM(&WT41_SERCOM5, tx_data); 
		if (status_code == STATUS_OK)
		{
			serialQ.midi_out++;			// write ok, inc ptr
		}
	}

	// write from the WT41  Q to midi 
	length = bt_to_midi_length();
	if (length>0)	{
		if (port_pin_get_input_level(WT41_CARRIER_DETECT_PIN) == true) { 
				// CARRIER  only relay when carrier is present, this should be midi data
				// mod 10/3/16
			tx_data = serialQ.blueToothToMidi[serialQ.bt_out];
			status_code  = usart_write_SERCOM(&midi_SERCOM4, tx_data); 
			if (status_code == STATUS_OK) {
				serialQ.bt_out++;			// write ok, inc ptr
			}
		}
		else  serialQ.bt_out++; // remove item from Q		// mod 10/3/16
	}
}

void write_BT(void)
{
	uint32_t length, lenMax=0;
	uint8_t tx_data;
	enum status_code status_code;

	length = midi_to_bt_length();
	if (length > lenMax ) lenMax = length;
	if (length>0)	{
		tx_data = serialQ.midiToBlueTooth[serialQ.midi_out];
		status_code  = usart_write_SERCOM(&WT41_SERCOM5, tx_data); //  mod12/31/15
		if (status_code == STATUS_OK)
		{
			serialQ.midi_out++;			// write ok, inc ptr
		}
	}

}
