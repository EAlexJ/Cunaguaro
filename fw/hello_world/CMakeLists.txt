cmake_minimum_required(VERSION 3.15)
project(riscv_baremetal C ASM)

set(TARGET_NAME hello_riscv)

set(RISCV_ARCH "rv32i")
set(RISCV_ABI "ilp32")

set(COMMON_FLAGS "-march=${RISCV_ARCH} -mabi=${RISCV_ABI} -mcmodel=medany -ffreestanding -Wall -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS} -nostdlib")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${COMMON_FLAGS}")

add_executable(${TARGET_NAME}.elf
    entry.S
    main.c
)

target_link_options(${TARGET_NAME}.elf PRIVATE
    -T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
    -nostdlib
    -static
)

add_custom_command(TARGET ${TARGET_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} -d -S $<TARGET_FILE:${TARGET_NAME}.elf> > ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.objdump
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET_NAME}.elf> ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.bin
    BYPRODUCTS 
        ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.objdump
        ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.bin
    COMMENT "Generating disassembly (.objdump) and raw binary (.bin)"
)