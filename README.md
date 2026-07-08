# acre_ctrl
## Introduction
The **acre_ctrl** package is a ROS2 package used to write and run control algorithms for the Unitree Go2. Before using **acre_ctrl**, please familiarize yourself with the steps to [connect with the Go2](https://github.com/ACRE-Lab-NCSU/ACRE-Wiki/wiki/Unitree-Go2-Quadruped/).

## Using acre_ctrl
#### Sportmode Driver
All control algorithms communicate with the ```sportmode_driver``` node. This node is responsible for communicating with the Unitree SDK and executing control commands on the Go2. To start the **sportmode_driver** node run:
```shell
ros2 run acre_ctrl sportmode_driver
``` 
#### sportmode_driver params:
| Parameter            | Default | Options | Description |
| -------------------- | ------- | - | ----------- |
| gait                 | classic | Classic or Agile | The Go2's gait. See the [Go2 Docs](https://support.unitree.com/home/en/developer/Motion_Services_Interface_V2.0) for more information |


#### Using Python Algorithms with the Control Node
**acre_ctrl** allows users to write simple algorithm classes in Python and then dynamically load these classes into the control loop to be executed. An algorithm class can be executed using the following command. Note that generally you should run the ```sportmode_driver``` and then the ```control_node```.
```shell
ros2 run acre_ctrl control_node --ros-args -p algorithm:=/path/to/algorithm.py
```

#### control_node params:
| Parameter            |        Default             | Description |
| -------------------- | -------------------------- | ----------- |
| algorithm            | None                       | The path to the control algorithm to execute |
| frequency(Hz)        | 10.0                       | The frequency to run the control algorithm at |
| pose_topic           | /mocap_pose                | Pose topic name |
| odom_topic           | /sport_odom                | Odometry topic name |
| map_topic            | /map                       | Map topic name |
| point_cloud_topic    | /unitree/slam_lidar/points | Point Cloud topic name |
| imu_topic            | /unitree/slam_lidar/imu    | IMU topic name |
| path_topic           | /plan                      | Plan topic name |
| traj_topic           | /joint_trajectory          | Trajectory topic name |

## Spin Example
This example uses the acre_ctrl stack to make the Go2 spin in place.
```shell
ros2 run acre_ctrl sportmode_driver --ros-args -p gait:="classic"
ros2 run acre_ctrl control_node --ros-args -p algorithm:=/workspace/src/acre_ctrl/example/spin_example.py
```

## Writing Algorithms for acre_ctrl
#### Python Algorithms
**acre_ctrl** provides a convient interface for writing control algorithms in python. To write a python control algorithm, you must create a class that inherits from the ```ControlAlgorithm``` class.

Control algorithms must implement a ```compute``` function
```python
class ControlAlgorithm(ABC):
    def init(self) -> None:
        pass

    @abstractmethod
    def compute(self, input: ComponentRegistry) -> Twist:
        ...
```

The input to the ```compute``` function is a ```ComponentRegistry```. This class stores all the possible inputs available for algorithm authors. When writing an algorithm you can access the input components you need using ROS2 types.
```python
@dataclass
class ComponentRegistry:
    pose:           Optional[Pose]              = None
    odom:           Optional[Odometry]          = None
    goal:           Optional[Pose]              = None
    path:           Optional[Path]              = None
    traj:           Optional[JointTrajectory]   = None
    point_cloud:    Optional[PointCloud2]       = None
    map:            Optional[OccupancyGrid]     = None
    imu:            Optional[Imu]               = None
```

To define which of the inputs you algorithm uses, you must use the ```@components(...)``` class decorator. This decorator tells **acre_ctrl** which inputs your algorithm expects. The naming conventions for the decorator follow the names of the ```ComponentRegistry``` fields.
```python
@components("odom", "goal", "map")
```

#### spin_example.py algorithm
To understand how to write algorithm code for **acre_ctrl** refer to this simple example:
```python
from acre_ctrl.algorithm import ComponentRegistry, ControlAlgorithm, components
from geometry_msgs.msg import Twist, Pose

@components("odom")
class MyAlgo(ControlAlgorithm):
    
    def compute(self, input: ComponentRegistry) -> Twist:
        print(f"Current linear_x{input.odom.twist.twist.linear.x}, linear_y{input.odom.twist.twist.linear.y}")
        twist = Twist()
        twist.angular.z = 0.5

        return twist
```

#### C++ Algorithms
Unlike Python algorithms, C++ algorithms cannot be loaded dynamically using the ```control_node```. If you wish to write a C++ algorithm you should write a C++ ROS2 node that communicates directly with the ```sportmode_driver```.

