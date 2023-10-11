// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: track.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_track_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_track_2eproto

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
#define PROTOBUF_INTERNAL_EXPORT_track_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_track_2eproto {
  static const uint32_t offsets[];
};
namespace atakmap {
namespace commoncommo {
namespace protobuf {
namespace v1 {
class Track;
struct TrackDefaultTypeInternal;
extern TrackDefaultTypeInternal _Track_default_instance_;
}  // namespace v1
}  // namespace protobuf
}  // namespace commoncommo
}  // namespace atakmap
PROTOBUF_NAMESPACE_OPEN
template<> ::atakmap::commoncommo::protobuf::v1::Track* Arena::CreateMaybeMessage<::atakmap::commoncommo::protobuf::v1::Track>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace atakmap {
namespace commoncommo {
namespace protobuf {
namespace v1 {

// ===================================================================

class Track final :
    public ::PROTOBUF_NAMESPACE_ID::MessageLite /* @@protoc_insertion_point(class_definition:atakmap.commoncommo.protobuf.v1.Track) */ {
 public:
  inline Track() : Track(nullptr) {}
  ~Track() override;
  explicit PROTOBUF_CONSTEXPR Track(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Track(const Track& from);
  Track(Track&& from) noexcept
    : Track() {
    *this = ::std::move(from);
  }

  inline Track& operator=(const Track& from) {
    CopyFrom(from);
    return *this;
  }
  inline Track& operator=(Track&& from) noexcept {
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

  static const Track& default_instance() {
    return *internal_default_instance();
  }
  static inline const Track* internal_default_instance() {
    return reinterpret_cast<const Track*>(
               &_Track_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Track& a, Track& b) {
    a.Swap(&b);
  }
  inline void Swap(Track* other) {
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
  void UnsafeArenaSwap(Track* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Track* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Track>(arena);
  }
  void CheckTypeAndMergeFrom(const ::PROTOBUF_NAMESPACE_ID::MessageLite& from)  final;
  void CopyFrom(const Track& from);
  void MergeFrom(const Track& from);
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
  void InternalSwap(Track* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "atakmap.commoncommo.protobuf.v1.Track";
  }
  protected:
  explicit Track(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  std::string GetTypeName() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kSpeedFieldNumber = 1,
    kCourseFieldNumber = 2,
  };
  // double speed = 1;
  void clear_speed();
  double speed() const;
  void set_speed(double value);
  private:
  double _internal_speed() const;
  void _internal_set_speed(double value);
  public:

  // double course = 2;
  void clear_course();
  double course() const;
  void set_course(double value);
  private:
  double _internal_course() const;
  void _internal_set_course(double value);
  public:

  // @@protoc_insertion_point(class_scope:atakmap.commoncommo.protobuf.v1.Track)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    double speed_;
    double course_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_track_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Track

// double speed = 1;
inline void Track::clear_speed() {
  _impl_.speed_ = 0;
}
inline double Track::_internal_speed() const {
  return _impl_.speed_;
}
inline double Track::speed() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.Track.speed)
  return _internal_speed();
}
inline void Track::_internal_set_speed(double value) {
  
  _impl_.speed_ = value;
}
inline void Track::set_speed(double value) {
  _internal_set_speed(value);
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.Track.speed)
}

// double course = 2;
inline void Track::clear_course() {
  _impl_.course_ = 0;
}
inline double Track::_internal_course() const {
  return _impl_.course_;
}
inline double Track::course() const {
  // @@protoc_insertion_point(field_get:atakmap.commoncommo.protobuf.v1.Track.course)
  return _internal_course();
}
inline void Track::_internal_set_course(double value) {
  
  _impl_.course_ = value;
}
inline void Track::set_course(double value) {
  _internal_set_course(value);
  // @@protoc_insertion_point(field_set:atakmap.commoncommo.protobuf.v1.Track.course)
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
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_track_2eproto
