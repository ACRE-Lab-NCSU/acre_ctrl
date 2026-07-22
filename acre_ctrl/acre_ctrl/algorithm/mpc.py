from acre_ctrl.algorithm import ComponentRegistry, ControlAlgorithm, components
from geometry_msgs.msg import Twist, Pose
from nav_msgs.msg import Odometry

import osqp
import numpy as np
from scipy.spatial.transform import Rotation
from scipy import sparse


@components("odom", "goal", "dt")
class MPC(ControlAlgorithm):
    def __init__(self):
        self.goal_tolerance = 0.05
        self.goal_theta_tolerance = 0.05
        self.max_linear = 0.8
        self.max_angular = 0.8
        self.N = 10         
        self._initialized = False

    def compute(self, input: ComponentRegistry) -> Twist:
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
        curr_theta = Rotation.from_quat([q.x, q.y, q.z, q.w]).as_euler('zyx', degrees=False)[0]

        # get goal position and orientation
        goal_x = goal.position.x
        goal_y = goal.position.y
        gq = goal.orientation
        goal_theta = Rotation.from_quat([gq.x, gq.y, gq.z, gq.w]).as_euler('zyx', degrees=False)[0]


        # check if we have reached our goal (within tolerance)
        dx = goal_x - curr_x
        dy = goal_y - curr_y

        d_theta = np.arctan2(np.sin(goal_theta - curr_theta), 
                        np.cos(goal_theta - curr_theta))

        # return if goal has been reached
        if np.hypot(dx, dy) < self.goal_tolerance and abs(d_theta) < self.goal_theta_tolerance:
            return cmd

        # Rotate world-frame error into robot body frame???

        x0 = np.array([curr_x, curr_y, curr_theta])
        xg = np.array([goal_x, goal_y, goal_theta])

        # Build A, B 
        cos_t, sin_t = np.cos(curr_theta), np.sin(curr_theta)

        v = np.sqrt(
            odom.twist.twist.linear.x**2 +
            odom.twist.twist.linear.y**2
        )

        A = sparse.csc_matrix([
            0.0, 0.0, -v*sin_t*dt,
            0.0, 0.0, v*cos_t*dt,
            0.0, 0.0, 0.0
        ])

        B = sparse.csc_matrix([
            cos_t*dt, 0.0,
            sin_t*dt, 0.0,
            0.0, 1.0
        ])

        # Build cost penalties Q, R
        Q = sparse.diags([10.0, 10.0, 2.0])
        QN = Q

        R = sparse.diags([0.1, 0.1])

        N = self.N

        # encoding constraints and input parameters for OSQP

        # Quadratic optimization
        P = sparse.block_diag([sparse.kron(sparse.eye(N), Q), QN,
                sparse.kron(sparse.eye(N), R)], format='csc')

        
        # Linear optimization
        q = np.hstack([np.kron(np.ones(N), -Q@xg), -QN@xg, np.zeros(N*nu)])





