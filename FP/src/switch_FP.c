// switch_FP.c				handles FP switch pads


#include <asf.h>
#include "therory_of_operation_fp.h"
#include "fp.h"
#include <port.h>

void readRow(uint8_t swState);
void Do_Switches(void);
void put_FP_swQ (uint8_t val);

uint8_t swDebounce[81];
uint8_t swState;

void Do_Switches(void)
{
	readRow(swState);
	swState++;
	if (swState>8)swState=0;		
	switch(swState)
	{
		case 0:
			port_pin_set_output_level(FP_SWO8, 0);		// off
			port_pin_set_output_level(FP_SWO0, 1);		// on
			break;
		case 1:
			port_pin_set_output_level(FP_SWO0, 0);
			port_pin_set_output_level(FP_SWO1, 1);
			break;
		case 2:
			port_pin_set_output_level(FP_SWO1, 0);
			port_pin_set_output_level(FP_SWO2, 1);
			break;
		case 3:
			port_pin_set_output_level(FP_SWO2, 0);
			port_pin_set_output_level(FP_SWO3, 1);
			break;
		case 4:
			port_pin_set_output_level(FP_SWO3, 0);
			port_pin_set_output_level(FP_SWO4, 1);
			break;
		case 5:
			port_pin_set_output_level(FP_SWO4, 0);
			port_pin_set_output_level(FP_SWO5, 1);
			break;
		case 6:
			port_pin_set_output_level(FP_SWO5, 0);
			port_pin_set_output_level(FP_SWO6, 1);
			break;
		case 7:
			port_pin_set_output_level(FP_SWO6, 0);
			port_pin_set_output_level(FP_SWO7, 1);
			break;
		case 8:
			port_pin_set_output_level(FP_SWO7, 0);
			port_pin_set_output_level(FP_SWO8, 1);
			break;
	}
}

static inline void inc_switch(uint8_t FPswitch)
{
	switch (swDebounce[FPswitch])
	{	case 10:
			put_FP_swQ(FPswitch+1);			// turn on
			swDebounce[FPswitch]++;
			break;
		case 11:							// counts up to 11
			break;
		default:
			swDebounce[FPswitch]++;
	}
}

static inline void dec_switch(uint8_t FPswitch)
{
	switch (swDebounce[FPswitch])
	{	case 1:
			put_FP_swQ(FP_SW_OPEN_FLAG + FPswitch+1);		// turn off
			swDebounce[FPswitch]--;
			break;
		case 0:
			break;							 // stops dec at 0
		default:
			swDebounce[FPswitch]--;
	}
}


void readRow(uint8_t swState1)
{


			if (port_pin_get_input_level(FP_SWI0) == 0)
				inc_switch(swState1*8);
			else
				dec_switch(swState1*8);
							
			if (port_pin_get_input_level(FP_SWI1) == 0)
				inc_switch(swState1*8+1);
			else
				dec_switch(swState1*8+1);
				
			if (port_pin_get_input_level(FP_SWI2) == 0)
				inc_switch(swState1*8+2);
			else
				dec_switch(swState1*8+2);
				
			if (port_pin_get_input_level(FP_SWI3) == 0)
				inc_switch(swState1*8+3);
			else
				dec_switch(swState1*8+3);
				
			if (port_pin_get_input_level(FP_SWI4) == 0)
				inc_switch(swState1*8+4);
			else
				dec_switch(swState1*8+4);
				
			if (port_pin_get_input_level(FP_SWI5) == 0)
				inc_switch(swState1*8+5);
			else
				dec_switch(swState1*8+5);
				
			if (port_pin_get_input_level(FP_SWI6) == 0)
				inc_switch(swState1*8+6);
			else
				dec_switch(swState1*8+6);
				
			if (port_pin_get_input_level(FP_SWI7) == 0)
				inc_switch(swState1*8+7);
			else
				dec_switch(swState1*8+7);
				
			if (swState1==8)
			{
				if (port_pin_get_input_level(FP_SWI8) == 0)
					inc_switch(72);
				else
					dec_switch(72);
			}			
}