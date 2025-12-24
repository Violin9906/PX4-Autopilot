add_subdirectory(common)

# 临时定义伪目标，直到我们真正开始写 RTT 的设备驱动对接
add_library(arch_adc INTERFACE)
add_library(arch_gpio INTERFACE)
add_library(arch_board_reset INTERFACE)
add_library(arch_version INTERFACE)
add_library(arch_io_pins INTERFACE)
add_library(arch_console INTERFACE)
add_library(arch_hrt INTERFACE)
add_library(arch_dshot INTERFACE)
add_library(arch_tone_alarm INTERFACE)
