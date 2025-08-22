# Apps & API
The rover modules have been tested and integrated with a subset of the available [Apps & API](../middleware/index.md) methods. \
We specifically provide a guide for integration with [ROS 2](../ros2/index.md) using the following methods:

| Method                                                    | Description                                                                                                                  |
| --------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------- |
| (Recommended) [PX4 ROS 2 Interface](#px4-ros-2-interface) | Register a custom mode and publish [RoverSetpointTypes](../ros2/px4_ros2_control_interface.md#experimental-rover-setpoints). |
| [ROS 2 Offboard Control](#ros-2-offboard-control)         | Publish to PX4 topics that are specified in [dds_topics.yaml](../middleware/dds_topics.md) (includes rover setpoints).       |

## PX4 ROS 2 Interface
We recommend the use of the [PX4 ROS 2 Interface Library](../ros2/px4_ros2_interface_lib.md) which allows you to register a custom drive mode and exposes [RoverSetpointTypes](../ros2/px4_ros2_control_interface.md#experimental-rover-setpoints). By using these setpoints instead of directly the PX4 internal rover setpoints, you are guaranteed to send valid control inputs to your vehicle and the control flags are automatically set for you.
Additionally, registering a custom drive mode instead of using [ROS 2 Offboard Control](#ros-2-offboard-control) provides all the advantages listed [here](#) (TODO add advantages list).

An example for the PX4 ROS 2 Interface Library being used for rovers can be found here: TODO: add link.


## ROS 2 Offboard Control
[ROS 2 Offboard Control](../ros2/offboard_control.md) provides access to all topics specified in [dds_topics.yaml](../middleware/dds_topics.md). This provides more options than with the [PX4 ROS 2 Interface](#px4-ros-2-interface), but requries a deeper understanding of PX4 and the structure of the rover modules.
