"""
Module: algorithm_base.py
Author: Nicholas Sutton
Date: 2026-07-22
Description: An abstract interface for Control Algorithms

Copyright 2026 Nicholas Sutton
 
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
 
http://www.apache.org/licenses/LICENSE-2.0
"""

from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import Optional
from geometry_msgs.msg import Twist, Pose
from nav_msgs.msg import Odometry, Path
from trajectory_msgs.msg import JointTrajectory
from sensor_msgs.msg import PointCloud2
from grid_map_msgs.msg import GridMap
from sensor_msgs.msg import Imu
from dataclasses import fields as dc_fields

@dataclass
class ComponentRegistry:
    """
    A ComponentRegistry defines all the inputs available to a control algorithm
    """
    dt:             Optional[float]             = None
    pose:           Optional[Pose]              = None
    odom:           Optional[Odometry]          = None
    goal:           Optional[Pose]              = None
    path:           Optional[Path]              = None
    traj:           Optional[JointTrajectory]   = None
    point_cloud:    Optional[PointCloud2]       = None
    map:            Optional[GridMap]           = None
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
    """
    An abstract interface for control algorithms
    """
    @abstractmethod
    def compute(self, input: ComponentRegistry) -> Twist:
        """
        Computes the desired linear and angular velocity

        Args:
            input: The input components for the control algorithm.

        Returns:
            The desired linear and angular velocity
        """
        ...