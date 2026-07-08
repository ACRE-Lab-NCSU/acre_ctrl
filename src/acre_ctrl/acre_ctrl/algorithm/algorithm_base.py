# acre_ctrl/algorithm/algorithm_base.py
from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import Optional
from geometry_msgs.msg import Twist, Pose
from nav_msgs.msg import Odometry, Path
from trajectory_msgs.msg import JointTrajectory
from sensor_msgs.msg import PointCloud2
from nav_msgs.msg import OccupancyGrid
from sensor_msgs.msg import Imu
from dataclasses import fields as dc_fields

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

_VALID_COMPONENTS = frozenset(f.name for f in dc_fields(ComponentRegistry))

def components(*names: str):
    """
        Class decorator declaring which ComponentRegistry fields an algorithm needs.
        Validated against ComponentRegistry's actual fields at decoration time.
    """
    unknown = set(names) - _VALID_COMPONENTS
    if unknown:
        raise ValueError(
            f"Unknown ComponentRegistry field(s): {sorted(unknown)}. "
            f"Valid fields: {sorted(_VALID_COMPONENTS)}"
        )

    def decorator(cls):
        cls._components = tuple(names)
        return cls

    return decorator

class ControlAlgorithm(ABC):
    def init(self) -> None:
        pass

    @abstractmethod
    def compute(self, input: ComponentRegistry) -> Twist:
        ...