//
// Created by 徐琰 on 2022/1/10.
//

#ifndef SCL_SCL_UTIL_CONTAINER_DEFAULT_OBJECTS_HANDLER_H_
#define SCL_SCL_UTIL_CONTAINER_DEFAULT_OBJECTS_HANDLER_H_

#include <cstring>
#include <new>
#include <type_traits>

#include "objects_handler.h"

namespace xcl {
template<typename E, bool v = std::is_copy_assignable_v<E>>
class CopyImpl;

template<typename E>
class CopyImpl<E, true> {
 public:
  static void *
  Impl(void *dst, const void *first, const void *last);
  static void *
  Impl(void *dst, void *end, const void *first, const void *last);
  static void
  Impl(PlainIterable *dst, PlainIterable *first, const PlainIterable &last);
  static void
  Impl(PlainIterable *dst, const PlainIterable &end, PlainIterable *first,
	   const PlainIterable &last);
};
template<typename E>
void *
CopyImpl<E, true>::Impl(void *dst, const void *first, const void *last) {
  if (std::is_trivially_copy_assignable_v<E>) {
	auto len = (const char *)last - (const char *)first;
	memmove(dst, first, len);
	dst = (char *)dst + len;
  } else {
	for (; first != last; first = (E *)first + 1, dst = (E *)dst + 1) {
	  *(E *)dst = *(const E *)first;
	}
  }
  return dst;
}

template<typename E>
void *
CopyImpl<E, true>::Impl(void *dst, void *end, const void *first,
						const void *last) {
  if (std::is_trivially_copy_assignable_v<E>) {
	auto len1 = (const char *)last - (const char *)first;
	auto len2 = (char *)end - (char *)dst;
	auto len = len1 > len2 ? len2 : len1;
	memmove(dst, first, len);
	dst = (char *)dst + len;
  } else {
	for (; first != last && dst != end;
		 dst = (E *)dst + 1, first = (const E *)first + 1) {
	  *(E *)dst = *(const E *)first;
	}
  }
  return dst;
}
template<typename E>
void
CopyImpl<E, true>::Impl(PlainIterable *dst, PlainIterable *first,
						const PlainIterable &last) {
  for (; !last.Equal(*first); dst->Next(), first->Next()) {
	*(E *)dst->Value() = *(const E *)first->Value();
  }
}
template<typename E>
void
CopyImpl<E, true>::Impl(PlainIterable *dst, const PlainIterable &end,
						PlainIterable *first, const PlainIterable &last) {
  for (; !end.Equal(*dst) && !last.Equal(*first); dst->Next(), first->Next()) {
	*(E *)dst->Value() = *(const E *)first->Value();
  }
}

template<typename E>
class CopyImpl<E, false> {
 public:
  static void *
  Impl(void *dst, const void *first, const void *last);
  static void *
  Impl(void *dst, void *end, const void *first, const void *last);
  static void
  Impl(PlainIterable *dst, PlainIterable *first, const PlainIterable &last);
  static void
  Impl(PlainIterable *dst, const PlainIterable &end, PlainIterable *first,
	   const PlainIterable &last);
};

template<typename E>
void *
CopyImpl<E, false>::Impl(void *dst, const void *first, const void *last) {}
template<typename E>
void *
CopyImpl<E, false>::Impl(void *dst, void *end, const void *first,
						 const void *last) {
  return dst;
}
template<typename E>
void
CopyImpl<E, false>::Impl(PlainIterable *dst, PlainIterable *first,
						 const PlainIterable &last) {}
template<typename E>
void
CopyImpl<E, false>::Impl(PlainIterable *dst, const PlainIterable &end,
						 PlainIterable *first, const PlainIterable &last) {}

template<typename E, bool v = std::is_move_assignable_v<E>>
class MoveImpl;

template<typename E>
class MoveImpl<E, true> {
 public:
  static void *
  Impl(void *dst, void *first, void *last);
  static void *
  Impl(void *dst, void *end, void *first, void *last);
  static void
  Impl(PlainIterable *dst, PlainIterable *first, const PlainIterable &last);
  static void
  Impl(PlainIterable *dst, const PlainIterable &end, PlainIterable *first,
	   const PlainIterable &last);
};
template<typename E>
void *
MoveImpl<E, true>::Impl(void *dst, void *first, void *last) {
  if (std::is_trivially_move_assignable_v<E>) {
	auto len = (char *)last - (char *)first;
	memmove(dst, first, len);
	dst = (char *)dst + len;
  } else {
	for (; first != last; first = (E *)first + 1) {
	  *(E *)dst = std::move(*(E *)first);
	}
  }
  return dst;
}
template<typename E>
void *
MoveImpl<E, true>::Impl(void *dst, void *end, void *first, void *last) {
  if (std::is_trivially_move_assignable_v<E>) {
	auto len1 = (char *)end - (char *)dst;
	auto len2 = (char *)last - (char *)first;
	auto len = len2 > len1 ? len1 : len2;
	memmove(dst, first, len);
	dst = (char *)dst + len;
  } else {
	for (; dst != end && first != last;
		 dst = (E *)dst + 1, first = (E *)first + 1) {
	  *(E *)dst = std::move(*(E *)first);
	}
  }
  return dst;
}
template<typename E>
void
MoveImpl<E, true>::Impl(PlainIterable *dst, PlainIterable *first,
						const PlainIterable &last) {
  for (; !last.Equal(*first); dst->Next(), first->Next()) {
	*(E *)dst->Value() = std::move(*(E *)first->Value());
  }
}
template<typename E>
void
MoveImpl<E, true>::Impl(PlainIterable *dst, const PlainIterable &end,
						PlainIterable *first, const PlainIterable &last) {
  for (; !last.Equal(*first) && !end.Equal(*dst); dst->Next(), first->Next()) {
	*(E *)dst->Value() = std::move(*(E *)first->Value());
  }
}

template<typename E>
class MoveImpl<E, false> {
 public:
  static void *
  Impl(void *dst, void *first, void *last);
  static void *
  Impl(void *dst, void *end, void *first, void *last);
  static void
  Impl(PlainIterable *dst, PlainIterable *first, const PlainIterable &last);
  static void
  Impl(PlainIterable *dst, const PlainIterable &end, PlainIterable *first,
	   const PlainIterable &last);
};
template<typename E>
void *
MoveImpl<E, false>::Impl(void *dst, void *first, void *last) {
  return dst;
}
template<typename E>
void *
MoveImpl<E, false>::Impl(void *dst, void *end, void *first, void *last) {
  return dst;
}
template<typename E>
void
MoveImpl<E, false>::Impl(PlainIterable *dst, PlainIterable *first,
						 const PlainIterable &last) {}
template<typename E>
void
MoveImpl<E, false>::Impl(PlainIterable *dst, const PlainIterable &end,
						 PlainIterable *first, const PlainIterable &last) {}

template<typename E, bool v = std::is_copy_assignable_v<E>>
class FillImpl;

template<typename E>
class FillImpl<E, true> {
 public:
  static void *
  Impl(void *dst, unsigned n, const void *value);
  static void *
  Impl(void *first, void *last, const void *value);
  static void
  Impl(PlainIterable *dst, unsigned n, const void *value);
  static void
  Impl(PlainIterable *first, const PlainIterable &last, const void *value);
};
template<typename E>
void *
FillImpl<E, true>::Impl(void *dst, unsigned int n, const void *value) {
  for (; n > 0; --n, dst = (E *)dst + 1) { *(E *)dst = *(const E *)value; }
  return dst;
}
template<typename E>
void *
FillImpl<E, true>::Impl(void *first, void *last, const void *value) {
  for (; first != last; first = (E *)first + 1) {
	*(E *)first = *(const E *)value;
  }
  return first;
}
template<typename E>
void
FillImpl<E, true>::Impl(PlainIterable *dst, unsigned int n, const void *value) {
  for (; n > 0; --n, dst->Next()) { *(E *)dst->Value() = *(const E *)value; }
}
template<typename E>
void
FillImpl<E, true>::Impl(PlainIterable *first, const PlainIterable &last,
						const void *value) {
  for (; last.Equal(*first); first->Next()) {
	*(E *)first->Value() = *(const E *)value;
  }
}

template<typename E>
class FillImpl<E, false> {
 public:
  static void *
  Impl(void *dst, unsigned n, const void *value);
  static void *
  Impl(void *first, void *last, const void *value);
  static void
  Impl(PlainIterable *dst, unsigned n, const void *value);
  static void
  Impl(PlainIterable *first, const PlainIterable &last, const void *value);
};
template<typename E>
void *
FillImpl<E, false>::Impl(void *dst, unsigned int n, const void *value) {
  return dst;
}
template<typename E>
void *
FillImpl<E, false>::Impl(void *first, void *last, const void *value) {
  return first;
}
template<typename E>
void
FillImpl<E, false>::Impl(PlainIterable *dst, unsigned int n,
						 const void *value) {}
template<typename E>
void
FillImpl<E, false>::Impl(PlainIterable *first, const PlainIterable &last,
						 const void *value) {}

template<typename E, bool v = std::is_copy_constructible_v<E>>
class UninitializedCopyImpl;

template<typename E>
class UninitializedCopyImpl<E, true> {
 public:
  static void *
  Impl(void *dst, const void *first, const void *last);
  static void *
  Impl(void *dst, void *end, const void *first, const void *last);
  static void
  Impl(PlainIterable *dst, PlainIterable *first, const PlainIterable &last);
  static void
  Impl(PlainIterable *dst, const PlainIterable &end, PlainIterable *first,
	   const PlainIterable &last);
};
template<typename E>
void *
UninitializedCopyImpl<E, true>::Impl(void *dst, const void *first,
									 const void *last) {
  if (std::is_trivially_copy_constructible_v<E>) {
	auto len = (const char *)last - (const char *)first;
	memmove(dst, first, len);
	dst = (char *)dst + len;
  } else {
	for (; first != last; dst = (E *)dst + 1, first = (const char *)first + 1) {
	  new ((E *)dst) E(*(const E *)first);
	}
  }
  return dst;
}
template<typename E>
void *
UninitializedCopyImpl<E, true>::Impl(void *dst, void *end, const void *first,
									 const void *last) {
  if (std::is_trivially_copy_constructible_v<E>) {
	auto len1 = (char *)end - (char *)dst;
	auto len2 = (const char *)last - (const char *)first;
	auto len = len1 > len2 ? len2 : len1;
	memmove(dst, first, len);
	dst = (char *)dst + len;
  } else {
	for (; dst != end && first != last;
		 dst = (E *)dst + 1, first = (const E *)first + 1) {
	  new ((E *)dst) E(*(const E *)first);
	}
  }
  return dst;
}
template<typename E>
void
UninitializedCopyImpl<E, true>::Impl(PlainIterable *dst, PlainIterable *first,
									 const PlainIterable &last) {
  for (; !last.Equal(*first); dst->Next(), first->Next()) {
	new ((E *)dst->Value()) E(*(const E *)first->Value());
  }
}
template<typename E>
void
UninitializedCopyImpl<E, true>::Impl(PlainIterable *dst,
									 const PlainIterable &end,
									 PlainIterable *first,
									 const PlainIterable &last) {
  for (; !last.Equal(*first) && !end.Equal(*dst); dst->Next(), first->Next()) {
	new ((E *)dst->Value()) E(*(const E *)first->Value());
  }
}

template<typename E>
class UninitializedCopyImpl<E, false> {
 public:
  static void *
  Impl(void *dst, const void *first, const void *last);
  static void *
  Impl(void *dst, void *end, const void *first, const void *last);
  static void
  Impl(PlainIterable *dst, PlainIterable *first, const PlainIterable &last);
  static void
  Impl(PlainIterable *dst, const PlainIterable &end, PlainIterable *first,
	   const PlainIterable &last);
};
template<typename E>
void *
UninitializedCopyImpl<E, false>::Impl(void *dst, const void *first,
									  const void *last) {
  return dst;
}
template<typename E>
void *
UninitializedCopyImpl<E, false>::Impl(void *dst, void *end, const void *first,
									  const void *last) {
  return dst;
}
template<typename E>
void
UninitializedCopyImpl<E, false>::Impl(PlainIterable *dst, PlainIterable *first,
									  const PlainIterable &last) {}
template<typename E>
void
UninitializedCopyImpl<E, false>::Impl(PlainIterable *dst,
									  const PlainIterable &end,
									  PlainIterable *first,
									  const PlainIterable &last) {}

template<typename E, bool v = std::is_move_constructible_v<E>>
class UninitializedMoveImpl;

template<typename E>
class UninitializedMoveImpl<E, true> {
 public:
  static void *
  Impl(void *dst, void *first, void *last);
  static void *
  Impl(void *dst, void *end, void *first, void *last);
  static void
  Impl(PlainIterable *dst, PlainIterable *first, const PlainIterable &last);
  static void
  Impl(PlainIterable *dst, const PlainIterable &end, PlainIterable *first,
	   const PlainIterable &last);
};
template<typename E>
void *
UninitializedMoveImpl<E, true>::Impl(void *dst, void *first, void *last) {
  if (std::is_trivially_move_constructible_v<E>) {
	auto len = (char *)last - (char *)first;
	memmove(dst, first, len);
	dst = (char *)dst + len;
  } else {
	for (; first != last; dst = (E *)dst + 1, first = (E *)first + 1) {
	  new ((E *)dst) E(std::move(*(E *)first));
	}
  }
  return dst;
}
template<typename E>
void *
UninitializedMoveImpl<E, true>::Impl(void *dst, void *end, void *first,
									 void *last) {
  if (std::is_trivially_move_constructible_v<E>) {
	auto len1 = (char *)end - (char *)dst;
	auto len2 = (char *)last - (char *)first;
	auto len = len1 > len2 ? len2 : len1;
	memmove(dst, first, len);
	dst = (char *)dst + len;
  } else {
	for (; dst != end && first != last;
		 dst = (E *)dst + 1, first = (E *)first + 1) {
	  new ((E *)dst) E(std::move(*(E *)first));
	}
  }
  return dst;
}
template<typename E>
void
UninitializedMoveImpl<E, true>::Impl(PlainIterable *dst, PlainIterable *first,
									 const PlainIterable &last) {
  if (std::is_trivially_move_constructible_v<E>) {
	for (; !last.Equal(*first); dst->Next(), first->Next()) {
	  *(E *)dst->Value() = std::move(*(E *)first->Value());
	}
  } else {
	for (; !last.Equal(*first); dst->Next(), first->Next()) {
	  new ((E *)dst->Value()) E(std::move(*(E *)first->Value()));
	}
  }
}
template<typename E>
void
UninitializedMoveImpl<E, true>::Impl(PlainIterable *dst,
									 const PlainIterable &end,
									 PlainIterable *first,
									 const PlainIterable &last) {
  if (std::is_trivially_move_constructible_v<E>) {
	for (; !last.Equal(*first) && !end.Equal(*dst);
		 dst->Next(), first->Next()) {
	  *(E *)dst->Value() = std::move(*(E *)first->Value());
	}
  } else {
	for (; !last.Equal(*first) && !end.Equal(*dst);
		 dst->Next(), first->Next()) {
	  new ((E *)dst->Value()) E(std::move(*(E *)first->Value()));
	}
  }
}

template<typename E>
class UninitializedMoveImpl<E, false> {
 public:
  static void *
  Impl(void *dst, void *first, void *last);
  static void *
  Impl(void *dst, void *end, void *first, void *last);
  static void
  Impl(PlainIterable *dst, PlainIterable *first, const PlainIterable &last);
  static void
  Impl(PlainIterable *dst, const PlainIterable &end, PlainIterable *first,
	   const PlainIterable &last);
};
template<typename E>
void *
UninitializedMoveImpl<E, false>::Impl(void *dst, void *first, void *last) {
  return dst;
}
template<typename E>
void *
UninitializedMoveImpl<E, false>::Impl(void *dst, void *end, void *first,
									  void *last) {
  return dst;
}
template<typename E>
void
UninitializedMoveImpl<E, false>::Impl(PlainIterable *dst, PlainIterable *first,
									  const PlainIterable &last) {}
template<typename E>
void
UninitializedMoveImpl<E, false>::Impl(PlainIterable *dst,
									  const PlainIterable &end,
									  PlainIterable *first,
									  const PlainIterable &last) {}

template<typename E, bool v = std::is_copy_constructible_v<E>>
class UninitializedFillImpl;

template<typename E>
class UninitializedFillImpl<E, true> {
 public:
  static void *
  Impl(void *dst, unsigned n, const void *value);
  static void *
  Impl(void *first, void *last, const void *value);
  static void
  Impl(PlainIterable *dst, unsigned n, const void *value);
  static void
  Impl(PlainIterable *first, const PlainIterable &last, const void *value);
};
template<typename E>
void *
UninitializedFillImpl<E, true>::Impl(void *dst, unsigned int n,
									 const void *value) {
  if (std::is_trivially_copy_constructible_v<E>) {
	for (; n > 0; --n, dst = (E *)dst + 1) { *(E *)dst = *(const E *)value; }
  } else {
	for (; n > 0; --n, dst = (E *)dst + 1) {
	  new ((E *)dst) E(*(const E *)value);
	}
  }
  return dst;
}
template<typename E>
void *
UninitializedFillImpl<E, true>::Impl(void *first, void *last,
									 const void *value) {
  return Impl(first, ((char *)last - (char *)first) / sizeof(E), value);
}
template<typename E>
void
UninitializedFillImpl<E, true>::Impl(PlainIterable *dst, unsigned int n,
									 const void *value) {
  if (std::is_trivially_copy_constructible_v<E>) {
	for (; n > 0; --n, dst->Next()) { *(E *)dst->Value() = *(const E *)value; }
  } else {
	for (; n > 0; --n, dst->Next()) {
	  new ((E *)dst->Value()) E(*(const E *)value);
	}
  }
}
template<typename E>
void
UninitializedFillImpl<E, true>::Impl(PlainIterable *first,
									 const PlainIterable &last,
									 const void *value) {
  if (std::is_trivially_copy_constructible_v<E>) {
	for (; !last.Equal(*first); first->Next()) {
	  *(E *)first->Value() = *(const E *)value;
	}
  } else {
	for (; !last.Equal(*first); first->Next()) {
	  new ((E *)first->Value()) E(*(const E *)value);
	}
  }
}

template<typename E>
class UninitializedFillImpl<E, false> {
 public:
  static void *
  Impl(void *dst, unsigned n, const void *value);
  static void *
  Impl(void *first, void *last, const void *value);
  static void
  Impl(PlainIterable *dst, unsigned n, const void *value);
  static void
  Impl(PlainIterable *first, const PlainIterable &last, const void *value);
};
template<typename E>
void *
UninitializedFillImpl<E, false>::Impl(void *dst, unsigned int n,
									  const void *value) {
  return dst;
}
template<typename E>
void *
UninitializedFillImpl<E, false>::Impl(void *first, void *last,
									  const void *value) {
  return first;
}
template<typename E>
void
UninitializedFillImpl<E, false>::Impl(PlainIterable *dst, unsigned int n,
									  const void *value) {}
template<typename E>
void
UninitializedFillImpl<E, false>::Impl(PlainIterable *first,
									  const PlainIterable &last,
									  const void *value) {}

template<typename E, bool v = std::is_destructible_v<E>>
class DestroyImpl;

template<typename E>
class DestroyImpl<E, true> {
 public:
  static void *
  Impl(void *first, void *last);
  static void
  Impl(PlainIterable *first, const PlainIterable &last);
};
template<typename E>
void *
DestroyImpl<E, true>::Impl(void *first, void *last) {
  if (!std::is_trivially_destructible_v<E>) {
	for (; first != last; first = (E *)first + 1) { ((E *)first)->~E(); }
	return first;
  } else {
	return last;
  }
}
template<typename E>
void
DestroyImpl<E, true>::Impl(PlainIterable *first, const PlainIterable &last) {
  if (!std::is_trivially_destructible_v<E>) {
	for (; !last.Equal(*first); first->Next()) { ((E *)first->Value())->~E(); }
  } else {
	first->Copy(last);
  }
}

template<typename E>
class DestroyImpl<E, false> {
 public:
  static void *
  Impl(void *first, void *last);
  static void
  Impl(PlainIterable *first, const PlainIterable &last);
};
template<typename E>
void *
DestroyImpl<E, false>::Impl(void *first, void *last) {
  return first;
}
template<typename E>
void
DestroyImpl<E, false>::Impl(PlainIterable *first, const PlainIterable &last) {}

template<typename E>
class DefaultObjectsHandler final : public ObjectsHandler {
 public:
  void *
  Copy(void *dst, const void *first, const void *last) override;
  void *
  Copy(void *dst, void *end, const void *first, const void *last) override;
  void *
  Move(void *dst, void *first, void *last) override;
  void *
  Move(void *dst, void *end, void *first, void *last) override;
  void *
  Fill(void *dst, unsigned int n, const void *value) override;
  void *
  Fill(void *first, void *last, const void *value) override;
  void *
  UninitializedCopy(void *dst, const void *first, const void *last) override;
  void *
  UninitializedCopy(void *dst, void *end, const void *first,
					const void *last) override;
  void *
  UninitializedMove(void *dst, void *first, void *last) override;
  void *
  UninitializedMove(void *dst, void *end, void *first, void *last) override;
  void *
  UninitializedFill(void *dst, unsigned int n, const void *value) override;
  void *
  UninitializedFill(void *first, void *last, const void *value) override;
  void *
  Destroy(void *first, void *last) override;
  void
  Copy(PlainIterable *dst, PlainIterable *first,
	   const PlainIterable &last) override;
  void
  Copy(PlainIterable *dst, const PlainIterable &end, PlainIterable *first,
	   const PlainIterable &last) override;
  void
  Move(PlainIterable *dst, PlainIterable *first,
	   const PlainIterable &last) override;
  void
  Move(PlainIterable *dst, const PlainIterable &end, PlainIterable *first,
	   const PlainIterable &last) override;
  void
  Fill(PlainIterable *dst, unsigned int n, const void *value) override;
  void
  Fill(PlainIterable *first, const PlainIterable &last,
	   const void *value) override;
  void
  UninitializedCopy(PlainIterable *dst, PlainIterable *first,
					const PlainIterable &last) override;
  void
  UninitializedCopy(PlainIterable *dst, const PlainIterable &end,
					PlainIterable *first, const PlainIterable &last) override;
  void
  UninitializedMove(PlainIterable *dst, PlainIterable *first,
					const PlainIterable &last) override;
  void
  UninitializedMove(PlainIterable *dst, const PlainIterable &end,
					PlainIterable *first, const PlainIterable &last) override;
  void
  UninitializedFill(PlainIterable *dst, unsigned int n,
					const void *value) override;
  void
  UninitializedFill(PlainIterable *first, const PlainIterable &last,
					const void *value) override;
  void
  Destroy(PlainIterable *first, const PlainIterable &last) override;

 private:
  static DefaultObjectsHandler<E> &instance_;

 private:
  DefaultObjectsHandler() = default;

 public:
  DefaultObjectsHandler(const DefaultObjectsHandler &) = delete;
  DefaultObjectsHandler &
  operator=(const DefaultObjectsHandler &) = delete;

 public:
  static DefaultObjectsHandler &
  Interface();
};

template<typename E>
DefaultObjectsHandler<E> &
DefaultObjectsHandler<E>::Interface() {
  return instance_;
}
template<typename E>
void *
DefaultObjectsHandler<E>::Copy(void *dst, const void *first, const void *last) {
  return CopyImpl<E>::Impl(dst, first, last);
}
template<typename E>
void *
DefaultObjectsHandler<E>::Copy(void *dst, void *end, const void *first,
							   const void *last) {
  return CopyImpl<E>::Impl(dst, end, first, last);
}
template<typename E>
void *
DefaultObjectsHandler<E>::Move(void *dst, void *first, void *last) {
  return MoveImpl<E>::Impl(dst, first, last);
}
template<typename E>
void *
DefaultObjectsHandler<E>::Move(void *dst, void *end, void *first, void *last) {
  return MoveImpl<E>::Impl(dst, end, first, last);
}
template<typename E>
void *
DefaultObjectsHandler<E>::Fill(void *dst, unsigned int n, const void *value) {
  return FillImpl<E>::Impl(dst, n, value);
}
template<typename E>
void *
DefaultObjectsHandler<E>::Fill(void *first, void *last, const void *value) {
  return FillImpl<E>::Impl(first, last, value);
}
template<typename E>
void *
DefaultObjectsHandler<E>::UninitializedCopy(void *dst, const void *first,
											const void *last) {
  return UninitializedCopyImpl<E>::Impl(dst, first, last);
}
template<typename E>
void *
DefaultObjectsHandler<E>::UninitializedCopy(void *dst, void *end,
											const void *first,
											const void *last) {
  return UninitializedCopyImpl<E>::Impl(dst, end, first, last);
}
template<typename E>
void *
DefaultObjectsHandler<E>::UninitializedMove(void *dst, void *first,
											void *last) {
  return UninitializedMoveImpl<E>::Impl(dst, first, last);
}
template<typename E>
void *
DefaultObjectsHandler<E>::UninitializedMove(void *dst, void *end, void *first,
											void *last) {
  return UninitializedMoveImpl<E>::Impl(dst, end, first, last);
}
template<typename E>
void *
DefaultObjectsHandler<E>::UninitializedFill(void *dst, unsigned int n,
											const void *value) {
  return UninitializedFillImpl<E>::Impl(dst, n, value);
}
template<typename E>
void *
DefaultObjectsHandler<E>::UninitializedFill(void *first, void *last,
											const void *value) {
  return UninitializedFillImpl<E>::Impl(first, last, value);
}
template<typename E>
void *
DefaultObjectsHandler<E>::Destroy(void *first, void *last) {
  return DestroyImpl<E>::Impl(first, last);
}
template<typename E>
void
DefaultObjectsHandler<E>::Copy(PlainIterable *dst, PlainIterable *first,
							   const PlainIterable &last) {
  CopyImpl<E>::Impl(dst, first, last);
}
template<typename E>
void
DefaultObjectsHandler<E>::Copy(PlainIterable *dst, const PlainIterable &end,
							   PlainIterable *first,
							   const PlainIterable &last) {
  CopyImpl<E>::Impl(dst, end, first, last);
}
template<typename E>
void
DefaultObjectsHandler<E>::Move(PlainIterable *dst, PlainIterable *first,
							   const PlainIterable &last) {
  MoveImpl<E>::Impl(dst, first, last);
}
template<typename E>
void
DefaultObjectsHandler<E>::Move(PlainIterable *dst, const PlainIterable &end,
							   PlainIterable *first,
							   const PlainIterable &last) {
  MoveImpl<E>::Impl(dst, end, first, last);
}
template<typename E>
void
DefaultObjectsHandler<E>::Fill(PlainIterable *dst, unsigned int n,
							   const void *value) {
  FillImpl<E>::Impl(dst, n, value);
}
template<typename E>
void
DefaultObjectsHandler<E>::Fill(PlainIterable *first, const PlainIterable &last,
							   const void *value) {
  FillImpl<E>::Impl(first, last, value);
}
template<typename E>
void
DefaultObjectsHandler<E>::UninitializedCopy(PlainIterable *dst,
											PlainIterable *first,
											const PlainIterable &last) {
  UninitializedCopyImpl<E>::Impl(dst, first, last);
}
template<typename E>
void
DefaultObjectsHandler<E>::UninitializedCopy(PlainIterable *dst,
											const PlainIterable &end,
											PlainIterable *first,
											const PlainIterable &last) {
  UninitializedCopyImpl<E>::Impl(dst, end, first, last);
}
template<typename E>
void
DefaultObjectsHandler<E>::UninitializedMove(PlainIterable *dst,
											PlainIterable *first,
											const PlainIterable &last) {
  UninitializedMoveImpl<E>::Impl(dst, first, last);
}
template<typename E>
void
DefaultObjectsHandler<E>::UninitializedMove(PlainIterable *dst,
											const PlainIterable &end,
											PlainIterable *first,
											const PlainIterable &last) {
  UninitializedMoveImpl<E>::Impl(dst, end, first, last);
}
template<typename E>
void
DefaultObjectsHandler<E>::UninitializedFill(PlainIterable *dst, unsigned int n,
											const void *value) {
  UninitializedFillImpl<E>::Impl(dst, n, value);
}
template<typename E>
void
DefaultObjectsHandler<E>::UninitializedFill(PlainIterable *first,
											const PlainIterable &last,
											const void *value) {
  UninitializedFillImpl<E>::Impl(first, last, value);
}
template<typename E>
void
DefaultObjectsHandler<E>::Destroy(PlainIterable *first,
								  const PlainIterable &last) {
  DestroyImpl<E>::Impl(first, last);
}

template<typename E>
DefaultObjectsHandler<E> &DefaultObjectsHandler<E>::instance_ =
	*new DefaultObjectsHandler<E>();
}// namespace xcl

#endif// SCL_SCL_UTIL_CONTAINER_DEFAULT_OBJECTS_HANDLER_H_
