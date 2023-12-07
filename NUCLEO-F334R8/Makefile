# STM32F334R8

CFLAGS ?= -W -Wall -Wextra -Werror -Wundef -Wshadow -Wdouble-promotion
CFLAGS += -Wformat-truncation -fno-common -Wconversion
CFLAGS += -DSTM32F334x8 # needed to include <stm32f3xx.h>
CFLAGS += -g3 -Os -ffunction-sections -fdata-sections
CFLAGS += -I. -Icmsis_core/CMSIS/Core/Include -Icmsis_f3/Include
CFLAGS += -mcpu=cortex-m4 -mthumb
LDFLAGS ?= -TSTM32F334R8Tx_FLASH.ld -nostartfiles -nostdlib --specs nano.specs -lc -lgcc -Wl,--gc-sections -Wl,-Map=$@.map

SOURCES = main.c retarget.c
SOURCES += cmsis_f3/Source/Templates/gcc/startup_stm32f334x8.s

RM = rm -rf

debug: firmware.elf
	@printf "" > debug.log
	@arm-none-eabi-gdb -ex "target extended | openocd -f stm32f334r8.cfg -c 'gdb_port pipe'" firmware.elf -ex "load" -ex "monitor reset halt" -ex "set confirm off" -ex "run"

debug-nostart: firmware.elf
	@printf "" > debug.log
	@arm-none-eabi-gdb -ex "target extended | openocd -f stm32f334r8.cfg -c 'gdb_port pipe'" firmware.elf -ex "load" -ex "monitor reset halt" -ex "set confirm off"

monitor:
	@touch debug.log; tail -n0 -f debug.log | ./monitor.zsh
	#@[ ! -f "debug.log" ] && touch debug.log; printf "" > debug.log; tail -n0 -f debug.log | ./monitor.zsh


build: firmware.bin

firmware.elf: cmsis_core cmsis_f3 hal.h STM32F334R8Tx_FLASH.ld Makefile stm32f334r8.cfg $(SOURCES)
	arm-none-eabi-gcc $(SOURCES) $(CFLAGS) $(CFLAGS_EXTRA) $(LDFLAGS) -o $@

firmware.bin: firmware.elf
	arm-none-eabi-objcopy -O binary $< $@

flash-openocd: firmware.elf
	openocd -f interface/stlink.cfg -f target/stm32f3x.cfg -c "program firmware.elf verify reset exit"

flash: firmware.bin
	st-flash --reset write $< 0x8000000

cmsis_core:
	git clone --depth 1 -b 5.9.0 https://github.com/ARM-software/CMSIS_5 $@

cmsis_f3:
	git clone --depth 1 -b v2.3.7 https://github.com/STMicroelectronics/cmsis_device_f3 $@

uid:
	openocd  -f interface/stlink.cfg -f target/stm32f3x.cfg -c "init" -c "mdw 0x1FFFF7AC 3" -c "exit"
clean:
	$(RM) firmware.* cmsis_*
