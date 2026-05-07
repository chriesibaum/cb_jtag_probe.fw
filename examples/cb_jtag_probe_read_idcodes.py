# Example usage of the CBJtagProbe to read IDCODEs from TAPs in the JTAG chain.

from cb_jtag import CBJtagProbe
from cb_jtag import CBJtag

# Create an instance of the CBJtagProbe to interact with the JTAG interface
probe = CBJtagProbe()

# Setup the JTAG interface for boundary-scan operations
jtag = CBJtag(jtag_probe=probe)

print(f'Probe Version: {jtag.get_probe_version()}')

# Hold the reset pin low for STM32xxx
jtag.set_sys_reset_pin_low()
# Reset the JTAG TAP controller
jtag.tap_reset()

# Get the number of TAPs in the JTAG chain
num_taps = jtag.get_taps_in_chain()
print(f'The following {num_taps} TAPs have been detected in the JTAG chain:' )

# Read and display the IDCODEs of all TAPs
id_codes = jtag.get_tap_id_code(num_taps)
for i, idcode in enumerate(id_codes):
    print(f'  TAP {i}: '
            f'IDCODE: 0x{idcode:08X}')