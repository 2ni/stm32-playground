# STM32F334R8
# 1. on https://github.com/STMicroelectronics search cmsis_device_f3, set latest version
# 2. with stm32 cube mx create *.ld file once (create new project and export as Makefile, then get the file)
# 3. get link o startup_stm32f33*.s file from cmsis_f3/Source/Templates/gcc
# 4. targets / interfaces for openocd: /usr/local/Cellar/open-ocd/0.12.0_1/share/openocd/scripts/[target|interface]
# 5. update include in hal.h, eg #include <stm32f334x8.h> found in cmsis_f3/Include/
#
# 6. openocd -f stm32f334r8.cfg
# 7. arm-none-eabi-gdb -ex "target remote :3333" -ex "load" -ex "continue" firmware.elf
# or
# arm-none-eabi-gdb -ex "target extended | openocd -f stm32f334r8.cfg -c 'gdb_port pipe'" firmware.elf \
-ex "monitor reset halt" -ex "load" -ex "monitor reset" -ex "continue"
# in another terminal: tail -f debug.log

CFLAGS ?= -W -Wall -Wextra -Werror -Wundef -Wshadow -Wdouble-promotion
CFLAGS += -Wformat-truncation -fno-common -Wconversion
CFLAGS += -g3 -Os -ffunction-sections -fdata-sections
CFLAGS += -I. -Icmsis_core/CMSIS/Core/Include -Icmsis_f3/Include
CFLAGS += -mcpu=cortex-m4 -mthumb
LDFLAGS ?= -TSTM32F334R8Tx_FLASH.ld -nostartfiles -nostdlib --specs nano.specs -lc -lgcc -Wl,--gc-sections -Wl,-Map=$@.map

SOURCES = main.c
SOURCES += cmsis_f3/Source/Templates/gcc/startup_stm32f334x8.s

RM = rm -rf

debug: firmware.elf
	@printf "" > debug.log
	@arm-none-eabi-gdb -ex "target extended | openocd -f stm32f334r8.cfg -c 'gdb_port pipe'" firmware.elf -ex "load" -ex "monitor reset halt" -ex "set confirm off"

monitor:
	@[ ! -f "debug.log" ] && touch debug.log; printf "" > debug.log; tail -f debug.log


build: firmware.bin

firmware.elf: cmsis_core cmsis_f3 hal.h STM32F334R8Tx_FLASH.ld Makefile $(SOURCES)
	arm-none-eabi-gcc $(SOURCES) $(CFLAGS) $(CFLAGS_EXTRA) $(LDFLAGS) -o $@

firmware.bin: firmware.elf
	arm-none-eabi-objcopy -O binary $< $@

flash-openocd: firmware.elf
	openocd -f interface/stlink-v2-1.cfg -f target/stm32f3x.cfg -c "program firmware.elf verify reset exit"

flash: firmware.bin
	st-flash --reset write $< 0x8000000

cmsis_core:
	git clone --depth 1 -b 5.9.0 https://github.com/ARM-software/CMSIS_5 $@

cmsis_f3:
	git clone --depth 1 -b v2.3.7 https://github.com/STMicroelectronics/cmsis_device_f3 $@

clean:
	$(RM) firmware.* cmsis_*
