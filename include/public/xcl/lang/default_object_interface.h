//
// Created by 徐琰 on 2021/10/31.
//

#ifndef SCL_DEFOBJINTERFACE_H
#define SCL_DEFOBJINTERFACE_H

#include <type_traits>

#include "object_interface.h"

namespace xcl {
template<typename E, bool, bool>
class ArgsConstructImpl {
 public:
  static void
  construct(void *dst, unsigned n, va_list args) {
	throw;
  }
};

template<typename E>
class ArgsConstructImpl<E, true, true> {
 public:
  static void
  construct(void *dst, unsigned n, va_list args) {
	if (n > 0) {
	  new ((E *)dst) E(n, args);
	} else {
	  new ((E *)dst) E();
	}
  }
};

template<typename E>
class ArgsConstructImpl<E, true, false> {
 public:
  static void
  construct(void *dst, unsigned n, va_list args) {
	if (n > 0) {
	  new ((E *)dst) E(n, args);
	} else {
	  throw;
	}
  }
};

template<typename E>
class ArgsConstructImpl<E, false, true> {
 public:
  static void
  construct(void *dst, unsigned n, va_list args) {
	if (n > 0) {
	  throw;
	} else {
	  new ((E *)dst) E();
	}
  }
};

template<typename E, bool>
class CopyConstructImpl {
 public:
  static void
  construct(void *dst, const void *src) {
	throw;
  }
};

template<typename E>
class CopyConstructImpl<E, true> {
 public:
  static void
  construct(void *dst, const void *src) {
	if (std::is_trivially_copy_constructible_v<E>) {
	  *(E *)dst = *(const E *)src;
	} else {
	  new ((E *)dst) E(*(const E *)src);
	}
  }
};

template<typename E, bool>
class MoveConstructImpl {
 public:
  static void
  construct(void *dst, void *src) {
	throw;
  }
};

template<typename E>
class MoveConstructImpl<E, true> {
 public:
  static void
  construct(void *dst, void *src) {
	if (std::is_trivially_move_constructible_v<E>) {
	  *(E *)dst = *(E *)src;
	} else {
	  new ((E *)dst) E(std::move(*(E *)src));
	}
  }
};

template<typename E, bool>
class CopyAssignImpl {
 public:
  static void
  assign(void *dst, const void *src) {
	throw;
  }
};

template<typename E>
class CopyAssignImpl<E, true> {
 public:
  static void
  assign(void *dst, const void *src) {
	*(E *)dst = *(const E *)src;
  }
};

template<typename E, bool>
class MoveAssignImpl {
 public:
  static void
  assign(void *dst, void *src) {
	throw;
  }
};

template<typename E>
class MoveAssignImpl<E, true> {
 public:
  static void
  assign(void *dst, void *src) {
	*(E *)dst = std::move(*(E *)src);
  }
};

template<typename E, bool>
class DestructImpl {
 public:
  static void
  destruct(void *dst) {
	throw;
  }
};

template<typename E>
class DestructImpl<E, true> {
 public:
  static void
  destruct(void *dst) {
	if (!std::is_trivially_destructible_v<E>) { ((E *)dst)->~E(); }
  }
};

template<typename E>
class DefaultObjectInterface final : public ObjectInterface {
 private:
  DefaultObjectInterface() = default;

 public:
  DefaultObjectInterface(const DefaultObjectInterface &) = delete;

 private:
  static DefaultObjectInterface<E> *instance_;

 public:
  static DefaultObjectInterface<E> &
  instance() {
	if (!instance_) { instance_ = new DefaultObjectInterface(); }
	return *instance_;
  }

 public:
  DefaultObjectInterface &
  operator=(const DefaultObjectInterface &) = delete;

 public:
  void
  ArgsConstruct(void *dst, unsigned int n, ...) override {
	va_list args;
	va_start(args, n);
	ArgsConstruct(dst, n, args);
	va_end(args);
  }

  void
  ArgsConstruct(void *dst, unsigned int n, va_list args) {
	typedef ArgsConstructImpl<E, std::is_constructible_v<E, unsigned, va_list>,
							  std::is_constructible_v<E>>
		ArgsConstructImpl;
	ArgsConstructImpl::construct(dst, n, args);
  }

  void
  CopyConstruct(void *dst, const void *src) override {
	CopyConstructImpl<E, std::is_copy_constructible_v<E>>::construct(dst, src);
  }

  void
  MoveConstruct(void *dst, void *src) override {
	MoveConstructImpl<E, std::is_move_constructible_v<E>>::construct(dst, src);
  }

  void
  CopyAssign(void *dst, const void *src) override {
	CopyAssignImpl<E, std::is_copy_assignable_v<E>>::assign(dst, src);
  }

  void
  MoveAssign(void *dst, void *src) override {
	MoveAssignImpl<E, std::is_move_assignable_v<E>>::assign(dst, src);
  }

  void
  Destroy(void *dst) override {
	DestructImpl<E, std::is_destructible_v<E>>::destruct(dst);
  }
};

template<typename E>
DefaultObjectInterface<E> *DefaultObjectInterface<E>::instance_ = nullptr;
}// namespace xcl

#endif// SCL_DEFOBJINTERFACE_H
