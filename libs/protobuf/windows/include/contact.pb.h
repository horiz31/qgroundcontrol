// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: contact.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_contact_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_contact_2eproto

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
#define PROTOBUF_INTERNAL_EXPORT_contact_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_contact_2eproto {
  static const uint32_t offsets[];
};
namespace atakmap {
namespace commoncommo {
namespace protobuf {
namespace v1 {
class Contact;
struct ContactDefaultTypeInternal;
extern ContactDefaultTypeInternal _Contact_default_instance_;
}  // namespace v1
}  // namespace protobuf
}  // namespace commoncommo
}  // namespace atakmap
PROTOBUF_NAMESPACE_OPEN
template<> ::atakmap::commoncommo::protobuf::v1::Contact* Arena::CreateMaybeMessage<::atakmap::commoncommo::protobuf::v1::Contact>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace atakmap {
namespace commoncommo {
namespace protobuf {
namespace v1 {

// ===================================================================

class Contact final :
    public ::PROTOBUF_NAMESPACE_ID::MessageLite /* @@protoc_insertion_point(class_definition:atakmap.commoncommo.protobuf.v1.Contact) */ {
 public:
  inline Contact() : Contact(nullptr) {}
  ~Contact() override;
  explicit PROTOBUF_CONSTEXPR Contact(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Contact(const Contact& from);
  Contact(Contact&& from) noexcept
    : Contact() {
    *this = ::std::move(from);
  }

  inline Contact& operator=(const Contact& from) {
    CopyFrom(from);
    return *this;
  }
  inline Contact& operator=(Contact&& from) noexcept {
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

  static const Contact& default_instance() {
    return *internal_default_instance();
  }
  static inline const Contact* internal_default_instance() {
    return reinterpret_cast<const Contact*>(
               &_Contact_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Contact& a, Contact& b) {
    a.Swap(&b);
  }
  inline void Swap(Contact* other) {
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
  void UnsafeArenaSwap(Contact* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Contact* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Contact>(arena);
  }
  void CheckTypeAndMergeFrom(const ::PROTOBUF_NAMESPACE_ID::MessageLite& from)  final;
  void CopyFrom(const Contact& from);
  void MergeFrom(const Contact& from);
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
  void InternalSwap(Contact* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "atakmap.commoncommo.protobuf.v1.Contact";
  }
  protected:
  explicit Contact(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  std::string GetTypeName() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kEndpointFieldNumber = 1,
    kCallsignFieldNumber = 2,
  };
  // string endpoint = 1;
  void clear_endpoint();
  const std::string& endpoint() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_endpoint(ArgT0&& arg0, ArgT... args);
  std::string* mutable_endpoint();
  PROTOBUF_NODISCARD std::string* release_endpoint();
  void set_allocated_endpoint(std::string* endpoint);
  private:
  const std::string& _internal_endpoint() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_endpoint(const std::string& value);
  std::string* _internal_mutable_endpoint();
  public:

  // string callsign = 2;
  void clear_callsign();
  const std::string& callsign() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_callsign(ArgT0&& arg0, ArgT... args);
  std::string* mutable_callsign();
  PROTOBUF_NODISCARD std::string* release_callsign();
  void set_allocated_callsign(std::string* callsign);
  private:
  const std::string& _internal_callsign() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_callsign(const std::string& value);
  std::string* _internal_mutable_callsign();
  public:

  // @@protoc_insertion_point(class_scope:atakmap.commoncommo.protobuf.v1.Contact)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr endpoint_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr callsign_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_contact_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Contact

// string endpoint = 1;
inline void Contact::clear_endpoint() {
  _impl_.endpoint_.ClearToEmpty();
}
inline const std::string& Contact::endpoint() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.Contact.endpoint)
  return _internal_endpoint();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Contact::set_endpoint(ArgT0&& arg0, ArgT... args) {
 
 _impl_.endpoint_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.Contact.endpoint)
}
inline std::string* Contact::mutable_endpoint() {
  std::string* _s = _internal_mutable_endpoint();
  // @@protoc_insertion_point(field_mutable:atakmap.commoncommo.protobuf.v1.Contact.endpoint)
  return _s;
}
inline const std::string& Contact::_internal_endpoint() const {
  return _impl_.endpoint_.Get();
}
inline void Contact::_internal_set_endpoint(const std::string& value) {
  
  _impl_.endpoint_.Set(value, GetArenaForAllocation());
}
inline std::string* Contact::_internal_mutable_endpoint() {
  
  return _impl_.endpoint_.Mutable(GetArenaForAllocation());
}
inline std::string* Contact::release_endpoint() {
  // @@protoc_insertion_point(field_release:atakmap.commoncommo.protobuf.v1.Contact.endpoint)
  return _impl_.endpoint_.Release();
}
inline void Contact::set_allocated_endpoint(std::string* endpoint) {
  if (endpoint != nullptr) {
    
  } else {
    
  }
  _impl_.endpoint_.SetAllocated(endpoint, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.endpoint_.IsDefault()) {
    _impl_.endpoint_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:atakmap.commoncommo.protobuf.v1.Contact.endpoint)
}

// string callsign = 2;
inline void Contact::clear_callsign() {
  _impl_.callsign_.ClearToEmpty();
}
inline const std::string& Contact::callsign() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.Contact.callsign)
  return _internal_callsign();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Contact::set_callsign(ArgT0&& arg0, ArgT... args) {
 
 _impl_.callsign_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.Contact.callsign)
}
inline std::string* Contact::mutable_callsign() {
  std::string* _s = _internal_mutable_callsign();
  // @@protoc_insertion_point(field_mutable:atakmap.commoncommo.protobuf.v1.Contact.callsign)
  return _s;
}
inline const std::string& Contact::_internal_callsign() const {
  return _impl_.callsign_.Get();
}
inline void Contact::_internal_set_callsign(const std::string& value) {
  
  _impl_.callsign_.Set(value, GetArenaForAllocation());
}
inline std::string* Contact::_internal_mutable_callsign() {
  
  return _impl_.callsign_.Mutable(GetArenaForAllocation());
}
inline std::string* Contact::release_callsign() {
  // @@protoc_insertion_point(field_release:atakmap.commoncommo.protobuf.v1.Contact.callsign)
  return _impl_.callsign_.Release();
}
inline void Contact::set_allocated_callsign(std::string* callsign) {
  if (callsign != nullptr) {
    
  } else {
    
  }
  _impl_.callsign_.SetAllocated(callsign, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.callsign_.IsDefault()) {
    _impl_.callsign_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:atakmap.commoncommo.protobuf.v1.Contact.callsign)
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
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_contact_2eproto
