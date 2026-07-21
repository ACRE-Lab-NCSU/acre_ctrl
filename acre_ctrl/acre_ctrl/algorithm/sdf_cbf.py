from acre_ctrl.algorithm import ComponentRegistry, ControlAlgorithm, components
from geometry_msgs.msg import Twist, Pose
from nav_msgs.msg import Odometry
import grid_map_python as gmp

import numpy as np
from scipy.spatial.transform import Rotation as R


@components("odom", "goal", "map")
class SdfCbf(ControlAlgorithm):
    def __init__(self):
        self.goal_tolerance = 0.05
        self.l = 0.05 
        self.alpha = 0.01
        self.max_linear = 0.8 # m/s
        self.max_angular = 0.8 # rad/s

    def single_integrator_control(self, curr_pos, curr_theta, goal_pos, goal_theta):

        # Calculate error
        p_dot = goal_pos - curr_pos
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
        return R_inverse @ s_dot
    
    def cbf(self, sdf_map, curr_pos):
        eps = sdf_map.getResolution()
        h_center = sdf_map.atPosition("sdf", curr_pos)
        h_dx = sdf_map.atPosition("sdf", np.array([curr_pos[0] + eps, curr_pos[1]]))
        h_dy = sdf_map.atPosition("sdf", np.array([curr_pos[0], curr_pos[1] + eps]))

        grad_h = np.array([(h_dx - h_center) / eps, (h_dy - h_center) / eps])
        return h_center, grad_h


    def safety_filter(self, nominal_cmd, h, grad_h, curr_theta):
        v = nominal_cmd[0]
        world_vel = np.array([v * np.cos(curr_theta), v * np.sin(curr_theta)])



    
    def compute(self, input: ComponentRegistry) -> Twist:
        cmd = Twist()

        odom: Odometry = input.odom
        goal: Pose = input.goal
        sdf_map = input.map

        # Store current pose and orientation
        curr_pos = np.array([odom.pose.pose.position.x, odom.pose.pose.position.y])
        q = odom.pose.pose.orientation
        curr_theta = R.from_quat([q.x, q.y, q.z, q.w]).as_euler('zyx', degrees=False)[0]

        # Store goal pose and orientation
        goal_pos = np.array([goal.position.x, goal.position.y])
        gq = goal.orientation
        goal_theta = R.from_quat([gq.x, gq.y, gq.z, gq.w]).as_euler('zyx', degrees=False)[0]

        # Compute a nominal control command
        nominal_cmd = self.single_integrator_control(curr_pos, curr_theta, goal_pos, goal_theta)

        # Query map for h and compute the gradient at the current position
        h, grad_h = self.cbf(sdf_map, curr_pos)

        # Apply safety filter to nominal command
        safe_cmd = self.safety_filter(nominal_cmd, h, grad_h, curr_theta)

        # Clip safe cmd to velocity limits
        cmd.linear.x = float(np.clip(safe_cmd[0], -self.max_linear, self.max_linear))
        cmd.angular.z = float(np.clip(safe_cmd[1], -self.max_angular, self.max_angular))
        return cmd
