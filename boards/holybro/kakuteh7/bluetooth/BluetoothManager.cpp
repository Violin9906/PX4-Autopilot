#include "BluetoothManager.hpp"
#include <board_config.h>

BluetoothManager::BluetoothManager() :
	ModuleParams(nullptr),
	ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::lp_default)
{
}

BluetoothManager::~BluetoothManager()
{
	perf_free(_loop_perf);
	perf_free(_loop_interval_perf);
}

bool BluetoothManager::init()
{
	int32_t bt_mode;
	param_get(param_find("BT_MODE"), &bt_mode);

	/* 开机按参数设一次 */
	switch (bt_mode) {
	default:
	case 0:
		update_gpio(false);
		break;

	case 1:
		update_gpio(true);
		break;

	case 2:
		update_gpio(!_armed);
		break;
	}

	ScheduleOnInterval(1_s);

	return true;
}


void BluetoothManager::Run()
{

	if (should_exit()) {
		ScheduleClear();
		exit_and_cleanup();
		return;
	}

	perf_begin(_loop_perf);
	perf_count(_loop_interval_perf);

	if (_parameter_update_sub.updated()) {
		// clear update
		parameter_update_s param_update;
		_parameter_update_sub.copy(&param_update);
		updateParams(); // update module parameters (in DEFINE_PARAMETERS)
	}

	int32_t mode = _param_bt_mode.get();

	switch (mode) {
	default:
	case 0:
		update_gpio(false);
		break;

	case 1:
		update_gpio(true);
		break;

	case 2:

		{

			if (_vehicle_status_sub.updated()) {
				vehicle_status_s vehicle_status;

				if (_vehicle_status_sub.copy(&vehicle_status)) {

					const bool armed = (vehicle_status.arming_state == vehicle_status_s::ARMING_STATE_ARMED);

					_armed = armed;
				}
			}

			update_gpio(!_armed);
		}
		break;
	}

	perf_end(_loop_perf);
}

void BluetoothManager::update_gpio(bool on)
{
	if (on == _last_bt_state) { return; }

	px4_arch_gpiowrite(GPIO_RF_SWITCH, on);
	_last_bt_state = on;
	PX4_INFO("Bluetooth RF switch %s", on ? "ON" : "OFF");
}

int BluetoothManager::task_spawn(int argc, char *argv[])
{
	BluetoothManager *instance = new BluetoothManager();

	if (instance) {
		_object.store(instance);
		_task_id = task_id_is_work_queue;

		if (instance->init()) {
			return PX4_OK;
		}

	} else {
		PX4_ERR("alloc failed");
	}

	delete instance;
	_object.store(nullptr);
	_task_id = -1;

	return PX4_ERROR;
}

int BluetoothManager::print_status()
{
	perf_print_counter(_loop_perf);
	perf_print_counter(_loop_interval_perf);
	return 0;
}

int BluetoothManager::custom_command(int argc, char *argv[])
{
	return print_usage("unknown command");
}

int BluetoothManager::print_usage(const char *reason)
{
	if (reason) { PX4_WARN("%s", reason); }

	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description
Board-level Bluetooth power manager for Kakute H7.

)DESCR_STR");
    return 0;
}

extern "C" __EXPORT int bluetooth_mgr_main(int argc, char *argv[]);
int bluetooth_mgr_main(int argc, char *argv[])
{
    return BluetoothManager::main(argc, argv);
}
