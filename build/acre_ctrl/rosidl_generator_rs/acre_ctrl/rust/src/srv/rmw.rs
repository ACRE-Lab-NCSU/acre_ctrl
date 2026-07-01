#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



#[link(name = "acre_ctrl__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__acre_ctrl__srv__Go2StartSequence_Request() -> *const std::ffi::c_void;
}

#[link(name = "acre_ctrl__rosidl_generator_c")]
extern "C" {
    fn acre_ctrl__srv__Go2StartSequence_Request__init(msg: *mut Go2StartSequence_Request) -> bool;
    fn acre_ctrl__srv__Go2StartSequence_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Go2StartSequence_Request>, size: usize) -> bool;
    fn acre_ctrl__srv__Go2StartSequence_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Go2StartSequence_Request>);
    fn acre_ctrl__srv__Go2StartSequence_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Go2StartSequence_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<Go2StartSequence_Request>) -> bool;
}

// Corresponds to acre_ctrl__srv__Go2StartSequence_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Go2StartSequence_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub structure_needs_at_least_one_member: u8,

}



impl Default for Go2StartSequence_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !acre_ctrl__srv__Go2StartSequence_Request__init(&mut msg as *mut _) {
        panic!("Call to acre_ctrl__srv__Go2StartSequence_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Go2StartSequence_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { acre_ctrl__srv__Go2StartSequence_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { acre_ctrl__srv__Go2StartSequence_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { acre_ctrl__srv__Go2StartSequence_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Go2StartSequence_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Go2StartSequence_Request where Self: Sized {
  const TYPE_NAME: &'static str = "acre_ctrl/srv/Go2StartSequence_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__acre_ctrl__srv__Go2StartSequence_Request() }
  }
}


#[link(name = "acre_ctrl__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__acre_ctrl__srv__Go2StartSequence_Response() -> *const std::ffi::c_void;
}

#[link(name = "acre_ctrl__rosidl_generator_c")]
extern "C" {
    fn acre_ctrl__srv__Go2StartSequence_Response__init(msg: *mut Go2StartSequence_Response) -> bool;
    fn acre_ctrl__srv__Go2StartSequence_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Go2StartSequence_Response>, size: usize) -> bool;
    fn acre_ctrl__srv__Go2StartSequence_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Go2StartSequence_Response>);
    fn acre_ctrl__srv__Go2StartSequence_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Go2StartSequence_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<Go2StartSequence_Response>) -> bool;
}

// Corresponds to acre_ctrl__srv__Go2StartSequence_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Go2StartSequence_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub message: rosidl_runtime_rs::String,

}



impl Default for Go2StartSequence_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !acre_ctrl__srv__Go2StartSequence_Response__init(&mut msg as *mut _) {
        panic!("Call to acre_ctrl__srv__Go2StartSequence_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Go2StartSequence_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { acre_ctrl__srv__Go2StartSequence_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { acre_ctrl__srv__Go2StartSequence_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { acre_ctrl__srv__Go2StartSequence_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Go2StartSequence_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Go2StartSequence_Response where Self: Sized {
  const TYPE_NAME: &'static str = "acre_ctrl/srv/Go2StartSequence_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__acre_ctrl__srv__Go2StartSequence_Response() }
  }
}






#[link(name = "acre_ctrl__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__acre_ctrl__srv__Go2StartSequence() -> *const std::ffi::c_void;
}

// Corresponds to acre_ctrl__srv__Go2StartSequence
#[allow(missing_docs, non_camel_case_types)]
pub struct Go2StartSequence;

impl rosidl_runtime_rs::Service for Go2StartSequence {
    type Request = Go2StartSequence_Request;
    type Response = Go2StartSequence_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__acre_ctrl__srv__Go2StartSequence() }
    }
}


