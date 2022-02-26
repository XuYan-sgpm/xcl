////
//// Created by 徐琰 on 2022/1/14.
////
//
//#include "../../../../../xcl/util/container/basic_vector.pair_"
//#include "../../../../../xcl/util/allocator/sys_allocator.pair_"
//#include "../../../../internal/xcl/util/container/vector_core.pair_"
//#include <cstring>
//
// namespace xcl {
// BasicVector::BasicVector() { Initialize(0, &SysAllocator::instance()); }
// void BasicVector::Initialize(unsigned n, Allocator *allocator) {
//  auto *vector_context = GetVectorContext();
//  vector_context->vec_allocator = allocator;
//  auto bytes = n * GetElementSize();
//  auto *vec_start = (char *)allocator->Allocate(bytes);
//  if (!vec_start) {
//    vector_context->vec_start = nullptr;
//    vector_context->vec_finish = nullptr;
//    vector_context->vec_end = nullptr;
//  } else {
//    vector_context->vec_start = vec_start;
//    vector_context->vec_finish = vec_start;
//    vector_context->vec_end = vec_start + bytes;
//  }
//}
// BasicVector::BasicVector(unsigned int n) {
//  Initialize(n, &SysAllocator::instance());
//}
// BasicVector::BasicVector(unsigned int n, Allocator *allocator) {
//  Initialize(n, allocator);
//}
// BasicVector::BasicVector(const BasicVector &v) {
//  Initialize(v.Size(), &SysAllocator::instance());
//}
// BasicVector::BasicVector(BasicVector &&v) noexcept {
//  memcpy(data, v.data, sizeof(data));
//  memset(v.data, 0, sizeof(data));
//  v.GetVectorContext()->vec_allocator = GetVectorContext()->vec_allocator;
//}
// void BasicVector::FreeVector() {
//  auto *vector_context = GetVectorContext();
//  auto *vec_start = vector_context->vec_start;
//  auto all_bytes = vector_context->vec_end - vec_start;
//  GetInterface()->Destroy(vec_start, vector_context->vec_finish);
//  vector_context->vec_allocator->Deallocate(vec_start, all_bytes);
//  vector_context->vec_start = nullptr;
//  vector_context->vec_finish = nullptr;
//  vector_context->vec_end = nullptr;
//}
//// BasicVector &BasicVector::operator=(const BasicVector &v) {
////   if (this != &v) {
////	auto *other_context = v.GetVectorContext();
////	Assign(other_context->vec_start, other_context->vec_finish);
////   }
////   return *this;
//// }
// VectorContext *BasicVector::GetVectorContext() {
//   return reinterpret_cast<VectorContext *>(data);
// }
// const VectorContext *BasicVector::GetVectorContext() const {
//   return reinterpret_cast<const VectorContext *>(data);
// }
// BasicVector &BasicVector::operator=(BasicVector &&v) noexcept {
//   if (this != &v) {
//     auto *vector_context = GetVectorContext();
//     GetInterface()->Destroy(vector_context->vec_start,
//                             vector_context->vec_finish);
//     vector_context->vec_allocator->Deallocate(vector_context->vec_start,
//                                               vector_context->vec_end -
//                                                   vector_context->vec_start);
//     auto *other_context = v.GetVectorContext();
//     vector_context->vec_start = other_context->vec_start;
//     vector_context->vec_finish = other_context->vec_finish;
//     vector_context->vec_end = other_context->vec_end;
//     other_context->vec_start = other_context->vec_finish = nullptr;
//     other_context->vec_end = nullptr;
//   }
//   return *this;
// }
// char *BasicVector::operator[](unsigned int idx) {
//   return GetVectorContext()->vec_start + (idx * GetElementSize());
// }
// bool BasicVector::PushBack(const void *value) {
//   return VecPush(GetVectorContext(), value, GetElementSize(),
//   GetInterface());
// }
// bool BasicVector::PopBack() {
//   return VecPop(GetVectorContext(), GetElementSize(), GetInterface());
// }
// unsigned BasicVector::Size() const {
//   auto *vector_context = GetVectorContext();
//   return (vector_context->vec_finish - vector_context->vec_start) /
//          GetElementSize();
// }
// bool BasicVector::IsEmpty() const {
//   return GetVectorContext()->vec_start == GetVectorContext()->vec_finish;
// }
// unsigned BasicVector::Capacity() const {
//   auto *vector_context = GetVectorContext();
//   return (vector_context->vec_end - vector_context->vec_start) /
//          GetElementSize();
// }
// bool BasicVector::Add(char *pos, const void *value) {
//   return VecAdd(GetVectorContext(), pos, value, GetElementSize(),
//                 GetInterface());
// }
// } // namespace xcl
