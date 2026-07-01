// generated from rosidl_generator_c/resource/rosidl_generator_c__visibility_control.h.in
// generated code does not contain a copyright notice

#ifndef ACRE_CTRL__MSG__ROSIDL_GENERATOR_C__VISIBILITY_CONTROL_H_
#define ACRE_CTRL__MSG__ROSIDL_GENERATOR_C__VISIBILITY_CONTROL_H_

#ifdef __cplusplus
extern "C"
{
#endif

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define ROSIDL_GENERATOR_C_EXPORT_acre_ctrl __attribute__ ((dllexport))
    #define ROSIDL_GENERATOR_C_IMPORT_acre_ctrl __attribute__ ((dllimport))
  #else
    #define ROSIDL_GENERATOR_C_EXPORT_acre_ctrl __declspec(dllexport)
    #define ROSIDL_GENERATOR_C_IMPORT_acre_ctrl __declspec(dllimport)
  #endif
  #ifdef ROSIDL_GENERATOR_C_BUILDING_DLL_acre_ctrl
    #define ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl ROSIDL_GENERATOR_C_EXPORT_acre_ctrl
  #else
    #define ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl ROSIDL_GENERATOR_C_IMPORT_acre_ctrl
  #endif
#else
  #define ROSIDL_GENERATOR_C_EXPORT_acre_ctrl __attribute__ ((visibility("default")))
  #define ROSIDL_GENERATOR_C_IMPORT_acre_ctrl
  #if __GNUC__ >= 4
    #define ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl __attribute__ ((visibility("default")))
  #else
    #define ROSIDL_GENERATOR_C_PUBLIC_acre_ctrl
  #endif
#endif

#ifdef __cplusplus
}
#endif

#endif  // ACRE_CTRL__MSG__ROSIDL_GENERATOR_C__VISIBILITY_CONTROL_H_
