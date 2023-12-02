# A bare metal example for NUCLEO-F334R8 based on STM32F334R8

## Useful links
- https://github.com/cpq/bare-metal-programming-guide/tree/main
- http://stefanfrings.de/stm32/stm32f3.html
- 

## Setup
```
brew install homebrew/cask/gcc-arm-embedded
arm-none-eabi-gcc --version

brew install openocd
openocd -v
Open On-Chip Debugger 0.12.0

brew install stlink
st-info --version
```

1. on https://github.com/STMicroelectronics search cmsis_device_f3, set latest version
2. with stm32 cube mx create *.ld file once (create new project and export as Makefile, then get the file)
3. get link o startup_stm32f33*.s file from cmsis_f3/Source/Templates/gcc
4. targets / interfaces for openocd: /usr/local/Cellar/open-ocd/0.12.0_1/share/openocd/scripts/[target|interface]
5. update include in hal.h, eg #include <stm32f334x8.h> found in cmsis_f3/Include/

6. openocd -f stm32f334r8.cfg
7. arm-none-eabi-gdb -ex "target remote :3333" -ex "load" -ex "continue" firmware.elf
   or
   arm-none-eabi-gdb -ex "target extended | openocd -f stm32f334r8.cfg -c 'gdb_port pipe'" firmware.elf -ex "load" -ex "monitor reset halt" -ex "set confirm off"
8. in another terminal: tail -f debug.log

## GDB commands
```
run                  start over
continue, c          continue
finish               continue until current function finished
step
step N
next                 does not step into function
q                    quit gdb
print var
set var=val
break lineno         set breakpoint at line number
break func           set breakpoint at specified function
break class:func
info break
delete               delete all break-, watch-, catchpoints
clear func           delete all breakpoints in function
clear lineno         delete breakpoints at line number
delete no            delete break-, watch-, catchpoints specified by number
```
