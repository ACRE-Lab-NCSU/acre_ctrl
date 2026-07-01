// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from acre_ctrl:srv/Go2StartSequence.idl
// generated code does not contain a copyright notice

#ifndef ACRE_CTRL__SRV__DETAIL__GO2_START_SEQUENCE__FUNCTIONS_H_
#define ACRE_CTRL__SRV__DETAIL__GO2_START_SEQUENCE__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/visibility_control.h"
#include "acre_ctrl/msg/rosidl_generator_c__visibility_control.h"

#include "acre_ctrl/srv/detail/go2_start_sequence__struct.h"

/// Initialize srv/Go2StartSequence message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * acre_ctrl__srv__Go2StartSequence_Request
 * )) before or use
 * acre_ctrl__srv__Go2StartSequence_Request__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
bool
acre_ctrl__srv__Go2StartSequence_Request__init(acre_ctrl__srv__Go2StartSequence_Request * msg);

/// Finalize srv/Go2StartSequence message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
void
acre_ctrl__srv__Go2StartSequence_Request__fini(acre_ctrl__srv__Go2StartSequence_Request * msg);

/// Create srv/Go2StartSequence message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * acre_ctrl__srv__Go2StartSequence_Request__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
acre_ctrl__srv__Go2StartSequence_Request *
acre_ctrl__srv__Go2StartSequence_Request__create();

/// Destroy srv/Go2StartSequence message.
/**
 * It calls
 * acre_ctrl__srv__Go2StartSequence_Request__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
void
acre_ctrl__srv__Go2StartSequence_Request__destroy(acre_ctrl__srv__Go2StartSequence_Request * msg);

/// Check for srv/Go2StartSequence message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
bool
acre_ctrl__srv__Go2StartSequence_Request__are_equal(const acre_ctrl__srv__Go2StartSequence_Request * lhs, const acre_ctrl__srv__Go2StartSequence_Request * rhs);

/// Copy a srv/Go2StartSequence message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
bool
acre_ctrl__srv__Go2StartSequence_Request__copy(
  const acre_ctrl__srv__Go2StartSequence_Request * input,
  acre_ctrl__srv__Go2StartSequence_Request * output);

/// Initialize array of srv/Go2StartSequence messages.
/**
 * It allocates the memory for the number of elements and calls
 * acre_ctrl__srv__Go2StartSequence_Request__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
bool
acre_ctrl__srv__Go2StartSequence_Request__Sequence__init(acre_ctrl__srv__Go2StartSequence_Request__Sequence * array, size_t size);

/// Finalize array of srv/Go2StartSequence messages.
/**
 * It calls
 * acre_ctrl__srv__Go2StartSequence_Request__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
void
acre_ctrl__srv__Go2StartSequence_Request__Sequence__fini(acre_ctrl__srv__Go2StartSequence_Request__Sequence * array);

/// Create array of srv/Go2StartSequence messages.
/**
 * It allocates the memory for the array and calls
 * acre_ctrl__srv__Go2StartSequence_Request__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
acre_ctrl__srv__Go2StartSequence_Request__Sequence *
acre_ctrl__srv__Go2StartSequence_Request__Sequence__create(size_t size);

/// Destroy array of srv/Go2StartSequence messages.
/**
 * It calls
 * acre_ctrl__srv__Go2StartSequence_Request__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
void
acre_ctrl__srv__Go2StartSequence_Request__Sequence__destroy(acre_ctrl__srv__Go2StartSequence_Request__Sequence * array);

/// Check for srv/Go2StartSequence message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
bool
acre_ctrl__srv__Go2StartSequence_Request__Sequence__are_equal(const acre_ctrl__srv__Go2StartSequence_Request__Sequence * lhs, const acre_ctrl__srv__Go2StartSequence_Request__Sequence * rhs);

/// Copy an array of srv/Go2StartSequence messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
bool
acre_ctrl__srv__Go2StartSequence_Request__Sequence__copy(
  const acre_ctrl__srv__Go2StartSequence_Request__Sequence * input,
  acre_ctrl__srv__Go2StartSequence_Request__Sequence * output);

/// Initialize srv/Go2StartSequence message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * acre_ctrl__srv__Go2StartSequence_Response
 * )) before or use
 * acre_ctrl__srv__Go2StartSequence_Response__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
bool
acre_ctrl__srv__Go2StartSequence_Response__init(acre_ctrl__srv__Go2StartSequence_Response * msg);

/// Finalize srv/Go2StartSequence message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
void
acre_ctrl__srv__Go2StartSequence_Response__fini(acre_ctrl__srv__Go2StartSequence_Response * msg);

/// Create srv/Go2StartSequence message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * acre_ctrl__srv__Go2StartSequence_Response__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
acre_ctrl__srv__Go2StartSequence_Response *
acre_ctrl__srv__Go2StartSequence_Response__create();

/// Destroy srv/Go2StartSequence message.
/**
 * It calls
 * acre_ctrl__srv__Go2StartSequence_Response__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
void
acre_ctrl__srv__Go2StartSequence_Response__destroy(acre_ctrl__srv__Go2StartSequence_Response * msg);

/// Check for srv/Go2StartSequence message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
bool
acre_ctrl__srv__Go2StartSequence_Response__are_equal(const acre_ctrl__srv__Go2StartSequence_Response * lhs, const acre_ctrl__srv__Go2StartSequence_Response * rhs);

/// Copy a srv/Go2StartSequence message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
bool
acre_ctrl__srv__Go2StartSequence_Response__copy(
  const acre_ctrl__srv__Go2StartSequence_Response * input,
  acre_ctrl__srv__Go2StartSequence_Response * output);

/// Initialize array of srv/Go2StartSequence messages.
/**
 * It allocates the memory for the number of elements and calls
 * acre_ctrl__srv__Go2StartSequence_Response__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
bool
acre_ctrl__srv__Go2StartSequence_Response__Sequence__init(acre_ctrl__srv__Go2StartSequence_Response__Sequence * array, size_t size);

/// Finalize array of srv/Go2StartSequence messages.
/**
 * It calls
 * acre_ctrl__srv__Go2StartSequence_Response__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
void
acre_ctrl__srv__Go2StartSequence_Response__Sequence__fini(acre_ctrl__srv__Go2StartSequence_Response__Sequence * array);

/// Create array of srv/Go2StartSequence messages.
/**
 * It allocates the memory for the array and calls
 * acre_ctrl__srv__Go2StartSequence_Response__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
acre_ctrl__srv__Go2StartSequence_Response__Sequence *
acre_ctrl__srv__Go2StartSequence_Response__Sequence__create(size_t size);

/// Destroy array of srv/Go2StartSequence messages.
/**
 * It calls
 * acre_ctrl__srv__Go2StartSequence_Response__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
void
acre_ctrl__srv__Go2StartSequence_Response__Sequence__destroy(acre_ctrl__srv__Go2StartSequence_Response__Sequence * array);

/// Check for srv/Go2StartSequence message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
bool
acre_ctrl__srv__Go2StartSequence_Response__Sequence__are_equal(const acre_ctrl__srv__Go2StartSequence_Response__Sequence * lhs, const acre_ctrl__srv__Go2StartSequence_Response__Sequence * rhs);

/// Copy an array of srv/Go2StartSequence messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
bool
acre_ctrl__srv__Go2StartSequence_Response__Sequence__copy(
  const acre_ctrl__srv__Go2StartSequence_Response__Sequence * input,
  acre_ctrl__srv__Go2StartSequence_Response__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // ACRE_CTRL__SRV__DETAIL__GO2_START_SEQUENCE__FUNCTIONS_H_
