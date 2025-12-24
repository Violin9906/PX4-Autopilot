# platforms/rtthread/CMakeLists.txt

# platforms/rtthread/CMakeLists.txt

# 1. 策略设置
if(POLICY CMP0079)
	cmake_policy(SET CMP0079 NEW)
endif()

# 2. RT-Thread 内核静态库定义 (基于你之前的逻辑)
set(RTT_DIR ${CMAKE_CURRENT_LIST_DIR}/rt-thread)
set(RTT_SRCS
    ${RTT_DIR}/src/clock.c
    ${RTT_DIR}/src/components.c
    ${RTT_DIR}/src/device.c
    ${RTT_DIR}/src/idle.c
    ${RTT_DIR}/src/ipc.c
    ${RTT_DIR}/src/irq.c
    ${RTT_DIR}/src/kservice.c
    ${RTT_DIR}/src/mem.c
    ${RTT_DIR}/src/object.c
    ${RTT_DIR}/src/scheduler_up.c
    ${RTT_DIR}/src/thread.c
    ${RTT_DIR}/src/timer.c
)
set(RTT_CPU_SRCS
    ${RTT_DIR}/libcpu/arm/cortex-m7/cpuport.c
    ${RTT_DIR}/libcpu/arm/cortex-m7/context_gcc.S
)

add_library(rtos_interface STATIC ${RTT_SRCS} ${RTT_CPU_SRCS})
target_include_directories(rtos_interface PUBLIC
    ${RTT_DIR}/include
    ${RTT_DIR}/libcpu/arm/cortex-m7
    ${CMAKE_CURRENT_LIST_DIR}/include # 存放 rtconfig.h
)

# 3. 定义最终可执行文件 (px4.elf)
# PX4 习惯用一个 empty.c 作为入口编译起点
add_executable(px4 ${PX4_SOURCE_DIR}/platforms/common/empty.c)
set(FW_NAME ${PX4_BOARD_VENDOR}_${PX4_BOARD_MODEL}_${PX4_BOARD_LABEL}.elf)
set_target_properties(px4 PROPERTIES OUTPUT_NAME ${FW_NAME})

# 4. 获取所有 PX4 模块库 (由 px4_add_module 自动收集)
get_property(module_libraries GLOBAL PROPERTY PX4_MODULE_LIBRARIES)

# 5. 进入子目录 (这里会进入 src/px4/common 定义 px4_layer)
add_subdirectory(src/px4)

# 6. 核心链接逻辑
target_link_libraries(px4 PRIVATE
    px4_layer        # 你的 RTT 适配层
    rtos_interface   # RTT 内核
    ${module_libraries} # 所有应用模块 (commander, uORB等)

    # 基础编译器库
    m
    gcc
)

# 7. 链接器标志 (设置内存布局和优化)
# 注意：你需要一个链接脚本，暂时可以参考 NuttX 的，或者使用 RTT 默认的
target_link_options(px4 PRIVATE
    -nostartfiles
    -nodefaultlibs
    -nostdlib
    -Wl,--gc-sections
    -Wl,-Map=${PX4_BINARY_DIR}/${PX4_CONFIG}.map
    -Wl,--print-memory-usage
    # -T ${YOUR_LINKER_SCRIPT} # 以后在这里指定 .ld 文件
)

# 8. 生成二进制文件 (.bin)
set(PX4_BINARY_OUTPUT ${PX4_BINARY_DIR}/${PX4_CONFIG}.bin)
add_custom_command(OUTPUT ${PX4_BINARY_OUTPUT}
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:px4> ${PX4_BINARY_OUTPUT}
    DEPENDS px4
)
add_custom_target(px4_bin ALL DEPENDS ${PX4_BINARY_OUTPUT})

# 9. 生成 PX4 固件包 (用于 QGC 上传)
if (TARGET parameters_xml AND TARGET airframes_xml)
    set(fw_package ${PX4_BINARY_DIR}/${PX4_BOARD_VENDOR}_${PX4_BOARD_MODEL}_${PX4_BOARD_LABEL}.px4)
    add_custom_command(
        OUTPUT ${fw_package}
        COMMAND ${PYTHON_EXECUTABLE} ${PX4_SOURCE_DIR}/Tools/px_mkfw.py
            --prototype ${PX4_SOURCE_DIR}/boards/${PX4_BOARD_VENDOR}/${PX4_BOARD_MODEL}/firmware.prototype
            --git_identity ${PX4_SOURCE_DIR}
            --parameter_xml ${PX4_BINARY_DIR}/parameters.xml
            --airframe_xml ${PX4_BINARY_DIR}/airframes.xml
            --image ${PX4_BINARY_OUTPUT} > ${fw_package}
        DEPENDS ${PX4_BINARY_OUTPUT} parameters_xml airframes_xml
        COMMENT "Creating ${fw_package}"
    )
    add_custom_target(px4_package ALL DEPENDS ${fw_package})
endif()
