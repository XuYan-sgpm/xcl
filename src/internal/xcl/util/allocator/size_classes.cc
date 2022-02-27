//
// Created by 徐琰 on 2021/12/14.
//
#include <xcl/util/allocator/size_classes.h>
#include <cassert>
#include <iostream>
namespace xcl {
SizeClasses SizeClasses::size_classes_(1 << 13, 13, 1 << 24);
unsigned
SizeClasses::Log2(unsigned i) {
  if (i == 0) {
    return 0;
  }
  unsigned n = 1;
  unsigned tmp = i;
  unsigned shifts[] = {16, 24, 28, 30};
  unsigned l = sizeof(shifts) / sizeof(unsigned);
  unsigned idx = 0;
  for (; idx < l; ++idx) {
    if (!(tmp >> shifts[idx])) {
      n += 32 - shifts[idx];
      tmp <<= 32 - shifts[idx];
    }
  }
  n -= (tmp >> 31);
  return 31 - n;
}
bool
SizeClasses::IsPow2(unsigned int i) {
  return (i & (i - 1)) == 0;
}
SizeClasses::SizeClasses(unsigned int page_size, unsigned int page_shifts,
                         unsigned int chunk_size)
  : kPageSize(page_size), kPageShifts(page_shifts), kChunkSize(chunk_size) {
  if (!IsPow2(page_size) || !IsPow2(chunk_size) ||
      page_size != (1 << page_shifts)) {
    throw;
  }
  unsigned group = Log2(chunk_size) + 1 - kLog2GroupInit;
  size_class_tab_ = new short *[group << kLog2GroupSize];
  for (unsigned i = 0; i < (group << kLog2GroupSize); ++i) {
    size_class_tab_[i] = new short[7];
  }
  n_size_ = CalSizeClasses();
  size_idx_2_size_tab_ = new unsigned[n_size_];
  page_idx_2_size_tab_ = new unsigned[n_page_sizes_];
  CalIdx2SizeTab();
  size_2_idx_tab_ = new unsigned[kLookupMaxSize >> 4];
  CalSize2IdxTab();
}
unsigned
SizeClasses::CalSizeClasses() {
  unsigned log_2_group = kLog2GroupInit;
  unsigned log_2_delta = log_2_group;
  unsigned n_delta = 0;
  unsigned size;
  unsigned idx = 0;
  while (idx < (1 << kLog2GroupSize)) {
    size = CalSizeClass(idx++, log_2_group, log_2_delta, n_delta++);
  }
  log_2_group += kLog2GroupSize;
  unsigned normal_max_size = -1;
  while (size < kChunkSize) {
    n_delta = 1;
    while (n_delta <= (1 << kLog2GroupSize) && size < kChunkSize) {
      size = CalSizeClass(idx++, log_2_group, log_2_delta, n_delta++);
      normal_max_size = size;
    }
    ++log_2_group;
    ++log_2_delta;
  }
  assert(normal_max_size == kChunkSize);
  return idx;
}
unsigned
SizeClasses::CalSizeClass(unsigned int idx, unsigned int log_2_group,
                          unsigned int log_2_delta, unsigned int n_delta) {
  unsigned size = (1 << log_2_group) + (n_delta << log_2_delta);
  short is_multi_pages = kPageShifts <= log_2_delta
                             ? ( short ) 1
                             : ( short ) (size % kPageSize == 0);
  short *size_class = size_class_tab_[idx];
  size_class[0] = ( short ) idx;
  size_class[1] = ( short ) log_2_group;
  size_class[2] = ( short ) log_2_delta;
  size_class[3] = ( short ) n_delta;
  size_class[4] = is_multi_pages;
  size_class[5] = ( short ) (size <= kMaxSubpageSize);
  size_class[6] =
      ( short ) (size <= (1 << kLog2MaxLookupSize) ? log_2_delta : 0);
  if (is_multi_pages) {
    ++n_page_sizes_;
  }
  return size;
}
void
SizeClasses::CalIdx2SizeTab() {
  unsigned page_idx = 0;
  for (unsigned i = 0; i < n_size_; i++) {
    short *size_class = size_class_tab_[i];
    unsigned size = (1 << size_class[1]) + (size_class[3] << size_class[2]);
    size_idx_2_size_tab_[i] = size;
    if (size_class[4] == 1) {
      page_idx_2_size_tab_[page_idx++] = size;
    }
  }
}
void
SizeClasses::CalSize2IdxTab() {
  unsigned idx = 0;
  unsigned size = 0;
  for (unsigned i = 0; size <= kLookupMaxSize; ++i) {
    unsigned log_2_delta = size_class_tab_[i][2];
    unsigned times = 1 << (log_2_delta - kLog2GroupInit);
    while (size <= kLookupMaxSize && times-- > 0) {
      size_2_idx_tab_[idx++] = i;
      size = (idx + 1) << kLog2GroupInit;
    }
  }
}
unsigned
SizeClasses::SizeIdx2Size(unsigned int size_idx) const {
  return size_idx_2_size_tab_[size_idx];
}
unsigned
SizeClasses::PageIdx2Size(unsigned int page_idx) const {
  return page_idx_2_size_tab_[page_idx];
}
unsigned
SizeClasses::Size2SizeIdx(unsigned int size) const {
  if (!size) {
    return 0;
  }
  if (size > kChunkSize) {
    return n_size_;
  }
  if (size <= kLookupMaxSize) {
    return size_2_idx_tab_[(size - 1) >> kLog2GroupInit];
  }
  unsigned x = Log2((size << 1) - 1);
  unsigned group_shift = x < kLog2GroupInit + kLog2GroupSize + 1
                             ? 0
                             : x - kLog2GroupInit - kLog2GroupSize;
  unsigned group = 1 << group_shift;
  unsigned log_2_delta = x < kLog2GroupInit + kLog2GroupSize + 1
                             ? kLog2GroupInit
                             : x - kLog2GroupSize - 1;
  unsigned mod = (size - 1) >> log_2_delta & (1 << kLog2GroupSize) - 1;
  return group + mod;
}
unsigned
SizeClasses::Page2PageIdx(unsigned int pages) const {
  if (pages == 0) {
    return 0;
  }
  unsigned size = pages << kPageShifts;
  if (size > kChunkSize) {
    return n_size_;
  }
  unsigned x = Log2((size << 1) - 1);
  unsigned shift = x <= kPageShifts + kLog2GroupSize
                       ? 0
                       : x - (kPageShifts + kLog2GroupSize);
  unsigned group = shift << kLog2GroupSize;
  unsigned log_2_delta = x <= kPageShifts + kLog2GroupSize + 1
                             ? kPageShifts
                             : x - kLog2GroupSize - 1;
  unsigned mod = (size - 1) >> log_2_delta & (1 << kLog2GroupSize) - 1;
  unsigned page_idx = group + mod;
  if (page_idx_2_size_tab_[page_idx] < size) {
    ++page_idx;
  }
  return page_idx;
}
unsigned
SizeClasses::Normalize(unsigned int size) const {
  if (size == 0) {
    return 0;
  }
  if (size <= kLookupMaxSize) {
    return size_idx_2_size_tab_[size_2_idx_tab_[(size - 1) >> kLog2GroupInit]];
  }
  unsigned x = Log2((size << 1) - 1);
  auto log_2_delta = x <= kLog2GroupSize + kLog2GroupInit + 1
                         ? kLog2GroupInit
                         : x - kLog2GroupSize - 1;
  auto delta_mask = (1 << log_2_delta) - 1;
  return (size + delta_mask) & ~delta_mask;
}
SizeClasses &
SizeClasses::Instance() {
  return size_classes_;
}
unsigned
SizeClasses::GetSizes() const {
  return n_size_;
}
unsigned
SizeClasses::GetChunkSize() const {
  return kChunkSize;
}
unsigned
SizeClasses::GetPages() const {
  return n_page_sizes_;
}
}  // namespace xcl
