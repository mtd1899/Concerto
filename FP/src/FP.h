// FP.h


#include "tc.h"



#define FP_QUE_SIZE 16		  // must be power of 2

struct FPrxQ_def {
	uint8_t buff_in;					// buffer pointers
	uint8_t buff_out;
	uint8_t length;
	uint8_t val [FP_QUE_SIZE];		// don't change - needs to roll over
};

struct FPtxQ_def {
	uint8_t buff_in;					// buffer pointers
	uint8_t buff_out;
	uint8_t length;
	uint8_t val [FP_QUE_SIZE];		// don't change - needs to roll over
};



#define FP_SPI_MODULE              SERCOM4
#define FP_SPI_SERCOM_MUX_SETTING  SPI_SIGNAL_MUX_SETTING_E
#define FP_SPI_SERCOM_PINMUX_PAD0  PINMUX_PB12C_SERCOM4_PAD0
#define FP_SPI_SERCOM_PINMUX_PAD1  PINMUX_PB13C_SERCOM4_PAD1
#define FP_SPI_SERCOM_PINMUX_PAD2  PINMUX_PB14C_SERCOM4_PAD2
#define FP_SPI_SERCOM_PINMUX_PAD3  PINMUX_PB15C_SERCOM4_PAD3

// status led
#define LED_RUN_PIN		PIN_PA19

// RESET FOR LED DRIVERS
#define	LED_CHIP_RST	PIN_PA20

#define LED_1_24_ADDRESS 2		   //bits 1-7 are addr bits --- bit 0 is a R/WR bit with 0 WR
#define LED_25_48_ADDRESS 0X0a
#define LED_49_61_ADDRESS 0x2a


#define LED_BRIGHTNESS 99

#define FP_LED_ALL_OFF	0xfa
#define FP_LED_OFF_FLAG	0x80
#define FP_LED_OFF_FLAG_MASK	0x7f
#define FP_LED_NO_CHANGE 0xf7


#define FP_SW_ALL_OFF	0xfa
#define FP_SW_OPEN_FLAG	0x80
#define FP_SW_NO_CHANGE 0xf7

#define FP_SWO0	PIN_PA00
#define FP_SWO1	PIN_PA01
#define FP_SWO2	PIN_PA02
#define FP_SWO3	PIN_PA03
#define FP_SWO4	PIN_PA04
#define FP_SWO5	PIN_PA05
#define FP_SWO6	PIN_PA06
#define FP_SWO7	PIN_PA07
#define FP_SWO8	PIN_PA08

#define FP_SWI0	PIN_PA09
#define FP_SWI1	PIN_PA10
#define FP_SWI2	PIN_PA11
#define FP_SWI3	PIN_PA12
#define FP_SWI4	PIN_PA13
#define FP_SWI5	PIN_PA14	
#define FP_SWI6	PIN_PA15	
#define FP_SWI7	PIN_PA16
#define FP_SWI8	PIN_PA17

enum command_reg {
	MODE1,          /**< MODE1 register      */
	MODE2,          /**< MODE2 register      */
	LEDOUT0,        /**< LEDOUT0 register    */
	LEDOUT1,        /**< LEDOUT1 register    */
	LEDOUT2,        /**< LEDOUT2 register    */
	LEDOUT3,        /**< LEDOUT3 register    */
	LEDOUT4,        /**< LEDOUT4 register    */
	LEDOUT5,        /**< LEDOUT5 register    */
	GRPPWM,         /**< GRPPWM register     */
	GRPFREQ,        /**< GRPFREQ register    */
	PWM0,           /**< PWM0 register       */
	PWM1,           /**< PWM1 register       */
	PWM2,           /**< PWM2 register       */
	PWM3,           /**< PWM3 register       */
	PWM4,           /**< PWM4 register       */
	PWM5,           /**< PWM5 register       */
	PWM6,           /**< PWM6 register       */
	PWM7,           /**< PWM7 register       */
	PWM8,           /**< PWM8 register       */
	PWM9,           /**< PWM9 register       */
	PWM10,          /**< PWM10 register      */
	PWM11,          /**< PWM11 register      */
	PWM12,          /**< PWM12 register      */
	PWM13,          /**< PWM13 register      */
	PWM14,          /**< PWM14 register      */
	PWM15,          /**< PWM15 register      */
	PWM16,          /**< PWM16 register      */
	PWM17,          /**< PWM17 register      */
	PWM18,          /**< PWM18 register      */
	PWM19,          /**< PWM19 register      */
	PWM20,          /**< PWM20 register      */
	PWM21,          /**< PWM21 register      */
	PWM22,          /**< PWM22 register      */
	PWM23,          /**< PWM23 register      */
	IREF0,          /**< IREF0 register      */
	IREF1,          /**< IREF1 register      */
	IREF2,          /**< IREF2 register      */
	IREF3,          /**< IREF3 register      */
	IREF4,          /**< IREF4 register      */
	IREF5,          /**< IREF5 register      */
	IREF6,          /**< IREF6 register      */
	IREF7,          /**< IREF7 register      */
	IREF8,          /**< IREF8 register      */
	IREF9,          /**< IREF9 register      */
	IREF10,         /**< IREF10 register     */
	IREF11,         /**< IREF11 register     */
	IREF12,         /**< IREF12 register     */
	IREF13,         /**< IREF13 register     */
	IREF14,         /**< IREF14 register     */
	IREF15,         /**< IREF15 register     */
	IREF16,         /**< IREF16 register     */
	IREF17,         /**< IREF17 register     */
	IREF18,         /**< IREF18 register     */
	IREF19,         /**< IREF19 register     */
	IREF20,         /**< IREF20 register     */
	IREF21,         /**< IREF21 register     */
	IREF22,         /**< IREF22 register     */
	IREF23,         /**< IREF23 register     */
	OFFSET  = 0x3A, /**< OFFSET register     */
	SUBADR1,        /**< SUBADR1 register    */
	SUBADR2,        /**< SUBADR2 register    */
	SUBADR3,        /**< SUBADR3 register    */
	ALLCALLADR,     /**< ALLCALLADR register */
	PWMALL,         /**< PWMALL register     */
	IREFALL,        /**< IREFALL register    */
	EFLAG0,         /**< EFLAG0 register     */
	EFLAG1,         /**< EFLAG1 register     */
	EFLAG2,         /**< EFLAG2 register     */
	EFLAG3,         /**< EFLAG3 register     */
	EFLAG4,         /**< EFLAG4 register     */
	EFLAG5,         /**< EFLAG5 register     */

	REGISTER_START          = MODE1,
	LEDOUT_REGISTER_START   = LEDOUT0,
	PWM_REGISTER_START      = PWM0,
	IREF_REGISTER_START     = IREF0,
};
