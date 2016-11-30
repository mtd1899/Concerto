// hardware defs

#define Reset_BT_PIN PIN_PA16
#define Green_LED_PIN PIN_PA07
#define Yellow_LED_PIN PIN_PA06
#define Red_LED_PIN PIN_PA05
#define UNPAIR_PUSH_BUTTON PIN_PA04

#define	WT41_CARRIER_DETECT_PIN PIN_PA23



#define INIT_STATE	0xf0
#define CK_PAIR_STATE 0xf1
#define	PAIRED_STATE 0xf2
#define INQUIRY_STATE 0xf3
#define SSP_STATE 0xf4
#define PAIRING_STATE 0xf5
#define CALLING_STATE 0xf6
#define MAX_RX_BUFFER_LENGTH 1

#define BT_NAME_SLAVE		"AISLVV02"
#define BT_NAME_MASTER		"AIMASV02"
#define SERIAL_COMM_QUITE  500				// 5 secs

#define WT41_AUTOCALL "\r\nset control autocall 1101 5000 rfcomm\r\n"
#define	WT41_SUPERVISORY_TIMEOUT "\r\nset bt role 0 F 1000\r\n"
#define	WT41_CARRIER_DETECT_CMD	"\r\nset control cd 20 2 80\r\n"

