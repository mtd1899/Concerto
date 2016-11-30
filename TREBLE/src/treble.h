
// Treble.h


#include "tc.h"

struct treble_key_status {				// for key/note management
	bool on;
	uint8_t lastAtoDval;
	uint8_t maxAtoDval;
	uint8_t minAtoDval;
	bool velCalculating;
	uint8_t velCnt;
	uint8_t state;
//	uint8_t analogHigh;
//	uint8_t analogLow;
	uint8_t thresholdHigh;
	uint8_t thresholdLow;
	uint16_t count;
	struct tc_module *tcPtr;
	bool sent;
	uint8_t calcVelocity;
}; 

#define KEY_QUE_SIZE 64				// must be power of 2
struct keyQ_def {
	uint8_t key [KEY_QUE_SIZE];		// don't change - needs to roll over
	uint8_t buff_in;					// buffer pointers
	uint8_t buff_out;
	uint8_t length;
};

// NON VELOCITY MODE
#define NOTE_ON_OFFSET	50		// atod val key must travel to turn on in non-velocity mode
#define NOTE_OFF_THRESHOLD 10

//	VELOCITY MODE
#define KEY_OFF			0
#define KEY_STARTED		2
#define KEY_WAITING		3
#define KEY_FULL_ON		4
#define KEY_TIMED_OUT	5
#define KEY_TURNING_OFF	6
#define VELOCITY_MODE	0xf1



#define MAX_TC_AVAIL 7

#define TREBLE_SPI_TRANS_LENGTH 3

#define TREBLE_SPI_MODULE              SERCOM4
#define TREBLE_SPI_SERCOM_MUX_SETTING  SPI_SIGNAL_MUX_SETTING_E
#define TREBLE_SPI_SERCOM_PINMUX_PAD0  PINMUX_PB12C_SERCOM4_PAD0
#define TREBLE_SPI_SERCOM_PINMUX_PAD1  PINMUX_PB13C_SERCOM4_PAD1
#define TREBLE_SPI_SERCOM_PINMUX_PAD2  PINMUX_PB14C_SERCOM4_PAD2
#define TREBLE_SPI_SERCOM_PINMUX_PAD3  PINMUX_PB15C_SERCOM4_PAD3

// status led
#define LED_PIN   PIN_PA00
#define	PROCESSOR_LOC0_PIN	PIN_PA13
#define	PROCESSOR_LOC1_PIN	PIN_PA14
#define	PROCESSOR_LOC2_PIN	PIN_PA15

// Test point outputs
#define TP41_PIN PIN_PA20
#define TP42_PIN PIN_PA21
#define TP43_PIN PIN_PA22


