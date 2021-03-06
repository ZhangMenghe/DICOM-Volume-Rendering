// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: proto/common.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_proto_2fcommon_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_proto_2fcommon_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3012000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3012002 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_proto_2fcommon_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_proto_2fcommon_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[2]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_proto_2fcommon_2eproto;
class Request;
class RequestDefaultTypeInternal;
extern RequestDefaultTypeInternal _Request_default_instance_;
class commonResponse;
class commonResponseDefaultTypeInternal;
extern commonResponseDefaultTypeInternal _commonResponse_default_instance_;
PROTOBUF_NAMESPACE_OPEN
template<> ::Request* Arena::CreateMaybeMessage<::Request>(Arena*);
template<> ::commonResponse* Arena::CreateMaybeMessage<::commonResponse>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

// ===================================================================

class Request PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Request) */ {
 public:
  inline Request() : Request(nullptr) {};
  virtual ~Request();

  Request(const Request& from);
  Request(Request&& from) noexcept
    : Request() {
    *this = ::std::move(from);
  }

  inline Request& operator=(const Request& from) {
    CopyFrom(from);
    return *this;
  }
  inline Request& operator=(Request&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const Request& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Request* internal_default_instance() {
    return reinterpret_cast<const Request*>(
               &_Request_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Request& a, Request& b) {
    a.Swap(&b);
  }
  inline void Swap(Request* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Request* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Request* New() const final {
    return CreateMaybeMessage<Request>(nullptr);
  }

  Request* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Request>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const Request& from);
  void MergeFrom(const Request& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Request* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Request";
  }
  protected:
  explicit Request(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_proto_2fcommon_2eproto);
    return ::descriptor_table_proto_2fcommon_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kReqMsgFieldNumber = 2,
    kClientIdFieldNumber = 1,
  };
  // string req_msg = 2;
  void clear_req_msg();
  const std::string& req_msg() const;
  void set_req_msg(const std::string& value);
  void set_req_msg(std::string&& value);
  void set_req_msg(const char* value);
  void set_req_msg(const char* value, size_t size);
  std::string* mutable_req_msg();
  std::string* release_req_msg();
  void set_allocated_req_msg(std::string* req_msg);
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  std::string* unsafe_arena_release_req_msg();
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  void unsafe_arena_set_allocated_req_msg(
      std::string* req_msg);
  private:
  const std::string& _internal_req_msg() const;
  void _internal_set_req_msg(const std::string& value);
  std::string* _internal_mutable_req_msg();
  public:

  // int32 client_id = 1;
  void clear_client_id();
  ::PROTOBUF_NAMESPACE_ID::int32 client_id() const;
  void set_client_id(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_client_id() const;
  void _internal_set_client_id(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // @@protoc_insertion_point(class_scope:Request)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr req_msg_;
  ::PROTOBUF_NAMESPACE_ID::int32 client_id_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_proto_2fcommon_2eproto;
};
// -------------------------------------------------------------------

class commonResponse PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:commonResponse) */ {
 public:
  inline commonResponse() : commonResponse(nullptr) {};
  virtual ~commonResponse();

  commonResponse(const commonResponse& from);
  commonResponse(commonResponse&& from) noexcept
    : commonResponse() {
    *this = ::std::move(from);
  }

  inline commonResponse& operator=(const commonResponse& from) {
    CopyFrom(from);
    return *this;
  }
  inline commonResponse& operator=(commonResponse&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const commonResponse& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const commonResponse* internal_default_instance() {
    return reinterpret_cast<const commonResponse*>(
               &_commonResponse_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(commonResponse& a, commonResponse& b) {
    a.Swap(&b);
  }
  inline void Swap(commonResponse* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(commonResponse* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline commonResponse* New() const final {
    return CreateMaybeMessage<commonResponse>(nullptr);
  }

  commonResponse* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<commonResponse>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const commonResponse& from);
  void MergeFrom(const commonResponse& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(commonResponse* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "commonResponse";
  }
  protected:
  explicit commonResponse(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_proto_2fcommon_2eproto);
    return ::descriptor_table_proto_2fcommon_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kResMsgFieldNumber = 2,
    kSuccessFieldNumber = 1,
  };
  // string res_msg = 2;
  void clear_res_msg();
  const std::string& res_msg() const;
  void set_res_msg(const std::string& value);
  void set_res_msg(std::string&& value);
  void set_res_msg(const char* value);
  void set_res_msg(const char* value, size_t size);
  std::string* mutable_res_msg();
  std::string* release_res_msg();
  void set_allocated_res_msg(std::string* res_msg);
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  std::string* unsafe_arena_release_res_msg();
  GOOGLE_PROTOBUF_RUNTIME_DEPRECATED("The unsafe_arena_ accessors for"
  "    string fields are deprecated and will be removed in a"
  "    future release.")
  void unsafe_arena_set_allocated_res_msg(
      std::string* res_msg);
  private:
  const std::string& _internal_res_msg() const;
  void _internal_set_res_msg(const std::string& value);
  std::string* _internal_mutable_res_msg();
  public:

  // bool success = 1;
  void clear_success();
  bool success() const;
  void set_success(bool value);
  private:
  bool _internal_success() const;
  void _internal_set_success(bool value);
  public:

  // @@protoc_insertion_point(class_scope:commonResponse)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr res_msg_;
  bool success_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_proto_2fcommon_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Request

// int32 client_id = 1;
inline void Request::clear_client_id() {
  client_id_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 Request::_internal_client_id() const {
  return client_id_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 Request::client_id() const {
  // @@protoc_insertion_point(field_get:Request.client_id)
  return _internal_client_id();
}
inline void Request::_internal_set_client_id(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  client_id_ = value;
}
inline void Request::set_client_id(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_client_id(value);
  // @@protoc_insertion_point(field_set:Request.client_id)
}

// string req_msg = 2;
inline void Request::clear_req_msg() {
  req_msg_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline const std::string& Request::req_msg() const {
  // @@protoc_insertion_point(field_get:Request.req_msg)
  return _internal_req_msg();
}
inline void Request::set_req_msg(const std::string& value) {
  _internal_set_req_msg(value);
  // @@protoc_insertion_point(field_set:Request.req_msg)
}
inline std::string* Request::mutable_req_msg() {
  // @@protoc_insertion_point(field_mutable:Request.req_msg)
  return _internal_mutable_req_msg();
}
inline const std::string& Request::_internal_req_msg() const {
  return req_msg_.Get();
}
inline void Request::_internal_set_req_msg(const std::string& value) {
  
  req_msg_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value, GetArena());
}
inline void Request::set_req_msg(std::string&& value) {
  
  req_msg_.Set(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:Request.req_msg)
}
inline void Request::set_req_msg(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  req_msg_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value),
              GetArena());
  // @@protoc_insertion_point(field_set_char:Request.req_msg)
}
inline void Request::set_req_msg(const char* value,
    size_t size) {
  
  req_msg_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:Request.req_msg)
}
inline std::string* Request::_internal_mutable_req_msg() {
  
  return req_msg_.Mutable(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline std::string* Request::release_req_msg() {
  // @@protoc_insertion_point(field_release:Request.req_msg)
  return req_msg_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void Request::set_allocated_req_msg(std::string* req_msg) {
  if (req_msg != nullptr) {
    
  } else {
    
  }
  req_msg_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), req_msg,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:Request.req_msg)
}
inline std::string* Request::unsafe_arena_release_req_msg() {
  // @@protoc_insertion_point(field_unsafe_arena_release:Request.req_msg)
  GOOGLE_DCHECK(GetArena() != nullptr);
  
  return req_msg_.UnsafeArenaRelease(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      GetArena());
}
inline void Request::unsafe_arena_set_allocated_req_msg(
    std::string* req_msg) {
  GOOGLE_DCHECK(GetArena() != nullptr);
  if (req_msg != nullptr) {
    
  } else {
    
  }
  req_msg_.UnsafeArenaSetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      req_msg, GetArena());
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:Request.req_msg)
}

// -------------------------------------------------------------------

// commonResponse

// bool success = 1;
inline void commonResponse::clear_success() {
  success_ = false;
}
inline bool commonResponse::_internal_success() const {
  return success_;
}
inline bool commonResponse::success() const {
  // @@protoc_insertion_point(field_get:commonResponse.success)
  return _internal_success();
}
inline void commonResponse::_internal_set_success(bool value) {
  
  success_ = value;
}
inline void commonResponse::set_success(bool value) {
  _internal_set_success(value);
  // @@protoc_insertion_point(field_set:commonResponse.success)
}

// string res_msg = 2;
inline void commonResponse::clear_res_msg() {
  res_msg_.ClearToEmpty(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline const std::string& commonResponse::res_msg() const {
  // @@protoc_insertion_point(field_get:commonResponse.res_msg)
  return _internal_res_msg();
}
inline void commonResponse::set_res_msg(const std::string& value) {
  _internal_set_res_msg(value);
  // @@protoc_insertion_point(field_set:commonResponse.res_msg)
}
inline std::string* commonResponse::mutable_res_msg() {
  // @@protoc_insertion_point(field_mutable:commonResponse.res_msg)
  return _internal_mutable_res_msg();
}
inline const std::string& commonResponse::_internal_res_msg() const {
  return res_msg_.Get();
}
inline void commonResponse::_internal_set_res_msg(const std::string& value) {
  
  res_msg_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value, GetArena());
}
inline void commonResponse::set_res_msg(std::string&& value) {
  
  res_msg_.Set(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:commonResponse.res_msg)
}
inline void commonResponse::set_res_msg(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  res_msg_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value),
              GetArena());
  // @@protoc_insertion_point(field_set_char:commonResponse.res_msg)
}
inline void commonResponse::set_res_msg(const char* value,
    size_t size) {
  
  res_msg_.Set(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:commonResponse.res_msg)
}
inline std::string* commonResponse::_internal_mutable_res_msg() {
  
  return res_msg_.Mutable(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline std::string* commonResponse::release_res_msg() {
  // @@protoc_insertion_point(field_release:commonResponse.res_msg)
  return res_msg_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void commonResponse::set_allocated_res_msg(std::string* res_msg) {
  if (res_msg != nullptr) {
    
  } else {
    
  }
  res_msg_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), res_msg,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:commonResponse.res_msg)
}
inline std::string* commonResponse::unsafe_arena_release_res_msg() {
  // @@protoc_insertion_point(field_unsafe_arena_release:commonResponse.res_msg)
  GOOGLE_DCHECK(GetArena() != nullptr);
  
  return res_msg_.UnsafeArenaRelease(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      GetArena());
}
inline void commonResponse::unsafe_arena_set_allocated_res_msg(
    std::string* res_msg) {
  GOOGLE_DCHECK(GetArena() != nullptr);
  if (res_msg != nullptr) {
    
  } else {
    
  }
  res_msg_.UnsafeArenaSetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      res_msg, GetArena());
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:commonResponse.res_msg)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_proto_2fcommon_2eproto
