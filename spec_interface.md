# CB JTAG Probe USB Interface Specification

## 1. Scope

This document specifies the device-side USB interface and frame protocol implemented by the CB JTAG Probe firmware.

It covers:
- USB interface descriptors and endpoints
- Bulk transport framing and constraints
- Command set and response semantics
- Error handling and host expectations

Source of truth in firmware:
- app/src/usb_bulk_jtag.c
- app/src/usb_jtag_transport.c
- app/src/jtag_proto.h
- app/src/jtag_proto.c

## 2. USB Device Interface

### 2.1 Device identity

- Vendor ID: 0x2FE3
- Product ID: 0x0001
- Manufacturer string: Chriesibaum GmbH
- Product string: Chriesibaum's JTAG Probe

### 2.2 Interface class

Single vendor-specific interface:
- bInterfaceClass: 0xFF (vendor specific)
- bInterfaceSubClass: 0x00
- bInterfaceProtocol: 0x00
- bNumEndpoints: 2

### 2.3 Endpoints

- Bulk OUT: endpoint address 0x01
- Bulk IN: endpoint address 0x81

Max packet size:
- Full speed: 64 bytes
- High speed: 512 bytes

The protocol is message framed at application level, so a logical frame may span multiple USB transactions.

## 3. Transport Model

### 3.1 Request/response pattern

Host sends exactly one request frame on Bulk OUT.
Device returns exactly one response frame on Bulk IN.

No asynchronous device-originated frames are defined.

### 3.2 Framing and reassembly

The device accumulates OUT data until the full request frame is received:
- If less than header size (8 bytes): wait for more data
- If frame length is invalid or exceeds limits: respond with BAD_LEN status
- If frame exactly complete: process command and respond

### 3.3 Maximum payload

Firmware imposes:
- Maximum scan size: 4096 bits
- Max request size: 8 + 2 * (4096 / 8) = 1032 bytes
- Max response size: 8 + (4096 / 8) = 520 bytes

## 4. Frame Format

All multi-byte integers are little-endian.

### 4.1 Common header (8 bytes)

Offset | Size | Field
--- | --- | ---
0 | 1 | cmd (request) / status (response)
1 | 1 | flags
2 | 2 | reserved (must be 0)
4 | 4 | n_bits

Constants:
- Header size: 8 bytes

### 4.2 Scan request payload

For cmd = SCAN (0x01), request body is:
- TDI bytes: ceil(n_bits/8)
- TMS bytes: ceil(n_bits/8)

Total request length for SCAN:
- 8 + 2 * ceil(n_bits/8)

Constraints for SCAN:
- n_bits must be > 0
- reserved must be 0

### 4.3 Response payload

Response body contains TDO data bytes when applicable.

Total response length:
- 8 + n_bytes
where n_bytes is command dependent (0 for control commands, ceil(n_bits/8) for SCAN).

## 5. Commands

### 5.1 Command codes

- 0x01: JTAG_CMD_SCAN
- 0x02: JTAG_CMD_NSRST_HIGH
- 0x03: JTAG_CMD_NSRST_LOW
- 0x04: JTAG_CMD_GET_FW_VERSION
- 0x05: JTAG_CMD_GET_DEVICE_ID

### 5.2 SCAN (0x01)

Request:
- Header cmd = 0x01
- flags: echoed back in response
- reserved = 0
- n_bits > 0
- payload: TDI then TMS, each ceil(n_bits/8)

Action:
- Shift n_bits through JTAG engine using provided TDI/TMS vectors

Response on success:
- status = OK
- flags = request flags
- n_bits = request n_bits
- payload = TDO bytes (ceil(n_bits/8))

Response on failure:
- status = BAD_LEN for format/length issues
- status = INTERNAL_ERR for JTAG engine/processing failure

### 5.3 NSRST_HIGH (0x02)

Request:
- Header-only frame (8 bytes)
- flags ignored
- reserved must be 0
- n_bits must be 0

Action:
- Drive system reset pin high

Response:
- status = OK on success
- status = INTERNAL_ERR on driver failure
- no payload

### 5.4 NSRST_LOW (0x03)

Request:
- Header-only frame (8 bytes)
- flags ignored
- reserved must be 0
- n_bits must be 0

Action:
- Drive system reset pin low

Response:
- status = OK on success
- status = INTERNAL_ERR on driver failure
- no payload

### 5.5 GET_FW_VERSION (0x04)

Request:
- Header-only frame (8 bytes)
- flags ignored
- reserved must be 0
- n_bits must be 0

Action:
- Return firmware version string from FW_VERSION_STRING

Response:
- status = OK
- flags = 0
- n_bits = 32 * 8 = 256
- payload length = 32 bytes
- payload format = null-terminated UTF-8/ASCII string in fixed 32-byte buffer, zero padded

### 5.6 GET_DEVICE_ID (0x05)

Request:
- Header-only frame (8 bytes)
- flags ignored
- reserved must be 0
- n_bits must be 0

Action:
- Return binary device identifier from Zephyr hwinfo_get_device_id()

Response on success:
- status = OK
- flags = 0
- n_bits = N * 8
- payload length = N bytes (N > 0)
- payload format = raw binary identifier bytes

Response on failure:
- status = INTERNAL_ERR
- no payload

## 6. Status Codes

Response status (byte 0):
- 0x00: JTAG_STATUS_OK
- 0x01: JTAG_STATUS_BAD_CMD
- 0x02: JTAG_STATUS_BAD_LEN
- 0x03: JTAG_STATUS_INTERNAL_ERR

Typical causes:
- BAD_CMD: unknown cmd value
- BAD_LEN: malformed frame, reserved != 0, invalid n_bits, incorrect total frame length, frame overflow
- INTERNAL_ERR: lower-level JTAG/reset processing failure

## 7. Host-Side Requirements

1. Use bulk transfers on endpoints 0x01 OUT and 0x81 IN.
2. Build request frames exactly as defined; especially:
   - reserved field must be 0
   - SCAN length must match n_bits
   - Header-only commands must use n_bits = 0 and 8-byte frame length
3. Read and parse one full response per request.
4. For GET_FW_VERSION, treat payload as fixed 32-byte buffer and trim trailing null bytes.
5. For GET_DEVICE_ID, treat payload as raw bytes and encode/decode as hex if text output is required.
6. Handle transient USB re-enumeration after firmware reset/flash before opening the probe.

## 8. Compatibility Notes

- Protocol versioning field is not currently present.
- Unknown commands are rejected with BAD_CMD.
- SCAN max n_bits is firmware-limited to 4096 bits per request.

Any protocol extension should preserve current header semantics and explicit status reporting.
