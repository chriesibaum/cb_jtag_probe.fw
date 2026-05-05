#include "jtag_proto.h"

#include <string.h>

#define JTAG_PROTO_RESERVED_MASK 0x0000u

static uint16_t le16_read(const uint8_t *p)
{
	return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t le32_read(const uint8_t *p)
{
	return (uint32_t)p[0] |
	       ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

static void le16_write(uint8_t *p, uint16_t v)
{
	p[0] = (uint8_t)(v & 0xFFu);
	p[1] = (uint8_t)((v >> 8) & 0xFFu);
}

static void le32_write(uint8_t *p, uint32_t v)
{
	p[0] = (uint8_t)(v & 0xFFu);
	p[1] = (uint8_t)((v >> 8) & 0xFFu);
	p[2] = (uint8_t)((v >> 16) & 0xFFu);
	p[3] = (uint8_t)((v >> 24) & 0xFFu);
}

size_t jtag_bytes_for_bits(uint32_t n_bits)
{
	return (size_t)((n_bits + 7u) / 8u);
}

int jtag_proto_decode_scan_request(const uint8_t *buf, size_t len,
					 struct jtag_scan_request *req)
{
	size_t n_bytes;
	uint16_t reserved;
	uint8_t cmd;

	if ((buf == NULL) || (req == NULL) || (len < JTAG_PROTO_HEADER_SIZE)) {
		return -1;
	}

	cmd = buf[0];
	if (cmd != JTAG_CMD_SCAN) {
		return -2;
	}

	reserved = le16_read(&buf[2]);
	if (reserved != JTAG_PROTO_RESERVED_MASK) {
		return -3;
	}

	req->flags = buf[1];
	req->n_bits = le32_read(&buf[4]);
	n_bytes = jtag_bytes_for_bits(req->n_bits);

	if (req->n_bits == 0u) {
		return -4;
	}

	if (len != (JTAG_PROTO_HEADER_SIZE + (2u * n_bytes))) {
		return -5;
	}

	req->n_bytes = n_bytes;
	req->tdi = &buf[JTAG_PROTO_HEADER_SIZE];
	req->tms = &buf[JTAG_PROTO_HEADER_SIZE + n_bytes];

	return 0;
}

int jtag_proto_encode_scan_response(const struct jtag_scan_response *rsp,
					 uint8_t *buf,
					 size_t buf_len,
					 size_t *encoded_len)
{
	size_t needed;

	if ((rsp == NULL) || (buf == NULL) || (encoded_len == NULL)) {
		return -1;
	}

	needed = JTAG_PROTO_HEADER_SIZE + rsp->n_bytes;
	if (buf_len < needed) {
		return -2;
	}

	buf[0] = rsp->status;
	buf[1] = rsp->flags;
	le16_write(&buf[2], 0u);
	le32_write(&buf[4], rsp->n_bits);

	if ((rsp->n_bytes > 0u) && (rsp->tdo != NULL)) {
		memcpy(&buf[JTAG_PROTO_HEADER_SIZE], rsp->tdo, rsp->n_bytes);
	}

	*encoded_len = needed;
	return 0;
}
