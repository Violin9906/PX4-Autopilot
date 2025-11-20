px4_add_module(
    MODULE boards__holybro__kakuteh7__bluetooth
    MAIN bluetooth_mgr
    STACK_MAIN 1200
    SRCS
        BluetoothManager.cpp
        BluetoothManager.hpp
    DEPENDS
        px4_work_queue
)
