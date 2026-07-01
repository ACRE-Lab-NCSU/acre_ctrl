// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from acre_ctrl:srv/Go2StartSequence.idl
// generated code does not contain a copyright notice

#ifndef ACRE_CTRL__SRV__DETAIL__GO2_START_SEQUENCE__TRAITS_HPP_
#define ACRE_CTRL__SRV__DETAIL__GO2_START_SEQUENCE__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "acre_ctrl/srv/detail/go2_start_sequence__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace acre_ctrl
{

namespace srv
{

inline void to_flow_style_yaml(
  const Go2StartSequence_Request & msg,
  std::ostream & out)
{
  (void)msg;
  out << "null";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Go2StartSequence_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  (void)msg;
  (void)indentation;
  out << "null\n";
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Go2StartSequence_Request & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace acre_ctrl

namespace rosidl_generator_traits
{

[[deprecated("use acre_ctrl::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const acre_ctrl::srv::Go2StartSequence_Request & msg,
  std::ostream & out, size_t indentation = 0)
{
  acre_ctrl::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use acre_ctrl::srv::to_yaml() instead")]]
inline std::string to_yaml(const acre_ctrl::srv::Go2StartSequence_Request & msg)
{
  return acre_ctrl::srv::to_yaml(msg);
}

template<>
inline const char * data_type<acre_ctrl::srv::Go2StartSequence_Request>()
{
  return "acre_ctrl::srv::Go2StartSequence_Request";
}

template<>
inline const char * name<acre_ctrl::srv::Go2StartSequence_Request>()
{
  return "acre_ctrl/srv/Go2StartSequence_Request";
}

template<>
struct has_fixed_size<acre_ctrl::srv::Go2StartSequence_Request>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<acre_ctrl::srv::Go2StartSequence_Request>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<acre_ctrl::srv::Go2StartSequence_Request>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace acre_ctrl
{

namespace srv
{

inline void to_flow_style_yaml(
  const Go2StartSequence_Response & msg,
  std::ostream & out)
{
  out << "{";
  // member: success
  {
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
    out << ", ";
  }

  // member: message
  {
    out << "message: ";
    rosidl_generator_traits::value_to_yaml(msg.message, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Go2StartSequence_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: success
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "success: ";
    rosidl_generator_traits::value_to_yaml(msg.success, out);
    out << "\n";
  }

  // member: message
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "message: ";
    rosidl_generator_traits::value_to_yaml(msg.message, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Go2StartSequence_Response & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace srv

}  // namespace acre_ctrl

namespace rosidl_generator_traits
{

[[deprecated("use acre_ctrl::srv::to_block_style_yaml() instead")]]
inline void to_yaml(
  const acre_ctrl::srv::Go2StartSequence_Response & msg,
  std::ostream & out, size_t indentation = 0)
{
  acre_ctrl::srv::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use acre_ctrl::srv::to_yaml() instead")]]
inline std::string to_yaml(const acre_ctrl::srv::Go2StartSequence_Response & msg)
{
  return acre_ctrl::srv::to_yaml(msg);
}

template<>
inline const char * data_type<acre_ctrl::srv::Go2StartSequence_Response>()
{
  return "acre_ctrl::srv::Go2StartSequence_Response";
}

template<>
inline const char * name<acre_ctrl::srv::Go2StartSequence_Response>()
{
  return "acre_ctrl/srv/Go2StartSequence_Response";
}

template<>
struct has_fixed_size<acre_ctrl::srv::Go2StartSequence_Response>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<acre_ctrl::srv::Go2StartSequence_Response>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<acre_ctrl::srv::Go2StartSequence_Response>
  : std::true_type {};

}  // namespace rosidl_generator_traits

namespace rosidl_generator_traits
{

template<>
inline const char * data_type<acre_ctrl::srv::Go2StartSequence>()
{
  return "acre_ctrl::srv::Go2StartSequence";
}

template<>
inline const char * name<acre_ctrl::srv::Go2StartSequence>()
{
  return "acre_ctrl/srv/Go2StartSequence";
}

template<>
struct has_fixed_size<acre_ctrl::srv::Go2StartSequence>
  : std::integral_constant<
    bool,
    has_fixed_size<acre_ctrl::srv::Go2StartSequence_Request>::value &&
    has_fixed_size<acre_ctrl::srv::Go2StartSequence_Response>::value
  >
{
};

template<>
struct has_bounded_size<acre_ctrl::srv::Go2StartSequence>
  : std::integral_constant<
    bool,
    has_bounded_size<acre_ctrl::srv::Go2StartSequence_Request>::value &&
    has_bounded_size<acre_ctrl::srv::Go2StartSequence_Response>::value
  >
{
};

template<>
struct is_service<acre_ctrl::srv::Go2StartSequence>
  : std::true_type
{
};

template<>
struct is_service_request<acre_ctrl::srv::Go2StartSequence_Request>
  : std::true_type
{
};

template<>
struct is_service_response<acre_ctrl::srv::Go2StartSequence_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

#endif  // ACRE_CTRL__SRV__DETAIL__GO2_START_SEQUENCE__TRAITS_HPP_
