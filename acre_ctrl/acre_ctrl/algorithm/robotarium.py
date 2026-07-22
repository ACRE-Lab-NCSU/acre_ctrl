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
        self.l                  = 0.05 # meters
        self.max_linear         = 0.4 # m/s
        self.max_angular        = 0.4 # rad/s

    def compute(self, input: ComponentRegistry) -> Twist:
        """
        Computes the desired linear and angular velocity based on a goal position

        Args:
            input: The input components for the control algorithm. In this case odometry and a goal pose

        Returns:
            The desired linear and angular velocity
        """
        cmd = Twist()

        # Store inputs
        odom: Odometry = input.odom
        goal: Pose = input.goal

        # Store current pose and orientation
        curr_pos = np.array([odom.pose.pose.position.x, odom.pose.pose.position.y])
        q = odom.pose.pose.orientation
        curr_theta = R.from_quat([q.x, q.y, q.z, q.w]).as_euler('zyx', degrees=False)[0]

        # Store goal pose and orientation
        goal_pos = np.array([goal.position.x, goal.position.y])
        gq = goal.orientation
        goal_theta = R.from_quat([gq.x, gq.y, gq.z, gq.w]).as_euler('zyx', degrees=False)[0]

        # Calculate error
        p_dot = goal_pos - curr_pos
        
        # Check if the goal is reached
        if np.linalg.norm(p_dot) < self.goal_tolerance:
            print("Goal reached")
            return cmd

        theta_dot = np.arctan2(np.sin(goal_theta - curr_theta), 
                             np.cos(goal_theta - curr_theta))
        dcm = np.array([-np.sin(curr_theta), np.cos(curr_theta)])

        # Store command components
        s_dot = p_dot + (self.l * theta_dot * dcm)
        R_inverse = np.array([
            [np.cos(curr_theta), np.sin(curr_theta)],
            [-(1 / self.l) * np.sin(curr_theta), (1 / self.l) * np.cos(curr_theta)]
        ])

        # Compute linear and angular velocity
        nominal_cmd = R_inverse @ s_dot
        nominal_cmd = np.array([
            np.clip(nominal_cmd[0], -self.max_linear, self.max_linear),
            np.clip(nominal_cmd[1], -self.max_angular, self.max_angular)
        ])

        cmd = Twist()
        cmd.linear.x = float(nominal_cmd[0])
        cmd.angular.z = float(nominal_cmd[1])
        return cmd
