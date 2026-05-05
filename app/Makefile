# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2023 Chriesibaum GmbH


# select the BOARD type here:

BOARD := rpi_pico    #  Raspberry Pi Pico, RP2040, 2MB flash, 264kB RAM
# BOARD := nucleo_u385rg_q   #  Nucleo U385RG, STM32U385RG, 2MB flash, 512kB RAM




all:
	@echo get the build process working...
	west build -p auto -b $(BOARD) ./ \
		-DEXTRA_CONF_FILE="prj_log.conf" \
 		-DEXTRA_DTC_OVERLAY_FILE="app.overlay"


clean:
	rm ./build/ -rf

flash:
	west flash -r jlink


copy_uf2:
	# copy the generated .uf2 file to the mounted RPI Pico
	cp ./build/zephyr/zephyr.uf2 /media/$(USER)/RPI-RP2/


flash.stm32cubeprogrammer:
	west flash -r stm32cubeprogrammer
