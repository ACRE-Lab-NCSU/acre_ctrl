"""
Module: robotarium.py
Author: Nicholas Sutton
Date: 2026-07-22
Description: P controller for a Unicycle model system based on the 
             design presented in (doi: 10.1109/MCS.2019.2949973)


Copyright 2026 Nicholas Sutton
 
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
 
http://www.apache.org/licenses/LICENSE-2.0
"""

from acre_ctrl.algorithm import ComponentRegistry, ControlAlgorithm, components
from geometry_msgs.msg import Twist, Pose
from nav_msgs.msg import Odometry

import numpy as np
from scipy.spatial.transform import Rotation as R


@components("odom", "goal")
class Robotarium(ControlAlgorithm):
    """
    P controller for a Unicycle model system based on the 
    design presented in (doi: 10.1109/MCS.2019.2949973)
    """
    def __init__(self):
        """
        Initialize a Robotarium controller object
        """
        self.goal_tolerance     = 0.05 # meters
        self.l                  = 0.15 # meters
        self.max_linear         = 0.4 # m/s
        self.max_angular        = 0.4 # rad/s
        self.k_p                = 0.5
        self.goal_reached       = False

    def compute(self, input: ComponentRegistry) -> Twist:
        """
        Computes the desired linear and angular velocity based on a goal position

        Args:
            input: The input components for the control algorithm. In this case odometry and a goal pose

        Returns:
            The desired linear and angular velocity
        """
        cmd = Twist()

        if self.goal_reached:
            print("Goal reached")
            return cmd

        odom: Odometry = input.odom
        goal: Pose= input.goal

        if odom is None or goal is None:
            return cmd

        curr_pos = np.array([odom.pose.pose.position.x, odom.pose.pose.position.y])
        q = odom.pose.pose.orientation
        curr_theta = R.from_quat([q.x, q.y, q.z, q.w]).as_euler('zyx', degrees=False)[0]

        goal_pos = np.array([goal.pose.position.x, goal.pose.position.y])

        p_error = goal_pos - curr_pos
        if np.linalg.norm(p_error) < self.goal_tolerance:
            print("Goal reached")
            self.goal_reached = True
            cmd.linear.x = float(0.0)
            cmd.angular.z = float(0.0)
            return cmd

        s_pos = curr_pos + self.l * np.array([np.cos(curr_theta), np.sin(curr_theta)])
        s_dot = self.k_p * (goal_pos - s_pos)

        R_inverse = np.array([
            [np.cos(curr_theta), np.sin(curr_theta)],
            [-(1/self.l) * np.sin(curr_theta), (1/self.l) * np.cos(curr_theta)]
        ])

        nominal_cmd = R_inverse @ s_dot
        cmd.linear.x = float(nominal_cmd[0])
        cmd.angular.z = float(nominal_cmd[1])
        return cmd
