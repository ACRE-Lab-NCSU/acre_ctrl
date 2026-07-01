// generated from rosidl_typesupport_fastrtps_cpp/resource/idl__rosidl_typesupport_fastrtps_cpp.hpp.em
// with input from acre_ctrl:srv/Go2StartSequence.idl
// generated code does not contain a copyright notice

#ifndef ACRE_CTRL__SRV__DETAIL__GO2_START_SEQUENCE__ROSIDL_TYPESUPPORT_FASTRTPS_CPP_HPP_
#define ACRE_CTRL__SRV__DETAIL__GO2_START_SEQUENCE__ROSIDL_TYPESUPPORT_FASTRTPS_CPP_HPP_

#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "acre_ctrl/msg/rosidl_typesupport_fastrtps_cpp__visibility_control.h"
#include "acre_ctrl/srv/detail/go2_start_sequence__struct.hpp"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

#include "fastcdr/Cdr.h"

namespace acre_ctrl
{

namespace srv
{

namespace typesupport_fastrtps_cpp
{

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_acre_ctrl
cdr_serialize(
  const acre_ctrl::srv::Go2StartSequence_Request & ros_message,
  eprosima::fastcdr::Cdr & cdr);

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_acre_ctrl
cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  acre_ctrl::srv::Go2StartSequence_Request & ros_message);

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_acre_ctrl
get_serialized_size(
  const acre_ctrl::srv::Go2StartSequence_Request & ros_message,
  size_t current_alignment);

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_acre_ctrl
max_serialized_size_Go2StartSequence_Request(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

}  // namespace typesupport_fastrtps_cpp

}  // namespace srv

}  // namespace acre_ctrl

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_acre_ctrl
const rosidl_message_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, acre_ctrl, srv, Go2StartSequence_Request)();

#ifdef __cplusplus
}
#endif

// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"
// already included above
// #include "acre_ctrl/msg/rosidl_typesupport_fastrtps_cpp__visibility_control.h"
// already included above
// #include "acre_ctrl/srv/detail/go2_start_sequence__struct.hpp"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

// already included above
// #include "fastcdr/Cdr.h"

namespace acre_ctrl
{

namespace srv
{

namespace typesupport_fastrtps_cpp
{

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_acre_ctrl
cdr_serialize(
  const acre_ctrl::srv::Go2StartSequence_Response & ros_message,
  eprosima::fastcdr::Cdr & cdr);

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_acre_ctrl
cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  acre_ctrl::srv::Go2StartSequence_Response & ros_message);

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_acre_ctrl
get_serialized_size(
  const acre_ctrl::srv::Go2StartSequence_Response & ros_message,
  size_t current_alignment);

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_acre_ctrl
max_serialized_size_Go2StartSequence_Response(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

}  // namespace typesupport_fastrtps_cpp

}  // namespace srv

}  // namespace acre_ctrl

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_acre_ctrl
const rosidl_message_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, acre_ctrl, srv, Go2StartSequence_Response)();

#ifdef __cplusplus
}
#endif

#include "rmw/types.h"
#include "rosidl_typesupport_cpp/service_type_support.hpp"
// already included above
// #include "rosidl_typesupport_interface/macros.h"
// already included above
// #include "acre_ctrl/msg/rosidl_typesupport_fastrtps_cpp__visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_acre_ctrl
const rosidl_service_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, acre_ctrl, srv, Go2StartSequence)();

#ifdef __cplusplus
}
#endif

#endif  // ACRE_CTRL__SRV__DETAIL__GO2_START_SEQUENCE__ROSIDL_TYPESUPPORT_FASTRTPS_CPP_HPP_
