from acre_ctrl.algorithm import ComponentRegistry, ControlAlgorithm, components
from geometry_msgs.msg import Twist, Pose
from nav_msgs.msg import Odometry, Path

import numpy as np
from scipy.spatial.transform import Rotation as R


@components("odom", "goal")
class PurePursuit(ControlAlgorithm):
    def __init__(self):
        self.goal_tolerance = 0.05
        self.k_linear = 0.8
        self.k_angular = 2.0
        self.max_linear = 0.8 # m/s
        self.max_angular = 0.8 # rad/s
    
    def compute(self, input: ComponentRegistry) -> Twist:
        cmd: Twist = Twist()

        odom: Odometry  = input.odom
        goal: Pose = input.goal

        # Store current point
        curr_x: float = odom.pose.pose.position.x
        curr_y: float = odom.pose.pose.position.y

        # Find the yaw from the quat
        q = odom.pose.pose.orientation
        r = R.from_quat([q.x, q.y, q.z, q.w])
        theta = r.as_euler('zyx', degrees=False)[0]

        # Store goal
        goal_x: float = goal.position.x
        goal_y: float = goal.position.y

        # Calculate the difference
        dx: float = goal_x - curr_x
        dy: float = goal_y - curr_y
        ld: float = np.hypot(dx, dy) # Look-ahead distance

        # Check if the goal is reached
        if ld < self.goal_tolerance:
            return cmd
        
        target_heading: float = np.arctan2(dy, dx)
        norm_diff: float = np.arctan2(np.sin(target_heading - theta),
                                    np.cos(target_heading - theta))
        
        # Apply Control
        heading_scale = max(0.0, np.cos(norm_diff)) # Slow down as robot approaches goal
        linear_vel = self.k_linear * ld * heading_scale
        angular_vel = self.k_angular * norm_diff

        cmd.linear.x = float(np.clip(linear_vel, -self.max_linear, self.max_linear))
        cmd.angular.z = float(np.clip(angular_vel, -self.max_angular, self.max_angular))

        return cmd
