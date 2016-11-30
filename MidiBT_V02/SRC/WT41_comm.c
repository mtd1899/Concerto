#include <asf.h>
#include <conf_clocks.h>
#include <stdio.h>
#include "MidiBT_pcb.h"


void wait_sec(uint32_t wait);
void wait_10msSec(uint32_t wait);
void reset_BT(void);
void write_BT(void);
void BT_master_init(void);
void check_unpairPB(void);
void exit_SSP_WT41(void);

_PTR	 _EXFUN(memmem, (const _PTR, size_t, const _PTR, size_t));
#include <string.h>
#include "serial_buffers.h"
#include "tc_stuff.h"
#include <tc.h>

#define WT41_AUTOCALL "\r\nset control autocall 1101 5000 rfcomm\r\n"
#define	WT41_SUPERVISORY_TIMEOUT "\r\nset bt role 0 F 1000\r\n"
#define	WT41_CARRIER_DETECT_CMD	"\r\nset control cd 20 2 80\r\n"




extern struct bufferDef serialQ;
extern char lastBT;
extern uint32_t msCounter;
extern uint8_t sysState;


char pair_addr[20];
char cStr[100];


extern bool BTpaired;


void exit_SSP_WT41(void)
{
	msCounter = 120;
	while(msCounter>0){}   // wait 1.2 sec - don't run serial - go quite
	init_serial_buffers();
	load_iWrap_Cmd((char*)"+++");
	while(msCounter>0){}   // wait 1.2 sec - don't run serial - go quite
}



// sends a bt inquiry looking for a bt module
enum status_code inquiry_WT41(void)
{
	uint8_t *ptrName, *ptrBtName;

/*														MOD 8/18
	port_pin_set_output_level(Green_LED_PIN,1);	
	port_pin_set_output_level(Yellow_LED_PIN,1);
	port_pin_set_output_level(Red_LED_PIN,0);	*/

	init_serial_buffers();
	load_iWrap_Cmd((char*) "at\r\ninquiry 5 name\r\n"); // 5 sec to inquiry if other bt is out there

	msCounter = 1000;	// wait 10 sec

	while (msCounter)
	{
		check_usarts();

		ptrBtName = ptrName = 0;
		strcpy(cStr,BT_NAME_SLAVE);
		ptrBtName = memmem((char *)(serialQ.blueToothToMidi),255, cStr,5);		// look at only 1st 6 char - not software version

//		ptrName = memmem((char *)(serialQ.blueToothToMidi),255, "NAME",4);

uint32_t l;
		l = ptrBtName-serialQ.blueToothToMidi;

		if (ptrBtName && (ptrBtName-19>serialQ.blueToothToMidi)) 
		{	memcpy(pair_addr,ptrBtName-19,17);
			pair_addr[17] = 0;
			return STATUS_OK; // cStr found and pair_addr documented
		}

	}
	return STATUS_ERR_DENIED;
}



enum status_code check_WT41_paired(void)
{// check if pair and get pair
	uint8_t *ptr;


	init_serial_buffers();
	load_iWrap_Cmd((char*)"set bt pair\r\n");

	msCounter = 200;	// wait 2 sec
	while (msCounter)	
	{
		check_usarts();
		
		strcpy(cStr,"SET BT PAIR");
		ptr = memmem((char *)(serialQ.blueToothToMidi),100, cStr,strlen(cStr));

		if(ptr) {
				wait_10msSec(50);				// wait for pair addr
				memcpy(pair_addr,ptr+12,17);
				pair_addr[17] = 0;
				BTpaired = true;

				return STATUS_OK;
			}

			else if (!msCounter)	// exit if timed out
			{
				pair_addr[0]=0;
				BTpaired = false;
				return STATUS_ERR_DENIED;
			}

	}
	BTpaired = false;
	return STATUS_ERR_DENIED;
}


// tests to see is the bt module is responding to ascii commands
enum status_code check_WT41_responding(void)
{
	uint8_t i;
	uint8_t *ptr;
	ptr = NULL;

	for (i=0; i< 3; i++)
	{
		msCounter = 200/3;
		init_serial_buffers();
		load_iWrap_Cmd( (char*)"at\r\nat\r\n");
		while (msCounter)
		{
			check_usarts();
			strcpy(cStr,"OK\r\n");			// check WT41 responding
			ptr = memmem((char *)(serialQ.blueToothToMidi),100, cStr,strlen(cStr));
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


/*	pair_WT41
	send pair request - answered with ok
		example -> PAIR 00:07:80:a1:7d:9a				request
				-> PAIR 00:07:80:a1:7d:9a OK			reply

requirements:  address to pair with is in pair_addr

*/

enum status_code pair_WT41(void)
{
	uint8_t *ptr;

	init_serial_buffers();
	
	msCounter = 1500;		// set time out at 15 sec
	
	strcpy(cStr, "\r\npair ");
	strcat(cStr,pair_addr);
	strcat(cStr,"\r\n");
	load_iWrap_Cmd((char*) cStr);	   // send request
	
	while (msCounter)				// wait for OK
	{
		ptr = memmem((char *)(serialQ.blueToothToMidi),100,"OK",2);
		if (ptr) break;
		check_usarts();		
	}
	// Look for OK		
	if( ptr ) {
			BTpaired = true;
			return STATUS_OK;
		}
		else {
			unpair_WT41();
			return STATUS_ERR_DENIED;
		}
}


enum status_code unpair_WT41(void)
{
	if (port_pin_get_input_level(WT41_CARRIER_DETECT_PIN) == true) exit_SSP_WT41();
	
	// turn off autocall
	strcpy((char *)cStr,"at\r\nSET CONTROL AUTOCALL\r\n");		// removes it
	init_serial_buffers();
	load_iWrap_Cmd((char*) cStr);	// send command to remove autocall
	wait_10msSec(10);	// wait .1 sec above to be sent	
	
	check_WT41_paired();
	if(pair_addr[0] == 0)				// not paired - return
		return STATUS_OK;
	
	cStr[0]=0;							
	strcpy(cStr, "at\r\nset bt pair ");
	strcat(cStr,pair_addr);
	strcat(cStr,"\r\n");
	init_serial_buffers();
	load_iWrap_Cmd((char*) cStr);	// send command to remove pairing
	
	wait_sec(1);
	
	check_WT41_paired();
	if(pair_addr[0] == 0)				// not paired - return
	{
		sysState = INQUIRY_STATE;
		return STATUS_OK;
	}
	else 
	{
		reset_BT();
		return STATUS_ERR_DENIED; 	
	}
}


void BT_master_init(void)
{	uint8_t *ptr;

// check if it's been initialized once

//	init_serial_buffers();
	init_serial_buffers();
	load_iWrap_Cmd((char*) "at\r\nset\r\n");

	if (port_pin_get_input_level(WT41_CARRIER_DETECT_PIN) != true) check_unpairPB();
	
	msCounter = 1000;
	
	while (msCounter)				// wait for OK
	{
		check_usarts();
		strcpy(cStr,BT_NAME_MASTER);
		ptr = memmem((char *)(serialQ.blueToothToMidi),255, cStr,strlen(cStr));
		if (ptr) break;
	}

	if(ptr == NULL)   // initialize WT41
	{
		load_iWrap_Cmd((char*)WT41_SUPERVISORY_TIMEOUT);
		wait_10msSec(5);
		load_iWrap_Cmd((char*)WT41_CARRIER_DETECT_CMD);
		wait_10msSec(5);
/*
		load_iWrap_Cmd((char*)WT41_AUTOCALL);
		wait_10msSec(5);
*/
		strcpy(cStr,"at\r\nat\r\nset bt name ");		// rename
		strcat(cStr,BT_NAME_MASTER);
		strcat(cStr,"\r\n");
		load_iWrap_Cmd((char*)cStr);
		wait_10msSec(25);
	}
}


