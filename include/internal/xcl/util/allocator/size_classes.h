//
// Created by 徐琰 on 2021/12/14.
//

#ifndef SCL_SIZE_CLASSES_H
#define SCL_SIZE_CLASSES_H

namespace xcl {
class SizeClasses {
 protected:
  static unsigned
  Log2(unsigned i);
  static bool
  IsPow2(unsigned i);

 protected:
  const static unsigned kLog2GroupInit = 4;
  const static unsigned kLog2GroupSize = 2;
  const static unsigned kLog2MaxLookupSize = 12;
  const static unsigned kMaxSubpageSize = 28 * 1024;

 private:
  SizeClasses(unsigned page_size, unsigned page_shifts, unsigned chunk_size);

 protected:
  const unsigned kPageSize, kPageShifts, kChunkSize;
  unsigned n_size_ = 0, n_page_sizes_ = 0;
  const unsigned kLookupMaxSize = 1 << kLog2MaxLookupSize;
  short **size_class_tab_ = nullptr;
  unsigned *size_2_idx_tab_ = nullptr;
  unsigned *page_idx_2_size_tab_ = nullptr;
  unsigned *size_idx_2_size_tab_ = nullptr;

 private:
  unsigned
  CalSizeClasses();
  unsigned
  CalSizeClass(unsigned idx, unsigned log_2_group, unsigned log_2_delta,
			   unsigned n_delta);
  void
  CalIdx2SizeTab();
  void
  CalSize2IdxTab();

 public:
  unsigned
  SizeIdx2Size(unsigned size_idx) const;
  unsigned
  PageIdx2Size(unsigned page_idx) const;
  unsigned
  Size2SizeIdx(unsigned size) const;
  unsigned
  Page2PageIdx(unsigned pages) const;
  unsigned
  Normalize(unsigned size) const;
  unsigned
  GetSizes() const;
  unsigned
  GetChunkSize() const;
  unsigned
  GetPages() const;

 private:
  static SizeClasses size_classes_;

 public:
  static SizeClasses &
  Instance();

 public:
  SizeClasses &
  operator=(const SizeClasses &) = delete;
  SizeClasses(const SizeClasses &) = delete;
};
}// namespace xcl

#endif// SCL_SIZE_CLASSES_H
