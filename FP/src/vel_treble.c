// TREBLE VELOCITY AND SWTICH CLOSER

#include <asf.h>
#include "treble.h"

struct keyQ_def keyQ;
uint8_t get_keyQ(void);
enum status_code put_keyQ (uint8_t *ptr);



/*

struct txQ_def{
	uint8_t val[32];
	uint8_t velocity [KEY_QUE_SIZE];
	uint8_t buff_in;					// buffer pointers
	uint8_t buff_out;


NEED TO PULL FROM THE keyQ and load txQ ???????????????  could pull tx directly from keyQ????
	
	
	};*/
extern struct treble_key_status trebKeys[20];
extern uint16_t adc_result_buffer[20];
extern uint8_t noteOffset;

void key_closer(uint8_t startKey, uint8_t stopKey) // operating in switch closure mode
{		uint8_t key[2],i;

	for (i=startKey; i<stopKey; i++)
	{
		// continuously update max and mins
		if (adc_result_buffer[i]>trebKeys[i].maxAtoDval) 
			trebKeys[i].maxAtoDval = adc_result_buffer[i];
		else if (adc_result_buffer[i]<trebKeys[i].minAtoDval) 
			trebKeys[i].minAtoDval = adc_result_buffer[i];

		if (!trebKeys[i].on)
		{	// trebKey is off, ck if should be on
			if(adc_result_buffer[i] < trebKeys[i].maxAtoDval - NOTE_ON_OFFSET)
			{
				// turn it on
				trebKeys[i].on = true;
				key[0] = (i + noteOffset)|0x80;
				key[1] = 0x7f;		// max velocity
//				key[1] = adc_result_buffer[i];		// TEST ONLY
				put_keyQ(key);				
			}
		}
		else						
		{	// if its on, ck if should be off
			if(adc_result_buffer[i] > trebKeys[i].maxAtoDval - NOTE_OFF_THRESHOLD)
			{
				// turn it off
				trebKeys[i].on = false;
				key[0] = (i + noteOffset)|0x80;
				key[1] = 01;		// 0ff
				put_keyQ(key);

				port_pin_set_output_level(TP41_PIN,1);
				
			}			
		}
	}
}



uint8_t get_keyQ(void)
{	uint8_t	val;
	val = keyQ.key[keyQ.buff_out];
	keyQ.buff_out++;
	keyQ.buff_out = keyQ.buff_out & (KEY_QUE_SIZE-1);
	return(val);
}

enum status_code put_keyQ (uint8_t *ptr)
{
	
	port_pin_set_output_level(TP41_PIN,0);
	
	system_interrupt_enter_critical_section();  // critical because the keyQ can be changed in a interrupt callback
	if (((KEY_QUE_SIZE-1) & (keyQ.buff_in + 1)) != keyQ.buff_out){
		keyQ.key[keyQ.buff_in] = *ptr;
		keyQ.buff_in++;
		keyQ.buff_in = keyQ.buff_in & (KEY_QUE_SIZE-1);
		keyQ.key[keyQ.buff_in] = *(ptr+1);
		keyQ.buff_in++;
		keyQ.buff_in = keyQ.buff_in & (KEY_QUE_SIZE-1);			// wrap at KEY_QUE_SIZE
		system_interrupt_leave_critical_section();
	}
	else
	{
		system_interrupt_leave_critical_section();
		return STATUS_ERR_DENIED;
	}
	return STATUS_OK;
}



void key_velocity (uint8_t startKey, uint8_t stopKey) // operating in key velocity mode
{
	
}
/*
	//	process scan of 2nd group of keys

		for (i=10; i<20; i++)
		{
			switch (keys[i].state)
			{
				case KEY_OFF:
					if (keys[i].analogHigh < keys[i].thresholdHigh)	 // true if turning on
  					{
						// find open timer
						
						
						tc_start_counter
						
						
						keys[i].state = KEY_STARTED;
						
					}
				
				break;

				case KEY_STARTED:
				// code
				break;

				case KEY_WAITING:
				// code
				break;

				case KEY_FULL_ON:
				// code
				break;


				case KEY_TIMED_OUT:
				// code
				break;


				case KEY_TURNING_OFF:
				// code
				break;
				
				default:
				// code
				break;
			} 


*/
