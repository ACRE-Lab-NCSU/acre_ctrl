// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from acre_ctrl:srv/Go2StartSequence.idl
// generated code does not contain a copyright notice

#ifndef ACRE_CTRL__SRV__DETAIL__GO2_START_SEQUENCE__STRUCT_H_
#define ACRE_CTRL__SRV__DETAIL__GO2_START_SEQUENCE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/Go2StartSequence in the package acre_ctrl.
typedef struct acre_ctrl__srv__Go2StartSequence_Request
{
  uint8_t structure_needs_at_least_one_member;
} acre_ctrl__srv__Go2StartSequence_Request;

// Struct for a sequence of acre_ctrl__srv__Go2StartSequence_Request.
typedef struct acre_ctrl__srv__Go2StartSequence_Request__Sequence
{
  acre_ctrl__srv__Go2StartSequence_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} acre_ctrl__srv__Go2StartSequence_Request__Sequence;


// Constants defined in the message

// Include directives for member types
// Member 'message'
#include "rosidl_runtime_c/string.h"

/// Struct defined in srv/Go2StartSequence in the package acre_ctrl.
typedef struct acre_ctrl__srv__Go2StartSequence_Response
{
  bool success;
  rosidl_runtime_c__String message;
} acre_ctrl__srv__Go2StartSequence_Response;

// Struct for a sequence of acre_ctrl__srv__Go2StartSequence_Response.
typedef struct acre_ctrl__srv__Go2StartSequence_Response__Sequence
{
  acre_ctrl__srv__Go2StartSequence_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} acre_ctrl__srv__Go2StartSequence_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ACRE_CTRL__SRV__DETAIL__GO2_START_SEQUENCE__STRUCT_H_
