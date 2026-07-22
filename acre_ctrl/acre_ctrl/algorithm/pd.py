"""
Module: pd.py
Author: Nicholas Sutton
Date: 2026-07-22
Description: PD controller for a Unicycle model system

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


@components("odom", "goal", "dt")
class PD(ControlAlgorithm):
    """
    PD controller for a Unicycle model system
    """
    def __init__(self):
        """
        Initialize a PD controller object
        """
        self.goal_tolerance = 0.05
        self.goal_theta_tolerance = 0.05
        self.k_p = 0.8
        self.k_d = 0.3
        self.max_linear = 0.8
        self.max_angular = 0.8
        self.prev_error_x = 0.0
        self.prev_error_theta = 0.0
        self._initialized = False

    def compute(self, input: ComponentRegistry) -> Twist:
        """
        Computes the desired linear and angular velocity based on a goal position

        Args:
            input: The input components for the control algorithm. In this case odometry, a goal pose, and dt

        Returns:
            The desired linear and angular velocity
        """
        cmd = Twist()
        dt = input.dt
        if dt <= 0.0:
            return cmd

        odom: Odometry = input.odom
        goal: Pose = input.goal

        # Get current position and orientation
        curr_x = odom.pose.pose.position.x
        curr_y = odom.pose.pose.position.y
        q = odom.pose.pose.orientation
        curr_theta = R.from_quat([q.x, q.y, q.z, q.w]).as_euler('zyx', degrees=False)[0]

        # get goal position and orientation
        goal_x = goal.position.x
        goal_y = goal.position.y
        gq = goal.orientation
        goal_theta = R.from_quat([gq.x, gq.y, gq.z, gq.w]).as_euler('zyx', degrees=False)[0]

        # Calculate error
        dx = goal_x - curr_x
        dy = goal_y - curr_y
        d_theta = np.arctan2(np.sin(goal_theta - curr_theta), 
                             np.cos(goal_theta - curr_theta))

        # check if goal has been reached
        if np.hypot(dx, dy) < self.goal_tolerance and abs(d_theta) < self.goal_theta_tolerance:
            return cmd

        # Rotate world-frame error into robot body frame
        cos_t, sin_t = np.cos(curr_theta), np.sin(curr_theta)
        error_x = cos_t * dx + sin_t * dy

        if not self._initialized:
            self.prev_error_x = error_x
            self.prev_error_theta = d_theta
            self._initialized = True

        # Find derivate component
        dv_x = (error_x - self.prev_error_x) / dt
        dv_theta = (d_theta - self.prev_error_theta) / dt

        # Update error values
        self.prev_error_x = error_x
        self.prev_error_theta = d_theta

        # Apply PD Control Law
        lin_x = self.k_p * error_x + self.k_d * dv_x
        ang_z = self.k_p * d_theta + self.k_d * dv_theta

        cmd.linear.x = float(np.clip(lin_x, -self.max_linear, self.max_linear))
        cmd.angular.z = float(np.clip(ang_z, -self.max_angular, self.max_angular)) # Clamp angular velocity
        return cmd
