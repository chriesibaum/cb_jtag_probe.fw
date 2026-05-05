#ifndef USB_JTAG_TRANSPORT_H_
#define USB_JTAG_TRANSPORT_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int usb_jtag_transport_process_frame(const uint8_t *rx_buf,
				     size_t rx_len,
				     uint8_t *tx_buf,
				     size_t tx_buf_len,
				     size_t *tx_len);

#ifdef __cplusplus
}
#endif

#endif /* USB_JTAG_TRANSPORT_H_ */
