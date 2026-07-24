from acre_ctrl.algorithm import ComponentRegistry, ControlAlgorithm, components
from geometry_msgs.msg import Twist, Pose
from nav_msgs.msg import Odometry

import numpy as np
from scipy.spatial.transform import Rotation as R


@components("odom", "goal", "map")
class SdfCbf(ControlAlgorithm):
    def __init__(self):
        self.goal_tolerance     = 0.05 # meters
        self.l                  = 0.15 # meters
        self.max_linear         = 0.8 # m/s
        self.max_angular        = 0.5 # rad/s
        self.alpha              = 0.01
        self.k_p = 0.5

    def unicycle_control(self, curr_pos, curr_theta, goal_pos):
        s_pos = curr_pos + self.l * np.array([np.cos(curr_theta), np.sin(curr_theta)])
        s_dot = self.k_p * (goal_pos - s_pos)

        R_inverse = np.array([
            [np.cos(curr_theta), np.sin(curr_theta)],
            [-(1/self.l) * np.sin(curr_theta), (1/self.l) * np.cos(curr_theta)]
        ])

        return R_inverse @ s_dot
    
    def cbf(self, sdf_map, curr_pos):
        h = sdf_map.atPosition("sdf", curr_pos)
        gx = sdf_map.atPosition("sdf_grad_x", curr_pos)
        gy = sdf_map.atPosition("sdf_grad_y", curr_pos)
        return h, np.array([gx, gy, 0])


    def safety_filter(self, nominal_cmd, h, grad_h, curr_theta):
        # Take the Lie Derivative of h(x) over the control vector field g(x)
        g_x = np.array([[np.cos(curr_theta), 0],
                        [np.sin(curr_theta), 0],
                        [0,                  1]])
        L_g = grad_h @ g_x

        # Determine value of eta
        eta = 0
        if np.any(L_g):
            eta = -((L_g @ nominal_cmd) + (self.alpha * h)) / np.sum(L_g**2)

        # For this system the CBF-QP has a closed form solution
        return nominal_cmd + np.maximum(0, eta) * L_g

    def compute(self, input: ComponentRegistry) -> Twist:
        # Store inputs
        odom: Odometry = input.odom
        goal: Pose = input.goal
        sdf_map = input.map

        # Store current pose and orientation
        curr_pos = np.array([odom.pose.pose.position.x, odom.pose.pose.position.y])
        q = odom.pose.pose.orientation
        curr_theta = R.from_quat([q.x, q.y, q.z, q.w]).as_euler('zyx', degrees=False)[0]

        # Store goal pose and orientation
        goal_pos = np.array([goal.position.x, goal.position.y])

        p_error = goal_pos - curr_pos
        if np.linalg.norm(p_error) < self.goal_tolerance:
            print("Goal reached")
            return Twist()

        # Compute a nominal control command and clamp to vel limits
        nominal_cmd = self.unicycle_control(curr_pos, curr_theta, goal_pos)

        # Query map for h and compute the gradient at the current position
        h, grad_h = self.cbf(sdf_map, curr_pos)

        # Apply safety filter to nominal command
        safe_cmd = self.safety_filter(nominal_cmd, h, grad_h, curr_theta)

        cmd = Twist()
        cmd.linear.x = float(safe_cmd[0])
        cmd.angular.z = float(safe_cmd[1])
        return cmd
