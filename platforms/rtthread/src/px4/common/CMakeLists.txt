# platforms/rtthread/src/px4/common/CMakeLists.txt

# 1. 设置 RTT 源码路径 (注意目录名大小写需与你 git submodule 的一致)
set(RTT_DIR ${CMAKE_CURRENT_LIST_DIR}/rt-thread)

# 2. RTT 核心内核源码 (保持不变)
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

# 3. CPU 架构相关 (Cortex-M7)
set(RTT_CPU_SRCS
    ${RTT_DIR}/libcpu/arm/cortex-m7/cpuport.c
    ${RTT_DIR}/libcpu/arm/cortex-m7/context_gcc.S
)

# 4. 包含路径
# 注意：这里加入了 platforms/rtthread/include，后面我们会在这里放适配头文件
set(RTT_INCLUDES
    ${RTT_DIR}/include
    ${RTT_DIR}/libcpu/arm/cortex-m7
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# 5. 创建静态库并链接
# 在 PX4 中，通常我们直接 add_library(px4_layer ...)，因为它是平台的核心接口实现
add_library(rtos_interface STATIC ${RTT_SRCS} ${RTT_CPU_SRCS})
target_include_directories(rtos_interface PUBLIC ${RTT_INCLUDES})

# 6. 处理 PX4 平台层必备的实现文件 (这是接下来的重头戏)
# 先定义一个基础的库，后续我们会往里面加 px4_rtthread_impl.cpp 等
px4_add_library(px4_layer
    src/px4_rtthread_impl.cpp  # 这个文件我们马上要创建
)
target_link_libraries(px4_layer PRIVATE rtos_interface)
