"""acre_ctrl/algorithm/cdr.py

Generic bridge between C++ rclcpp messages and Python rclpy messages.
Neither side needs to know about the other's memory layout -- both already
know how to (de)serialize themselves to/from CDR bytes, so we just use that.

Add a new field to ComponentRegistry and you write ONE line on the C++ side
(the type name string) -- nothing here needs to change.
"""
import importlib
from rclpy.serialization import deserialize_message, serialize_message

_type_cache: dict[str, type] = {}


def _resolve(type_name: str) -> type:
    """'nav_msgs/msg/Odometry' -> the actual rclpy message class."""
    if type_name not in _type_cache:
        try:
            pkg, _, cls_name = type_name.split("/")
        except ValueError as e:
            raise ValueError(
                f"Expected 'pkg/msg/Type' format, got: {type_name!r}"
            ) from e
        mod = importlib.import_module(f"{pkg}.msg")
        _type_cache[type_name] = getattr(mod, cls_name)
    return _type_cache[type_name]


def from_cdr(data: bytes, type_name: str):
    """Deserialize raw CDR bytes (from rclcpp::Serialization) into the
    corresponding rclpy message object."""
    return deserialize_message(bytes(data), _resolve(type_name))


def to_cdr(msg) -> bytes:
    """Serialize an rclpy message object into raw CDR bytes for
    rclcpp::Serialization to deserialize on the C++ side."""
    return bytes(serialize_message(msg))
