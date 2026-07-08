from acre_ctrl.algorithm import ComponentRegistry, ControlAlgorithm, components
from geometry_msgs.msg import Twist, Pose

@components("odom")
class MyAlgo(ControlAlgorithm):
    
    def compute(self, input: ComponentRegistry) -> Twist:
        print(f"Current linear_x{input.odom.twist.twist.linear.x}, linear_y{input.odom.twist.twist.linear.y}")
        twist = Twist()
        twist.angular.z = 0.5

        return twist