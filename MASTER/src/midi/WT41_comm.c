#include <asf.h>
#include <conf_clocks.h>
#include <stdio.h>

#include "master.h"
#include "prototypes.h"

#define INIT_STATE	0xf0
#define CK_PAIR_STATE 0xf1
#define	PAIRED_STATE 0xf2
#define INQUIRY_STATE 0xf3
#define SSP_STATE 0xf4
#define PAIRING_STATE 0xf5
#define CALLING_STATE 0xf6
#define MAX_RX_BUFFER_LENGTH 1

#define SERIAL_COMM_QUITE 500				// 5 secs

//uint32_t get_msCounter(void);		  // added this to keep reset_BT from hanging up from optimization

// BLUETOOTH - WT41
void reset_BT(void);
enum status_code check_WT41_paired(struct usartQdef *usartQptr);
enum status_code check_WT41_responding(struct usartQdef *usartQptr);
enum status_code unpair_WT41(struct usartQdef *usartQptr);
void exit_SSP_WT41(struct usartQdef *usartQptr);
void load_iWrap_Cmd(struct usartQdef *usartQptr, uint8_t *str);
	
// 
void init_usart_Qs(struct usartQdef *usartQptr);  // sets Q to zero

_PTR	 _EXFUN(memmem, (const _PTR, size_t, const _PTR, size_t));
#include <string.h>
//#include "serial_buffers.h"
//#include "tc_stuff.h"
#include <tc.h>


//extern struct bufferDef serialQ;
extern char lastBT;
extern uint32_t volatile msCounter;
extern uint8_t sysState;


char pair_addr[20];


char cStr[100];


void load_iWrap_Cmd(struct usartQdef *usartQptr, uint8_t *str)
{

//	len = strlen((const char*)str);
//	for (i = 0; i < len; i++)
	{
		system_interrupt_enter_critical_section();
		put_tx_Q(usartQptr, str, strlen((char *)str));
		system_interrupt_leave_critical_section();
		str++;
	}
}


void exit_SSP_WT41(struct usartQdef *usartQptr)
{
	msCounter = 120;
	while(msCounter>0){
		do_FP_SPI();
		}   // wait 1.2 sec - don't run serial usarts - go quite
	init_usart_Qs(usartQptr);	
	load_iWrap_Cmd(usartQptr, (uint8_t*) "+++");
	msCounter = 120;			
	while(msCounter>0){
				do_FP_SPI();
		}   // wait 1.2 sec - don't run serial - go quite
}

enum status_code check_WT41_paired(struct usartQdef *usartQptr)
{// check if pair and get pair
	uint8_t *ptr;


	init_usart_Qs(usartQptr);
	load_iWrap_Cmd(usartQptr, (uint8_t*) "set bt pair\r\n");

	msCounter = 200;	// wait 2 sec
	while (msCounter > 0)	
	{
		do_FP_SPI();
		TX_usart_Qs();
		
		strcpy(cStr,"SET BT PAIR");
		ptr = memmem((char *)(usartQptr->rx),100, cStr,strlen(cStr));

		if(ptr) {
				wait_10msSec(50);				// wait for pair addr
				memcpy(pair_addr,ptr+12,17);
				pair_addr[17] = 0;

				return STATUS_OK;
			}

			else if (!msCounter)	// exit if timed out
			{
				pair_addr[0]=0;
				return STATUS_ERR_DENIED;
			}

	}
	return STATUS_ERR_DENIED;
}


enum status_code check_WT41_responding(struct usartQdef *usartQptr)
{
	uint8_t i;
	uint8_t *ptr;
	ptr = NULL;

	for (i=0; i< 3; i++)
	{
		msCounter = 200/3;
		init_usart_Qs(usartQptr);
		load_iWrap_Cmd(usartQptr, (uint8_t*) "at\r\nat\r\n");
		while (msCounter>0)
		{	
			do_FP_SPI();
			TX_usart_Qs();
			strcpy(cStr,"OK\r\n");			// check WT41 responding
			ptr = memmem((char *)(usartQptr->rx),100, cStr,strlen(cStr));
			if (ptr) break;
		}
		if(ptr) 
			return STATUS_OK;
		else 
		{
			reset_BT();							// not responding, try reset
			return STATUS_ERR_DENIED;
		}
	}
	return STATUS_ERR_DENIED;
}

enum status_code unpair_WT41(struct usartQdef *usartQptr)
{


	// exit SSP mode   (if in it)
	pair_addr[0] = 0;
	
	exit_SSP_WT41(usartQptr);

	check_WT41_paired(usartQptr);
	if(pair_addr[0] == 0)				// not paired - return
		return STATUS_OK;
	
	cStr[0]=0;							
	strcpy(cStr, "at\r\nset bt pair ");
	strcat(cStr,pair_addr);
	strcat(cStr,"\r\n");
	init_usart_Qs(usartQptr);
	load_iWrap_Cmd(usartQptr, (uint8_t*) cStr);	// send command to remove pairing
	
	wait_10msSec(100);
	
	check_WT41_paired(usartQptr);
	if(pair_addr[0] == 0)				// not paired - return
	{

		return STATUS_OK;
	}
	else 
	{
		reset_BT();
		return STATUS_ERR_DENIED; 	
	}
}



  // Reset BT module
void reset_BT(void)
{

 	port_pin_set_output_level(Reset_BT_PIN,0);
	
	msCounter=10;
	while (msCounter){
		do_FP_SPI();
	}
		
	port_pin_set_output_level(Reset_BT_PIN,1);
	
	msCounter=50;
	while (msCounter){
		do_FP_SPI();
	}
}
