# A bare metal example for NUCLEO-F334R8 based on STM32F334R8

## Useful links
- https://github.com/cpq/bare-metal-programming-guide/tree/main
- http://stefanfrings.de/stm32/stm32f3.html

## of interest
- https://github.com/wd5gnr/mbed-swo/
  based on mbed, debugport.printf()

## Install STM32CubeMX on a mac
see https://community.st.com/t5/stm32cubemx-mcus/how-to-open-stm32cubemx-6-3-0-on-macos/td-p/227909
```
open ~/Downloads/en.stm32cubemx-mac-v6-9-2/SetupSTM32CubeMX-6.9.2.app/Contents/MacOs/SetupSTM32CubeMX-6_9_2
```
- it'll complain "java" cannot be opened because the developer cannot be verified.
- go to system settings/privacy&security, scroll down -> "java has been blocked" -> "allow anyway
- re-run command above

I used CubeMX to get the STM32F334R8Tx_FLASH.ld needed below
1. start CubeMX
2. choose your chip
3. project manager
   - add any project name, eg temporaryproject
   - add project location
   - application structure "Basic"
   - in toolchain/IDE select "Makefile"
4. generate code
5. get the *.ld file

## Setup toolchain
```
brew install homebrew/cask/gcc-arm-embedded
arm-none-eabi-gcc --version

brew install openocd
openocd -v
Open On-Chip Debugger 0.12.0

brew install stlink
st-info --version
```

```
make debug
make monitor (in separate terminal)
```

Installation is based on the following steps and are reflected mainly in the Makefile, so no need to run it manually, but might be useful to setup the toolchain of another STM32 chip:
1. on https://github.com/STMicroelectronics search cmsis_device_f3, set latest version
2. with stm32 cube mx create *.ld file once (create new project and export as Makefile, then get the file)
3. get link of startup_stm32f33*.s file from cmsis_f3/Source/Templates/gcc
4. targets / interfaces for openocd: /usr/local/Cellar/open-ocd/0.12.0_1/share/openocd/scripts/[target|interface]
5. update include in hal.h, eg #include <stm32f334x8.h> found in cmsis_f3/Include/

6. openocd -f stm32f334r8.cfg
7. arm-none-eabi-gdb -ex "target extended | openocd -f stm32f334r8.cfg -c 'gdb_port pipe'" firmware.elf -ex "load" -ex "monitor reset halt" -ex "set confirm off"
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
break fn:lineno      set breakpoint at line number of file fn
break func           set breakpoint at specified function
break class:func
info break
delete               delete all break-, watch-, catchpoints
clear func           delete all breakpoints in function
clear lineno         delete breakpoints at line number
delete no            delete break-, watch-, catchpoints specified by number
```
