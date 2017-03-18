cmake_minimum_required(VERSION 3.6)
project(FSMC)

set(CMAKE_CXX_STANDARD 11)

include_directories(
        inc
        Libraries/CMSIS/Include
        Libraries/Device/STM32F4xx/Include
        Libraries/STM32F4xx_StdPeriph_Driver/inc)

set(SOURCE_FILES
        Libraries/STM32F4xx_StdPeriph_Driver/src/misc.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_adc.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_can.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_crc.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_aes.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_des.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_tdes.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dac.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dbgmcu.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dcmi.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fsmc.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_md5.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_sha1.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_iwdg.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_pwr.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rng.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rtc.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c
        Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_wwdg.c
        src/AsciiLib.c
        src/delay.c
        src/main.cpp
        src/stm32f4xx_it.c
        src/system_stm32f4xx.c
        src/lib.c
        src/dac.c
        src/menu.c)

add_executable(FSMC ${SOURCE_FILES})
