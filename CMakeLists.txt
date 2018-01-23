project(FSMC C ASM)
cmake_minimum_required(VERSION 3.9.0)

add_definitions(-DSTM32F4xx -DSTM32F40XX -DUSE_STDPERIPH_DRIVER)

file(GLOB_RECURSE USER_SOURCES src/*.s src/*.c)
file(GLOB_RECURSE STL_SOURCES Libraries/STM32F4xx_StdPeriph_Driver/src/*.c)

include_directories(inc)
include_directories(Libraries/STM32F4xx_StdPeriph_Driver/inc)
include_directories(Libraries/Device/STM32F4xx/Include)
include_directories(Libraries/CMSIS/Include)

add_executable(${PROJECT_NAME}.elf ${USER_SOURCES} ${STL_SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.map")
set(HEX_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.bin)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")
