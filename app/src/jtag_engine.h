#ifndef JTAG_ENGINE_H_
#define JTAG_ENGINE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int jtag_engine_init(void);

int jtag_engine_shift_bits(const uint8_t *tdi,
			   const uint8_t *tms,
			   uint8_t *tdo,
			   uint32_t n_bits);

int jtag_engine_shift_loopback(const uint8_t *tdi,
				      const uint8_t *tms,
				      uint8_t *tdo,
				      uint32_t n_bits);

int jtag_engine_set_nsrst_high(void);
int jtag_engine_set_nsrst_low(void);

#ifdef __cplusplus
}
#endif

#endif /* JTAG_ENGINE_H_ */
