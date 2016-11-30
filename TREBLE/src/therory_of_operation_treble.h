/*
  "therory_of_operation_treble.h"
  
 file included for documentation
 
 CLOCKS
	System 8Mhz 
	ADC 8Mhz (generator 1)
 
 SPI
	SERCOM4
	PB12	MOSI
	PB13	CS
	PB14	MISO
	PB15	SCK
	
	note 3 treble uP per board
	polled in sequence
	communicates in one polled cycle and then waiting for 2 cycles as the other 2 uP communicate
	
	receives 3 bytes 
		byte 0 command		bit 7 set to signal a command byte	  0xf7 = switch closure 0xf1 is velocity
	
	transmits 
		byte 0 note #			bit 7 = 1 signals its a note   starts at 21
		byte 1 velocity
 		 or
		byte 0 = 0x80 signals no change
		no byte 1
		  
 ADC
	20 channels			(all available on uP)
	pin_scans 1st 10 then 2nd 10	(scans limit with ASF is 16 max on each scan )
	200 us total read time (measured with scope)
 
 TC
	8 timer counters, free running
	32225 cnts/sec or 31 us/cnt 
 
 KEY VELOCITY SENSING
	8 keys measured at same time (limited by 8 TC)
	keys_module[20] - using struct key holds the current state and operating conditions
	scanned and updated in round robin format
	
CALIBRATION



*/