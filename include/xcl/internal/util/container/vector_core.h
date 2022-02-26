////
//// Created by 徐琰 on 2022/1/2.
////
//
//#ifndef SCL_SRC_BUILTIN_SCL_UTIL_CONTAINER_VECTOR_CORE_H_
//#define SCL_SRC_BUILTIN_SCL_UTIL_CONTAINER_VECTOR_CORE_H_
//
//#include "../../../../../xcl/util/allocator/allocator.pair_"
//#include "../../../../../xcl/util/container/objects_handler.pair_"
// namespace xcl {
// struct VectorContext {
//  char *vec_start;
//  char *vec_finish;
//  char *vec_end;
//  Allocator *vec_allocator;
//};
// unsigned VecGrow(unsigned vec_size, unsigned increase_size, unsigned
// vec_cap); bool VecReserveAtPosition(VectorContext *vector_context, char
// *reserve_pos,
//                          unsigned reserve_elements, unsigned element_size);
// void VecAssign(VectorContext *vector_context, unsigned n, const char *value,
//               unsigned element_size, ObjectsHandler *objects_handler);
// void VecAssign(VectorContext *vector_context, const void *first,
//               const void *last, unsigned element_size,
//               ObjectsHandler *objects_handler);
// bool VecMoveAssign(VectorContext *vector_context, void *first, void *last,
//                   unsigned element_size, ObjectsHandler *objects_handler);
// bool VecMoveAssign(VectorContext *vector_context, PlainIterable *first,
//                   const PlainIterable &last, unsigned element_size,
//                   ObjectsHandler *objects_handler);
// void VecAssign(VectorContext *vector_context, PlainIterable *first,
//               const PlainIterable &last, unsigned element_size,
//               ObjectsHandler *objects_handler);
// void VecClear(VectorContext *vector_context, ObjectsHandler
// *objects_handler); bool VecAdd(VectorContext *vector_context, char *pos,
// const void *value,
//            unsigned element_size, ObjectsHandler *objects_handler);
// bool VecAdd(VectorContext *vector_context, char *pos, unsigned element_count,
//            const void *value, unsigned element_size,
//            ObjectsHandler *objects_handler);
// bool VecAdd(VectorContext *vector_context, char *pos, const void *first,
//            const void *last, unsigned element_size,
//            ObjectsHandler *objects_handler);
// bool VecAdd(VectorContext *vector_context, char *pos, PlainIterable *first,
//            const PlainIterable &last, unsigned element_size,
//            ObjectsHandler *objects_handler);
// void *VecRemove(VectorContext *vector_context, char *pos, unsigned
// element_size,
//                ObjectsHandler *objects_handler);
// void VecRemove(VectorContext *vector_context, char *first, char *last,
//               ObjectsHandler *objects_handler);
// void VecResize(VectorContext *vector_context, unsigned n, const void *value,
//               unsigned element_size, ObjectsHandler *objects_handler);
// void VecReserve(VectorContext *vector_context, unsigned n,
//                unsigned element_size);
// bool VecMoveAdd(VectorContext *vector_context, char *pos, void *value,
//                unsigned element_size, ObjectsHandler *objects_handler);
// bool VecMoveAdd(VectorContext *vector_context, char *pos, void *first,
//                void *last, unsigned element_size,
//                ObjectsHandler *objects_handler);
// bool VecMoveAdd(VectorContext *vector_context, char *pos, PlainIterable
// *first,
//                const PlainIterable &last, unsigned element_size,
//                ObjectsHandler *objects_handler);
// bool VecPush(VectorContext *vector_context, const void *value,
//             unsigned element_size, ObjectsHandler *objects_handler);
// bool VecPop(VectorContext *vector_context, unsigned element_size,
//            ObjectsHandler *objects_handler);
// bool VecMovePush(VectorContext *vector_context, void *value,
//                 unsigned element_size, ObjectsHandler *objects_handler);
//} // namespace xcl
//
//#endif // SCL_SRC_BUILTIN_SCL_UTIL_CONTAINER_VECTOR_CORE_H_
