from dataclasses import dataclass
from typing import Optional
from .components import OdomData, GoalData, PathData

@dataclass
class ComponentRegistry:
    odom:  Optional[OdomData]  = None
    goal:  Optional[GoalData]  = None
    path:  Optional[PathData]  = None