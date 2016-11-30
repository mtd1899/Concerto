
#ifndef USART_INTERRUPT_H_INCLUDED
#define USART_INTERRUPT_H_INCLUDED

#include "usart.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__DOXYGEN__)
enum status_code _usart_write_buffer(
		struct usart_module *const module,
		uint8_t *tx_data,
		uint16_t length);

enum status_code _usart_read_buffer(
		struct usart_module *const module,
		uint8_t *rx_data,
		uint16_t length);

void _usart_interrupt_handler(
		uint8_t instance);
#endif

/**
 * \addtogroup asfdoc_sam0_sercom_usart_group
 *
 * @{
 */



/**
 * \name Writing and Reading
 * @{
 */
enum status_code usart_write_job(
		struct usart_module *const module,
		const uint16_t *tx_data);

enum status_code usart_read_job(
		struct usart_module *const module,
		uint16_t *const rx_data);

enum status_code usart_write_buffer_job(
		struct usart_module *const module,
		uint8_t *tx_data,
		uint16_t length);

enum status_code usart_read_buffer_job(
		struct usart_module *const module,
		uint8_t *rx_data,
		uint16_t length);

void usart_abort_job(
		struct usart_module *const module,
		enum usart_transceiver_type transceiver_type);

enum status_code usart_get_job_status(
		struct usart_module *const module,
		enum usart_transceiver_type transceiver_type);
/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* USART_INTERRUPT_H_INCLUDED */

