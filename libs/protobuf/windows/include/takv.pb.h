// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: takv.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_takv_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_takv_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_takv_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_takv_2eproto {
  static const uint32_t offsets[];
};
namespace atakmap {
namespace commoncommo {
namespace protobuf {
namespace v1 {
class Takv;
struct TakvDefaultTypeInternal;
extern TakvDefaultTypeInternal _Takv_default_instance_;
}  // namespace v1
}  // namespace protobuf
}  // namespace commoncommo
}  // namespace atakmap
PROTOBUF_NAMESPACE_OPEN
template<> ::atakmap::commoncommo::protobuf::v1::Takv* Arena::CreateMaybeMessage<::atakmap::commoncommo::protobuf::v1::Takv>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace atakmap {
namespace commoncommo {
namespace protobuf {
namespace v1 {

// ===================================================================

class Takv final :
    public ::PROTOBUF_NAMESPACE_ID::MessageLite /* @@protoc_insertion_point(class_definition:atakmap.commoncommo.protobuf.v1.Takv) */ {
 public:
  inline Takv() : Takv(nullptr) {}
  ~Takv() override;
  explicit PROTOBUF_CONSTEXPR Takv(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Takv(const Takv& from);
  Takv(Takv&& from) noexcept
    : Takv() {
    *this = ::std::move(from);
  }

  inline Takv& operator=(const Takv& from) {
    CopyFrom(from);
    return *this;
  }
  inline Takv& operator=(Takv&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const Takv& default_instance() {
    return *internal_default_instance();
  }
  static inline const Takv* internal_default_instance() {
    return reinterpret_cast<const Takv*>(
               &_Takv_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Takv& a, Takv& b) {
    a.Swap(&b);
  }
  inline void Swap(Takv* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Takv* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Takv* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Takv>(arena);
  }
  void CheckTypeAndMergeFrom(const ::PROTOBUF_NAMESPACE_ID::MessageLite& from)  final;
  void CopyFrom(const Takv& from);
  void MergeFrom(const Takv& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(Takv* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "atakmap.commoncommo.protobuf.v1.Takv";
  }
  protected:
  explicit Takv(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  std::string GetTypeName() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kDeviceFieldNumber = 1,
    kPlatformFieldNumber = 2,
    kOsFieldNumber = 3,
    kVersionFieldNumber = 4,
  };
  // string device = 1;
  void clear_device();
  const std::string& device() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_device(ArgT0&& arg0, ArgT... args);
  std::string* mutable_device();
  PROTOBUF_NODISCARD std::string* release_device();
  void set_allocated_device(std::string* device);
  private:
  const std::string& _internal_device() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_device(const std::string& value);
  std::string* _internal_mutable_device();
  public:

  // string platform = 2;
  void clear_platform();
  const std::string& platform() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_platform(ArgT0&& arg0, ArgT... args);
  std::string* mutable_platform();
  PROTOBUF_NODISCARD std::string* release_platform();
  void set_allocated_platform(std::string* platform);
  private:
  const std::string& _internal_platform() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_platform(const std::string& value);
  std::string* _internal_mutable_platform();
  public:

  // string os = 3;
  void clear_os();
  const std::string& os() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_os(ArgT0&& arg0, ArgT... args);
  std::string* mutable_os();
  PROTOBUF_NODISCARD std::string* release_os();
  void set_allocated_os(std::string* os);
  private:
  const std::string& _internal_os() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_os(const std::string& value);
  std::string* _internal_mutable_os();
  public:

  // string version = 4;
  void clear_version();
  const std::string& version() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_version(ArgT0&& arg0, ArgT... args);
  std::string* mutable_version();
  PROTOBUF_NODISCARD std::string* release_version();
  void set_allocated_version(std::string* version);
  private:
  const std::string& _internal_version() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_version(const std::string& value);
  std::string* _internal_mutable_version();
  public:

  // @@protoc_insertion_point(class_scope:atakmap.commoncommo.protobuf.v1.Takv)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr device_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr platform_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr os_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr version_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_takv_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Takv

// string device = 1;
inline void Takv::clear_device() {
  _impl_.device_.ClearToEmpty();
}
inline const std::string& Takv::device() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.Takv.device)
  return _internal_device();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Takv::set_device(ArgT0&& arg0, ArgT... args) {
 
 _impl_.device_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.Takv.device)
}
inline std::string* Takv::mutable_device() {
  std::string* _s = _internal_mutable_device();
  // @@protoc_insertion_point(field_mutable:atakmap.commoncommo.protobuf.v1.Takv.device)
  return _s;
}
inline const std::string& Takv::_internal_device() const {
  return _impl_.device_.Get();
}
inline void Takv::_internal_set_device(const std::string& value) {
  
  _impl_.device_.Set(value, GetArenaForAllocation());
}
inline std::string* Takv::_internal_mutable_device() {
  
  return _impl_.device_.Mutable(GetArenaForAllocation());
}
inline std::string* Takv::release_device() {
  // @@protoc_insertion_point(field_release:atakmap.commoncommo.protobuf.v1.Takv.device)
  return _impl_.device_.Release();
}
inline void Takv::set_allocated_device(std::string* device) {
  if (device != nullptr) {
    
  } else {
    
  }
  _impl_.device_.SetAllocated(device, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.device_.IsDefault()) {
    _impl_.device_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:atakmap.commoncommo.protobuf.v1.Takv.device)
}

// string platform = 2;
inline void Takv::clear_platform() {
  _impl_.platform_.ClearToEmpty();
}
inline const std::string& Takv::platform() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.Takv.platform)
  return _internal_platform();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Takv::set_platform(ArgT0&& arg0, ArgT... args) {
 
 _impl_.platform_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.Takv.platform)
}
inline std::string* Takv::mutable_platform() {
  std::string* _s = _internal_mutable_platform();
  // @@protoc_insertion_point(field_mutable:atakmap.commoncommo.protobuf.v1.Takv.platform)
  return _s;
}
inline const std::string& Takv::_internal_platform() const {
  return _impl_.platform_.Get();
}
inline void Takv::_internal_set_platform(const std::string& value) {
  
  _impl_.platform_.Set(value, GetArenaForAllocation());
}
inline std::string* Takv::_internal_mutable_platform() {
  
  return _impl_.platform_.Mutable(GetArenaForAllocation());
}
inline std::string* Takv::release_platform() {
  // @@protoc_insertion_point(field_release:atakmap.commoncommo.protobuf.v1.Takv.platform)
  return _impl_.platform_.Release();
}
inline void Takv::set_allocated_platform(std::string* platform) {
  if (platform != nullptr) {
    
  } else {
    
  }
  _impl_.platform_.SetAllocated(platform, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.platform_.IsDefault()) {
    _impl_.platform_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:atakmap.commoncommo.protobuf.v1.Takv.platform)
}

// string os = 3;
inline void Takv::clear_os() {
  _impl_.os_.ClearToEmpty();
}
inline const std::string& Takv::os() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.Takv.os)
  return _internal_os();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Takv::set_os(ArgT0&& arg0, ArgT... args) {
 
 _impl_.os_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.Takv.os)
}
inline std::string* Takv::mutable_os() {
  std::string* _s = _internal_mutable_os();
  // @@protoc_insertion_point(field_mutable:atakmap.commoncommo.protobuf.v1.Takv.os)
  return _s;
}
inline const std::string& Takv::_internal_os() const {
  return _impl_.os_.Get();
}
inline void Takv::_internal_set_os(const std::string& value) {
  
  _impl_.os_.Set(value, GetArenaForAllocation());
}
inline std::string* Takv::_internal_mutable_os() {
  
  return _impl_.os_.Mutable(GetArenaForAllocation());
}
inline std::string* Takv::release_os() {
  // @@protoc_insertion_point(field_release:atakmap.commoncommo.protobuf.v1.Takv.os)
  return _impl_.os_.Release();
}
inline void Takv::set_allocated_os(std::string* os) {
  if (os != nullptr) {
    
  } else {
    
  }
  _impl_.os_.SetAllocated(os, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.os_.IsDefault()) {
    _impl_.os_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:atakmap.commoncommo.protobuf.v1.Takv.os)
}

// string version = 4;
inline void Takv::clear_version() {
  _impl_.version_.ClearToEmpty();
}
inline const std::string& Takv::version() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.Takv.version)
  return _internal_version();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Takv::set_version(ArgT0&& arg0, ArgT... args) {
 
 _impl_.version_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.Takv.version)
}
inline std::string* Takv::mutable_version() {
  std::string* _s = _internal_mutable_version();
  // @@protoc_insertion_point(field_mutable:atakmap.commoncommo.protobuf.v1.Takv.version)
  return _s;
}
inline const std::string& Takv::_internal_version() const {
  return _impl_.version_.Get();
}
inline void Takv::_internal_set_version(const std::string& value) {
  
  _impl_.version_.Set(value, GetArenaForAllocation());
}
inline std::string* Takv::_internal_mutable_version() {
  
  return _impl_.version_.Mutable(GetArenaForAllocation());
}
inline std::string* Takv::release_version() {
  // @@protoc_insertion_point(field_release:atakmap.commoncommo.protobuf.v1.Takv.version)
  return _impl_.version_.Release();
}
inline void Takv::set_allocated_version(std::string* version) {
  if (version != nullptr) {
    
  } else {
    
  }
  _impl_.version_.SetAllocated(version, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.version_.IsDefault()) {
    _impl_.version_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:atakmap.commoncommo.protobuf.v1.Takv.version)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace v1
}  // namespace protobuf
}  // namespace commoncommo
}  // namespace atakmap

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_takv_2eproto
