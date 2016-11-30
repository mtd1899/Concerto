/* serial_buffers. h

	defines structures
	*/

#define  TBUF_SIZE  256					/* size of serial receiving buffer      */
#define  RBUF_SIZE  256                     /* size of serial transmission buffer   */
#define  STATUS_BUFFER_EMPTY 255


struct bufferDef {
	uint8_t midiToBlueTooth [256];  // don't change - needs to roll over
	uint8_t blueToothToMidi [256];
	
	uint8_t bt_in;					// buffer pointers
	uint8_t bt_out;
	uint8_t midi_in;
	uint8_t midi_out;
	uint8_t last_mTob;
	uint8_t last_bTom;
};
	
struct bufferDef *init_serial_buffers(void);
enum status_code bt_to_midi_loadQ (uint8_t data);
enum status_code midi_to_bt_loadQ (uint8_t data);
uint8_t bt_to_midi_length (void);
uint8_t midi_to_bt_length (void);
enum status_code usart_write( struct usart_module *const module, const uint16_t tx_data);
void load_iWrap_Cmd( char *str);
void check_usarts(void);
void configure_usart_SERCOM3(void);
void configure_usart_SERCOM4(void);
void configure_usart_SERCOM5(void);
enum status_code inquiry_WT41(void);
enum status_code check_WT41_paired(void);
enum status_code check_WT41_responding(void);
enum status_code pair_WT41(void);
enum status_code unpair_WT41(void);
enum status_code calling_WT41(void);





