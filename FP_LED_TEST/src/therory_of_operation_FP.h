/*
  "therory_of_operation_spi.h"
  
 file included for documentation
 
 CLOCKS
	System 8Mhz 

	tc_7	1ms (with 2nd 100ms running)
	
 
 SPI
	SERCOM4
	PB12	MOSI
	PB13	CS
	PB14	MISO
	PB15	SCK
	
	SPI recieves LED state & sends switch closures
		LED 
			61 LEDS
			codes
				0xfa			all off
				led #			on
				0x80+led#		off
				0xf7			no change
	
		SWITCH	(tx, each byte)
			73 switches
			codes
				0xfa			all open
				switch #		switch closed
				0x80+switch #	switch open
				0xf7			no change
				
					


*/