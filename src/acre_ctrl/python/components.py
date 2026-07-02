from dataclasses import dataclass, field
import math

@dataclass
class OdomData:
    x:             float = 0.0
    y:             float = 0.0
    z:             float = 0.0
    heading:       float = 0.0
    linear_vel_x:  float = 0.0
    linear_vel_y:  float = 0.0
    angular_vel_z: float = 0.0
    dt:            float = 0.0

@dataclass
class GoalData:
    goal_x:        float = 0.0
    goal_y:        float = 0.0
    goal_heading:  float = 0.0
    heading_error: float = 0.0
    distance:      float = 0.0

@dataclass
class Waypoint:
    x:       float = 0.0
    y:       float = 0.0
    heading: float = 0.0

@dataclass
class PathData:
    waypoints:   list[Waypoint] = field(default_factory=list)
    current_idx: int = 0

@dataclass
class Action:
    pass

@dataclass
class VelocityAction(Action):
    linear_vel:  float = 0.0
    angular_vel: float = 0.0