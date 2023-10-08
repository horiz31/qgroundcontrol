// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: cotevent.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_cotevent_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_cotevent_2eproto

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
#include "detail.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_cotevent_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_cotevent_2eproto {
  static const uint32_t offsets[];
};
namespace atakmap {
namespace commoncommo {
namespace protobuf {
namespace v1 {
class CotEvent;
struct CotEventDefaultTypeInternal;
extern CotEventDefaultTypeInternal _CotEvent_default_instance_;
}  // namespace v1
}  // namespace protobuf
}  // namespace commoncommo
}  // namespace atakmap
PROTOBUF_NAMESPACE_OPEN
template<> ::atakmap::commoncommo::protobuf::v1::CotEvent* Arena::CreateMaybeMessage<::atakmap::commoncommo::protobuf::v1::CotEvent>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace atakmap {
namespace commoncommo {
namespace protobuf {
namespace v1 {

// ===================================================================

class CotEvent final :
    public ::PROTOBUF_NAMESPACE_ID::MessageLite /* @@protoc_insertion_point(class_definition:atakmap.commoncommo.protobuf.v1.CotEvent) */ {
 public:
  inline CotEvent() : CotEvent(nullptr) {}
  ~CotEvent() override;
  explicit PROTOBUF_CONSTEXPR CotEvent(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  CotEvent(const CotEvent& from);
  CotEvent(CotEvent&& from) noexcept
    : CotEvent() {
    *this = ::std::move(from);
  }

  inline CotEvent& operator=(const CotEvent& from) {
    CopyFrom(from);
    return *this;
  }
  inline CotEvent& operator=(CotEvent&& from) noexcept {
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

  static const CotEvent& default_instance() {
    return *internal_default_instance();
  }
  static inline const CotEvent* internal_default_instance() {
    return reinterpret_cast<const CotEvent*>(
               &_CotEvent_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(CotEvent& a, CotEvent& b) {
    a.Swap(&b);
  }
  inline void Swap(CotEvent* other) {
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
  void UnsafeArenaSwap(CotEvent* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  CotEvent* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<CotEvent>(arena);
  }
  void CheckTypeAndMergeFrom(const ::PROTOBUF_NAMESPACE_ID::MessageLite& from)  final;
  void CopyFrom(const CotEvent& from);
  void MergeFrom(const CotEvent& from);
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
  void InternalSwap(CotEvent* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "atakmap.commoncommo.protobuf.v1.CotEvent";
  }
  protected:
  explicit CotEvent(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  std::string GetTypeName() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kTypeFieldNumber = 1,
    kAccessFieldNumber = 2,
    kQosFieldNumber = 3,
    kOpexFieldNumber = 4,
    kUidFieldNumber = 5,
    kHowFieldNumber = 9,
    kDetailFieldNumber = 15,
    kSendTimeFieldNumber = 6,
    kStartTimeFieldNumber = 7,
    kStaleTimeFieldNumber = 8,
    kLatFieldNumber = 10,
    kLonFieldNumber = 11,
    kHaeFieldNumber = 12,
    kCeFieldNumber = 13,
    kLeFieldNumber = 14,
  };
  // string type = 1;
  void clear_type();
  const std::string& type() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_type(ArgT0&& arg0, ArgT... args);
  std::string* mutable_type();
  PROTOBUF_NODISCARD std::string* release_type();
  void set_allocated_type(std::string* type);
  private:
  const std::string& _internal_type() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_type(const std::string& value);
  std::string* _internal_mutable_type();
  public:

  // string access = 2;
  void clear_access();
  const std::string& access() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_access(ArgT0&& arg0, ArgT... args);
  std::string* mutable_access();
  PROTOBUF_NODISCARD std::string* release_access();
  void set_allocated_access(std::string* access);
  private:
  const std::string& _internal_access() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_access(const std::string& value);
  std::string* _internal_mutable_access();
  public:

  // string qos = 3;
  void clear_qos();
  const std::string& qos() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_qos(ArgT0&& arg0, ArgT... args);
  std::string* mutable_qos();
  PROTOBUF_NODISCARD std::string* release_qos();
  void set_allocated_qos(std::string* qos);
  private:
  const std::string& _internal_qos() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_qos(const std::string& value);
  std::string* _internal_mutable_qos();
  public:

  // string opex = 4;
  void clear_opex();
  const std::string& opex() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_opex(ArgT0&& arg0, ArgT... args);
  std::string* mutable_opex();
  PROTOBUF_NODISCARD std::string* release_opex();
  void set_allocated_opex(std::string* opex);
  private:
  const std::string& _internal_opex() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_opex(const std::string& value);
  std::string* _internal_mutable_opex();
  public:

  // string uid = 5;
  void clear_uid();
  const std::string& uid() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_uid(ArgT0&& arg0, ArgT... args);
  std::string* mutable_uid();
  PROTOBUF_NODISCARD std::string* release_uid();
  void set_allocated_uid(std::string* uid);
  private:
  const std::string& _internal_uid() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_uid(const std::string& value);
  std::string* _internal_mutable_uid();
  public:

  // string how = 9;
  void clear_how();
  const std::string& how() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_how(ArgT0&& arg0, ArgT... args);
  std::string* mutable_how();
  PROTOBUF_NODISCARD std::string* release_how();
  void set_allocated_how(std::string* how);
  private:
  const std::string& _internal_how() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_how(const std::string& value);
  std::string* _internal_mutable_how();
  public:

  // .atakmap.commoncommo.protobuf.v1.Detail detail = 15;
  bool has_detail() const;
  private:
  bool _internal_has_detail() const;
  public:
  void clear_detail();
  const ::atakmap::commoncommo::protobuf::v1::Detail& detail() const;
  PROTOBUF_NODISCARD ::atakmap::commoncommo::protobuf::v1::Detail* release_detail();
  ::atakmap::commoncommo::protobuf::v1::Detail* mutable_detail();
  void set_allocated_detail(::atakmap::commoncommo::protobuf::v1::Detail* detail);
  private:
  const ::atakmap::commoncommo::protobuf::v1::Detail& _internal_detail() const;
  ::atakmap::commoncommo::protobuf::v1::Detail* _internal_mutable_detail();
  public:
  void unsafe_arena_set_allocated_detail(
      ::atakmap::commoncommo::protobuf::v1::Detail* detail);
  ::atakmap::commoncommo::protobuf::v1::Detail* unsafe_arena_release_detail();

  // uint64 sendTime = 6;
  void clear_sendtime();
  uint64_t sendtime() const;
  void set_sendtime(uint64_t value);
  private:
  uint64_t _internal_sendtime() const;
  void _internal_set_sendtime(uint64_t value);
  public:

  // uint64 startTime = 7;
  void clear_starttime();
  uint64_t starttime() const;
  void set_starttime(uint64_t value);
  private:
  uint64_t _internal_starttime() const;
  void _internal_set_starttime(uint64_t value);
  public:

  // uint64 staleTime = 8;
  void clear_staletime();
  uint64_t staletime() const;
  void set_staletime(uint64_t value);
  private:
  uint64_t _internal_staletime() const;
  void _internal_set_staletime(uint64_t value);
  public:

  // double lat = 10;
  void clear_lat();
  double lat() const;
  void set_lat(double value);
  private:
  double _internal_lat() const;
  void _internal_set_lat(double value);
  public:

  // double lon = 11;
  void clear_lon();
  double lon() const;
  void set_lon(double value);
  private:
  double _internal_lon() const;
  void _internal_set_lon(double value);
  public:

  // double hae = 12;
  void clear_hae();
  double hae() const;
  void set_hae(double value);
  private:
  double _internal_hae() const;
  void _internal_set_hae(double value);
  public:

  // double ce = 13;
  void clear_ce();
  double ce() const;
  void set_ce(double value);
  private:
  double _internal_ce() const;
  void _internal_set_ce(double value);
  public:

  // double le = 14;
  void clear_le();
  double le() const;
  void set_le(double value);
  private:
  double _internal_le() const;
  void _internal_set_le(double value);
  public:

  // @@protoc_insertion_point(class_scope:atakmap.commoncommo.protobuf.v1.CotEvent)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr type_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr access_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr qos_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr opex_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr uid_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr how_;
    ::atakmap::commoncommo::protobuf::v1::Detail* detail_;
    uint64_t sendtime_;
    uint64_t starttime_;
    uint64_t staletime_;
    double lat_;
    double lon_;
    double hae_;
    double ce_;
    double le_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_cotevent_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// CotEvent

// string type = 1;
inline void CotEvent::clear_type() {
  _impl_.type_.ClearToEmpty();
}
inline const std::string& CotEvent::type() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.CotEvent.type)
  return _internal_type();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void CotEvent::set_type(ArgT0&& arg0, ArgT... args) {
 
 _impl_.type_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.CotEvent.type)
}
inline std::string* CotEvent::mutable_type() {
  std::string* _s = _internal_mutable_type();
  // @@protoc_insertion_point(field_mutable:atakmap.commoncommo.protobuf.v1.CotEvent.type)
  return _s;
}
inline const std::string& CotEvent::_internal_type() const {
  return _impl_.type_.Get();
}
inline void CotEvent::_internal_set_type(const std::string& value) {
  
  _impl_.type_.Set(value, GetArenaForAllocation());
}
inline std::string* CotEvent::_internal_mutable_type() {
  
  return _impl_.type_.Mutable(GetArenaForAllocation());
}
inline std::string* CotEvent::release_type() {
  // @@protoc_insertion_point(field_release:atakmap.commoncommo.protobuf.v1.CotEvent.type)
  return _impl_.type_.Release();
}
inline void CotEvent::set_allocated_type(std::string* type) {
  if (type != nullptr) {
    
  } else {
    
  }
  _impl_.type_.SetAllocated(type, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.type_.IsDefault()) {
    _impl_.type_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:atakmap.commoncommo.protobuf.v1.CotEvent.type)
}

// string access = 2;
inline void CotEvent::clear_access() {
  _impl_.access_.ClearToEmpty();
}
inline const std::string& CotEvent::access() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.CotEvent.access)
  return _internal_access();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void CotEvent::set_access(ArgT0&& arg0, ArgT... args) {
 
 _impl_.access_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.CotEvent.access)
}
inline std::string* CotEvent::mutable_access() {
  std::string* _s = _internal_mutable_access();
  // @@protoc_insertion_point(field_mutable:atakmap.commoncommo.protobuf.v1.CotEvent.access)
  return _s;
}
inline const std::string& CotEvent::_internal_access() const {
  return _impl_.access_.Get();
}
inline void CotEvent::_internal_set_access(const std::string& value) {
  
  _impl_.access_.Set(value, GetArenaForAllocation());
}
inline std::string* CotEvent::_internal_mutable_access() {
  
  return _impl_.access_.Mutable(GetArenaForAllocation());
}
inline std::string* CotEvent::release_access() {
  // @@protoc_insertion_point(field_release:atakmap.commoncommo.protobuf.v1.CotEvent.access)
  return _impl_.access_.Release();
}
inline void CotEvent::set_allocated_access(std::string* access) {
  if (access != nullptr) {
    
  } else {
    
  }
  _impl_.access_.SetAllocated(access, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.access_.IsDefault()) {
    _impl_.access_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:atakmap.commoncommo.protobuf.v1.CotEvent.access)
}

// string qos = 3;
inline void CotEvent::clear_qos() {
  _impl_.qos_.ClearToEmpty();
}
inline const std::string& CotEvent::qos() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.CotEvent.qos)
  return _internal_qos();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void CotEvent::set_qos(ArgT0&& arg0, ArgT... args) {
 
 _impl_.qos_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.CotEvent.qos)
}
inline std::string* CotEvent::mutable_qos() {
  std::string* _s = _internal_mutable_qos();
  // @@protoc_insertion_point(field_mutable:atakmap.commoncommo.protobuf.v1.CotEvent.qos)
  return _s;
}
inline const std::string& CotEvent::_internal_qos() const {
  return _impl_.qos_.Get();
}
inline void CotEvent::_internal_set_qos(const std::string& value) {
  
  _impl_.qos_.Set(value, GetArenaForAllocation());
}
inline std::string* CotEvent::_internal_mutable_qos() {
  
  return _impl_.qos_.Mutable(GetArenaForAllocation());
}
inline std::string* CotEvent::release_qos() {
  // @@protoc_insertion_point(field_release:atakmap.commoncommo.protobuf.v1.CotEvent.qos)
  return _impl_.qos_.Release();
}
inline void CotEvent::set_allocated_qos(std::string* qos) {
  if (qos != nullptr) {
    
  } else {
    
  }
  _impl_.qos_.SetAllocated(qos, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.qos_.IsDefault()) {
    _impl_.qos_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:atakmap.commoncommo.protobuf.v1.CotEvent.qos)
}

// string opex = 4;
inline void CotEvent::clear_opex() {
  _impl_.opex_.ClearToEmpty();
}
inline const std::string& CotEvent::opex() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.CotEvent.opex)
  return _internal_opex();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void CotEvent::set_opex(ArgT0&& arg0, ArgT... args) {
 
 _impl_.opex_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.CotEvent.opex)
}
inline std::string* CotEvent::mutable_opex() {
  std::string* _s = _internal_mutable_opex();
  // @@protoc_insertion_point(field_mutable:atakmap.commoncommo.protobuf.v1.CotEvent.opex)
  return _s;
}
inline const std::string& CotEvent::_internal_opex() const {
  return _impl_.opex_.Get();
}
inline void CotEvent::_internal_set_opex(const std::string& value) {
  
  _impl_.opex_.Set(value, GetArenaForAllocation());
}
inline std::string* CotEvent::_internal_mutable_opex() {
  
  return _impl_.opex_.Mutable(GetArenaForAllocation());
}
inline std::string* CotEvent::release_opex() {
  // @@protoc_insertion_point(field_release:atakmap.commoncommo.protobuf.v1.CotEvent.opex)
  return _impl_.opex_.Release();
}
inline void CotEvent::set_allocated_opex(std::string* opex) {
  if (opex != nullptr) {
    
  } else {
    
  }
  _impl_.opex_.SetAllocated(opex, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.opex_.IsDefault()) {
    _impl_.opex_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:atakmap.commoncommo.protobuf.v1.CotEvent.opex)
}

// string uid = 5;
inline void CotEvent::clear_uid() {
  _impl_.uid_.ClearToEmpty();
}
inline const std::string& CotEvent::uid() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.CotEvent.uid)
  return _internal_uid();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void CotEvent::set_uid(ArgT0&& arg0, ArgT... args) {
 
 _impl_.uid_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.CotEvent.uid)
}
inline std::string* CotEvent::mutable_uid() {
  std::string* _s = _internal_mutable_uid();
  // @@protoc_insertion_point(field_mutable:atakmap.commoncommo.protobuf.v1.CotEvent.uid)
  return _s;
}
inline const std::string& CotEvent::_internal_uid() const {
  return _impl_.uid_.Get();
}
inline void CotEvent::_internal_set_uid(const std::string& value) {
  
  _impl_.uid_.Set(value, GetArenaForAllocation());
}
inline std::string* CotEvent::_internal_mutable_uid() {
  
  return _impl_.uid_.Mutable(GetArenaForAllocation());
}
inline std::string* CotEvent::release_uid() {
  // @@protoc_insertion_point(field_release:atakmap.commoncommo.protobuf.v1.CotEvent.uid)
  return _impl_.uid_.Release();
}
inline void CotEvent::set_allocated_uid(std::string* uid) {
  if (uid != nullptr) {
    
  } else {
    
  }
  _impl_.uid_.SetAllocated(uid, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.uid_.IsDefault()) {
    _impl_.uid_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:atakmap.commoncommo.protobuf.v1.CotEvent.uid)
}

// uint64 sendTime = 6;
inline void CotEvent::clear_sendtime() {
  _impl_.sendtime_ = uint64_t{0u};
}
inline uint64_t CotEvent::_internal_sendtime() const {
  return _impl_.sendtime_;
}
inline uint64_t CotEvent::sendtime() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.CotEvent.sendTime)
  return _internal_sendtime();
}
inline void CotEvent::_internal_set_sendtime(uint64_t value) {
  
  _impl_.sendtime_ = value;
}
inline void CotEvent::set_sendtime(uint64_t value) {
  _internal_set_sendtime(value);
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.CotEvent.sendTime)
}

// uint64 startTime = 7;
inline void CotEvent::clear_starttime() {
  _impl_.starttime_ = uint64_t{0u};
}
inline uint64_t CotEvent::_internal_starttime() const {
  return _impl_.starttime_;
}
inline uint64_t CotEvent::starttime() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.CotEvent.startTime)
  return _internal_starttime();
}
inline void CotEvent::_internal_set_starttime(uint64_t value) {
  
  _impl_.starttime_ = value;
}
inline void CotEvent::set_starttime(uint64_t value) {
  _internal_set_starttime(value);
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.CotEvent.startTime)
}

// uint64 staleTime = 8;
inline void CotEvent::clear_staletime() {
  _impl_.staletime_ = uint64_t{0u};
}
inline uint64_t CotEvent::_internal_staletime() const {
  return _impl_.staletime_;
}
inline uint64_t CotEvent::staletime() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.CotEvent.staleTime)
  return _internal_staletime();
}
inline void CotEvent::_internal_set_staletime(uint64_t value) {
  
  _impl_.staletime_ = value;
}
inline void CotEvent::set_staletime(uint64_t value) {
  _internal_set_staletime(value);
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.CotEvent.staleTime)
}

// string how = 9;
inline void CotEvent::clear_how() {
  _impl_.how_.ClearToEmpty();
}
inline const std::string& CotEvent::how() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.CotEvent.how)
  return _internal_how();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void CotEvent::set_how(ArgT0&& arg0, ArgT... args) {
 
 _impl_.how_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.CotEvent.how)
}
inline std::string* CotEvent::mutable_how() {
  std::string* _s = _internal_mutable_how();
  // @@protoc_insertion_point(field_mutable:atakmap.commoncommo.protobuf.v1.CotEvent.how)
  return _s;
}
inline const std::string& CotEvent::_internal_how() const {
  return _impl_.how_.Get();
}
inline void CotEvent::_internal_set_how(const std::string& value) {
  
  _impl_.how_.Set(value, GetArenaForAllocation());
}
inline std::string* CotEvent::_internal_mutable_how() {
  
  return _impl_.how_.Mutable(GetArenaForAllocation());
}
inline std::string* CotEvent::release_how() {
  // @@protoc_insertion_point(field_release:atakmap.commoncommo.protobuf.v1.CotEvent.how)
  return _impl_.how_.Release();
}
inline void CotEvent::set_allocated_how(std::string* how) {
  if (how != nullptr) {
    
  } else {
    
  }
  _impl_.how_.SetAllocated(how, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.how_.IsDefault()) {
    _impl_.how_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:atakmap.commoncommo.protobuf.v1.CotEvent.how)
}

// double lat = 10;
inline void CotEvent::clear_lat() {
  _impl_.lat_ = 0;
}
inline double CotEvent::_internal_lat() const {
  return _impl_.lat_;
}
inline double CotEvent::lat() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.CotEvent.lat)
  return _internal_lat();
}
inline void CotEvent::_internal_set_lat(double value) {
  
  _impl_.lat_ = value;
}
inline void CotEvent::set_lat(double value) {
  _internal_set_lat(value);
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.CotEvent.lat)
}

// double lon = 11;
inline void CotEvent::clear_lon() {
  _impl_.lon_ = 0;
}
inline double CotEvent::_internal_lon() const {
  return _impl_.lon_;
}
inline double CotEvent::lon() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.CotEvent.lon)
  return _internal_lon();
}
inline void CotEvent::_internal_set_lon(double value) {
  
  _impl_.lon_ = value;
}
inline void CotEvent::set_lon(double value) {
  _internal_set_lon(value);
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.CotEvent.lon)
}

// double hae = 12;
inline void CotEvent::clear_hae() {
  _impl_.hae_ = 0;
}
inline double CotEvent::_internal_hae() const {
  return _impl_.hae_;
}
inline double CotEvent::hae() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.CotEvent.hae)
  return _internal_hae();
}
inline void CotEvent::_internal_set_hae(double value) {
  
  _impl_.hae_ = value;
}
inline void CotEvent::set_hae(double value) {
  _internal_set_hae(value);
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.CotEvent.hae)
}

// double ce = 13;
inline void CotEvent::clear_ce() {
  _impl_.ce_ = 0;
}
inline double CotEvent::_internal_ce() const {
  return _impl_.ce_;
}
inline double CotEvent::ce() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.CotEvent.ce)
  return _internal_ce();
}
inline void CotEvent::_internal_set_ce(double value) {
  
  _impl_.ce_ = value;
}
inline void CotEvent::set_ce(double value) {
  _internal_set_ce(value);
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.CotEvent.ce)
}

// double le = 14;
inline void CotEvent::clear_le() {
  _impl_.le_ = 0;
}
inline double CotEvent::_internal_le() const {
  return _impl_.le_;
}
inline double CotEvent::le() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.CotEvent.le)
  return _internal_le();
}
inline void CotEvent::_internal_set_le(double value) {
  
  _impl_.le_ = value;
}
inline void CotEvent::set_le(double value) {
  _internal_set_le(value);
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.CotEvent.le)
}

// .atakmap.commoncommo.protobuf.v1.Detail detail = 15;
inline bool CotEvent::_internal_has_detail() const {
  return this != internal_default_instance() && _impl_.detail_ != nullptr;
}
inline bool CotEvent::has_detail() const {
  return _internal_has_detail();
}
inline const ::atakmap::commoncommo::protobuf::v1::Detail& CotEvent::_internal_detail() const {
  const ::atakmap::commoncommo::protobuf::v1::Detail* p = _impl_.detail_;
  return p != nullptr ? *p : reinterpret_cast<const ::atakmap::commoncommo::protobuf::v1::Detail&>(
      ::atakmap::commoncommo::protobuf::v1::_Detail_default_instance_);
}
inline const ::atakmap::commoncommo::protobuf::v1::Detail& CotEvent::detail() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.CotEvent.detail)
  return _internal_detail();
}
inline void CotEvent::unsafe_arena_set_allocated_detail(
    ::atakmap::commoncommo::protobuf::v1::Detail* detail) {
  if (GetArenaForAllocation() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.detail_);
  }
  _impl_.detail_ = detail;
  if (detail) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:atakmap.commoncommo.protobuf.v1.CotEvent.detail)
}
inline ::atakmap::commoncommo::protobuf::v1::Detail* CotEvent::release_detail() {
  
  ::atakmap::commoncommo::protobuf::v1::Detail* temp = _impl_.detail_;
  _impl_.detail_ = nullptr;
#ifdef PROTOBUF_FORCE_COPY_IN_RELEASE
  auto* old =  reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(temp);
  temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  if (GetArenaForAllocation() == nullptr) { delete old; }
#else  // PROTOBUF_FORCE_COPY_IN_RELEASE
  if (GetArenaForAllocation() != nullptr) {
    temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  }
#endif  // !PROTOBUF_FORCE_COPY_IN_RELEASE
  return temp;
}
inline ::atakmap::commoncommo::protobuf::v1::Detail* CotEvent::unsafe_arena_release_detail() {
  // @@protoc_insertion_point(field_release:atakmap.commoncommo.protobuf.v1.CotEvent.detail)
  
  ::atakmap::commoncommo::protobuf::v1::Detail* temp = _impl_.detail_;
  _impl_.detail_ = nullptr;
  return temp;
}
inline ::atakmap::commoncommo::protobuf::v1::Detail* CotEvent::_internal_mutable_detail() {
  
  if (_impl_.detail_ == nullptr) {
    auto* p = CreateMaybeMessage<::atakmap::commoncommo::protobuf::v1::Detail>(GetArenaForAllocation());
    _impl_.detail_ = p;
  }
  return _impl_.detail_;
}
inline ::atakmap::commoncommo::protobuf::v1::Detail* CotEvent::mutable_detail() {
  ::atakmap::commoncommo::protobuf::v1::Detail* _msg = _internal_mutable_detail();
  // @@protoc_insertion_point(field_mutable:atakmap.commoncommo.protobuf.v1.CotEvent.detail)
  return _msg;
}
inline void CotEvent::set_allocated_detail(::atakmap::commoncommo::protobuf::v1::Detail* detail) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.detail_);
  }
  if (detail) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
        ::PROTOBUF_NAMESPACE_ID::Arena::InternalGetOwningArena(
                reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(detail));
    if (message_arena != submessage_arena) {
      detail = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, detail, submessage_arena);
    }
    
  } else {
    
  }
  _impl_.detail_ = detail;
  // @@protoc_insertion_point(field_set_allocated:atakmap.commoncommo.protobuf.v1.CotEvent.detail)
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
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_cotevent_2eproto
