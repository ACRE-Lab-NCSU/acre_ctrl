"""
Module: spin_example.py
Author: Nicholas Sutton
Date: 2026-07-22
Description: A controller that spins a robot at a constant velocity

Copyright 2026 Nicholas Sutton
 
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
 
http://www.apache.org/licenses/LICENSE-2.0
"""

from acre_ctrl.algorithm import ComponentRegistry, ControlAlgorithm, components
from geometry_msgs.msg import Twist

@components("odom")
class MyAlgo(ControlAlgorithm):
    """
    A controller that spins a robot at a constant velocity
    """
    def compute(self, input: ComponentRegistry) -> Twist:
        """
        Spins the system at a constant angular velocity

        Args:
            input: The input components for the control algorithm. In this case odometry

        Returns:
            The desired linear and angular velocity
        """
        print(f"Current linear_x{input.odom.twist.twist.linear.x}, linear_y{input.odom.twist.twist.linear.y}")
        twist = Twist()
        twist.angular.z = 0.5

        return twist