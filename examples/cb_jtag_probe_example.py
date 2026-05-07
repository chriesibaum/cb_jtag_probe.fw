
# Simple usage and test of the CBJtagProbe to verify basic functionality.

from cb_jtag import CBJtagProbe

if __name__ == "__main__":
    with CBJtagProbe() as jtag:
        v = jtag.get_version()
        print(f"Probe Firmware Version: {v}")

        # Example JTAG write-read operation
        # This will just reset the TAP state machine
        tdi = bytes([0x00, 0x00, 0x00, 0x00])  # TDI data to send
        tms = bytes([0xff, 0xff, 0xff, 0xff])  # TMS data to send (all high to reset TAP)
        tdo = bytearray(len(tdi))

        # Reset the target system using the JTAG sys reset pin
        # STM32x's would like to have the sys reset pin low
        jtag.set_sys_reset_pin_low()

        # Perform JTAG write-read operation
        jtag.jtag_write_read(tdi, tdo, tms, len(tdi) * 8)
        print(f"TDO Received: {[hex(b) for b in tdo]}")

        # Release the reset pin after the operation
        jtag.set_sys_reset_pin_high()
