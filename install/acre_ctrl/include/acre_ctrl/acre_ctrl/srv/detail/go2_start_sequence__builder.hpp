// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from acre_ctrl:srv/Go2StartSequence.idl
// generated code does not contain a copyright notice

#ifndef ACRE_CTRL__SRV__DETAIL__GO2_START_SEQUENCE__BUILDER_HPP_
#define ACRE_CTRL__SRV__DETAIL__GO2_START_SEQUENCE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "acre_ctrl/srv/detail/go2_start_sequence__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace acre_ctrl
{

namespace srv
{


}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::acre_ctrl::srv::Go2StartSequence_Request>()
{
  return ::acre_ctrl::srv::Go2StartSequence_Request(rosidl_runtime_cpp::MessageInitialization::ZERO);
}

}  // namespace acre_ctrl


namespace acre_ctrl
{

namespace srv
{

namespace builder
{

class Init_Go2StartSequence_Response_message
{
public:
  explicit Init_Go2StartSequence_Response_message(::acre_ctrl::srv::Go2StartSequence_Response & msg)
  : msg_(msg)
  {}
  ::acre_ctrl::srv::Go2StartSequence_Response message(::acre_ctrl::srv::Go2StartSequence_Response::_message_type arg)
  {
    msg_.message = std::move(arg);
    return std::move(msg_);
  }

private:
  ::acre_ctrl::srv::Go2StartSequence_Response msg_;
};

class Init_Go2StartSequence_Response_success
{
public:
  Init_Go2StartSequence_Response_success()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Go2StartSequence_Response_message success(::acre_ctrl::srv::Go2StartSequence_Response::_success_type arg)
  {
    msg_.success = std::move(arg);
    return Init_Go2StartSequence_Response_message(msg_);
  }

private:
  ::acre_ctrl::srv::Go2StartSequence_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::acre_ctrl::srv::Go2StartSequence_Response>()
{
  return acre_ctrl::srv::builder::Init_Go2StartSequence_Response_success();
}

}  // namespace acre_ctrl

#endif  // ACRE_CTRL__SRV__DETAIL__GO2_START_SEQUENCE__BUILDER_HPP_
