#=============================================================================
# Required OS Interface Functions:
#   * px4_os_add_flags
#   * px4_os_determine_build_chip
#   * px4_os_prebuild_targets
#=============================================================================

# 1. 设置编译标志
function(px4_os_add_flags)

	# 包含 RT-Thread 核心头文件路径
	include_directories(SYSTEM
		${PX4_SOURCE_DIR}/platforms/rtthread/rt-thread/include
		${PX4_SOURCE_DIR}/platforms/rtthread/rt-thread/libcpu/arm/cortex-m7
		${PX4_SOURCE_DIR}/platforms/rtthread/include # 存放 rtconfig.h
	)

	# PX4 适配层头文件路径 (必须放在前面)
	include_directories(
		${PX4_SOURCE_DIR}/platforms/rtthread/src/px4/common/include
	)

	set(cxx_flags)
	list(APPEND cxx_flags
		-fno-exceptions
		-fno-rtti
		-fno-sized-deallocation
		-fno-threadsafe-statics
		# RT-Thread 通常配合工具链的标准 C++ 库，或者使用其组件。这里先保持与 PX4 策略一致
	)

	foreach(flag ${cxx_flags})
		add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${flag}>)
	endforeach()

	add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wbad-function-cast>)

	# 平台宏定义
	add_definitions(
		-D__PX4_RTTHREAD
		-D__RTTHREAD__
		-D__PX4_POSIX   # 告诉 PX4 使用 POSIX 风格的 API
	)

endfunction()

# 2. 确定芯片信息 (直接硬编码为 H7，直到我们做 K230)
function(px4_os_determine_build_chip)
	set(CHIP "stm32h7" CACHE STRING "PX4 Chip" FORCE)
	set(CHIP_MANUFACTURER "stm" CACHE STRING "PX4 Chip Manufacturer" FORCE)

    set(PX4_CHIP ${CHIP} CACHE STRING "PX4 Chip" FORCE)
	set(PX4_CHIP_MANUFACTURER ${CHIP_MANUFACTURER} CACHE STRING "PX4 Chip Manufacturer" FORCE)
endfunction()

# 3. 预编译目标
function(px4_os_prebuild_targets)
	px4_parse_function_args(
			NAME px4_os_prebuild_targets
			ONE_VALUE OUT BOARD
			REQUIRED OUT
			ARGN ${ARGN})

	# 创建一个伪目标，确保 uorb 映射头文件先生成
	add_library(prebuild_targets INTERFACE)
	add_dependencies(prebuild_targets uorb_headers)

	# 链接 RT-Thread 静态库 (我们在 platforms/rtthread/CMakeLists.txt 中定义的)
	target_link_libraries(prebuild_targets INTERFACE rtos_interface m gcc)

	set(${OUT} prebuild_targets PARENT_SCOPE)
endfunction()
