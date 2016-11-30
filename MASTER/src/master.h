/*
	overall theory of operation

	2 BT modules - one for midi data, one for tablet data
			DEBUG_MIDI_TO_TABLET compile switch: is used to send ascii midi data to the tablet BT - originally used for debugging
				with the BlueGiga BT board as the receiver
				

			
		
*/


#define DEBUG_MIDI_TO_TABLET


// MASTER PCB

#define RUN_LED PIN_PA00			 // test
#define LOOP_IND PIN_PA08			 // test
#define Reset_BT_PIN PIN_PA20
#define WT41_TABLET_CARRIER_DETECT_PIN PIN_PA23
#define	WT41_MIDI_CARRIER_DETECT_PIN PIN_PA22
#define MASTERtoFP_SPI_CS_PIN PIN_PA17


#define BT_NAME_MIDI_SLAVE		"AISLV02"
#define BT_NAME_TABLET_SLAVE	"AITBV02"
#define	WT41_CARRIER_DETECT_CMD	"\r\nset control cd 20 2 80\r\n"

// FP (FRONT PANEL) STUFF

#define MASTERtoFP_SPI_MODULE              SERCOM1
#define MASTERtoFP_SPI_SERCOM_MUX_SETTING  SPI_SIGNAL_MUX_SETTING_E
#define MASTERtoFP_SPI_SERCOM_PINMUX_PAD0  PINMUX_PA16C_SERCOM1_PAD0
#define MASTERtoFP_SPI_SERCOM_PINMUX_PAD1  PINMUX_PA17C_SERCOM1_PAD1
#define MASTERtoFP_SPI_SERCOM_PINMUX_PAD2  PINMUX_PA18C_SERCOM1_PAD2
#define MASTERtoFP_SPI_SERCOM_PINMUX_PAD3  PINMUX_PA19C_SERCOM1_PAD3


// ********************* TREBLE STUFF

#define  VELOCITY_MODE_COMMAND 0xf1
#define  SWITCH_CLOSE_MODE_COMMAND 0xf7


#define TREBLE_CS0			PIN_PA05
#define TREBLE_CS1			PIN_PB08
#define TREBLE_CS2			PIN_PB09

#define MASTERtoTREBLE_SPI_MODULE              SERCOM0
#define MASTERtoTREBLE_SPI_SERCOM_MUX_SETTING  SPI_SIGNAL_MUX_SETTING_E
#define MASTERtoTREBLE_SPI_SERCOM_PINMUX_PAD0  PINMUX_PA04D_SERCOM0_PAD0
#define MASTERtoTREBLE_SPI_SERCOM_PINMUX_PAD1  PINMUX_PA05D_SERCOM0_PAD1
#define MASTERtoTREBLE_SPI_SERCOM_PINMUX_PAD2  PINMUX_PA06D_SERCOM0_PAD2
#define MASTERtoTREBLE_SPI_SERCOM_PINMUX_PAD3  PINMUX_PA07D_SERCOM0_PAD3


/*
#define NOTE_ON		0Xb0			// on channel 0
#define NOTE_OFF	0X80*/

#define KEY_QUE_SIZE 32		  // must be power of 2
#define REC_QUE_SIZE 32		  // must be power of 2
#define  MIDI_Q_SIZE 256	  // don't change - needs to roll over

struct keyQ_def {
	uint8_t volatile buff_in;					// buffer pointers
	uint8_t volatile buff_out;
	uint8_t volatile length;
	uint8_t volatile key [KEY_QUE_SIZE];  // don't change - needs to roll over
	uint8_t volatile vel [KEY_QUE_SIZE];  // don't change - needs to roll over
};

struct trebleRxQ_def {
	uint8_t volatile buff_in;					// buffer pointers
	uint8_t volatile buff_out;
	uint8_t volatile length;
	uint8_t volatile val [REC_QUE_SIZE];		// don't change - needs to roll over
};

struct usartQdef {
	uint8_t volatile tx_in;					// buffer pointers
	uint8_t volatile tx_out;
	uint8_t volatile rx_in;
	uint8_t volatile rx_out;
	uint8_t volatile rx [MIDI_Q_SIZE];  // don't change - needs to roll over
	uint8_t volatile tx [MIDI_Q_SIZE];
};



