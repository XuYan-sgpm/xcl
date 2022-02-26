////
//// Created by 徐琰 on 2022/1/2.
////
//
//#include "vector_core.pair_"
//#include "normal_pointer_iterable.pair_"
//#include <cstring>
//
// namespace xcl {
// unsigned VecGrow(unsigned int vec_size, unsigned int increase_size,
//                 unsigned int vec_cap) {
//  unsigned diff = vec_cap - (vec_size + increase_size);
//  return vec_cap + (vec_cap > diff ? vec_cap : diff);
//}
// bool VecReserveAtPosition(VectorContext *vector_context, char *reserve_pos,
//                          unsigned reserve_elements, unsigned element_size) {
//  char *vec_start = vector_context->vec_start;
//  char *vec_finish = vector_context->vec_finish;
//  unsigned vec_size = (vec_finish - vec_start) / element_size;
//  unsigned vec_cap = (vector_context->vec_end - vec_start) / element_size;
//  unsigned reserve_bytes = reserve_elements * element_size;
//  if (vec_size + reserve_elements <= vec_cap) {
//    if (reserve_pos != vec_finish) {
//      memmove(reserve_pos + reserve_bytes, reserve_pos,
//              vec_finish - reserve_pos);
//    }
//    vector_context->vec_finish += reserve_bytes;
//  } else {
//    unsigned new_vec_cap = VecGrow(vec_size, reserve_elements, vec_cap);
//    auto *allocator = vector_context->vec_allocator;
//    auto *new_vec_start =
//        (char *)allocator->Allocate(new_vec_cap * element_size);
//    if (!new_vec_start) {
//      return false;
//    }
//    if (reserve_pos == vec_start) {
//      memcpy(new_vec_start + reserve_bytes, vec_start, vec_finish -
//      vec_start);
//    } else if (reserve_pos == vec_finish) {
//      memcpy(new_vec_start, vec_start, vec_finish - vec_start);
//    } else {
//      auto offset = reserve_pos - vec_start;
//      memcpy(new_vec_start, vec_start, reserve_pos - vec_start);
//      memcpy(new_vec_start + offset + reserve_bytes, reserve_pos,
//             vec_finish - reserve_pos);
//    }
//    vector_context->vec_start = new_vec_start;
//    vector_context->vec_finish =
//        new_vec_start + vec_size * element_size + reserve_bytes;
//    vector_context->vec_end = new_vec_start + new_vec_cap * element_size;
//    allocator->Deallocate(vec_start, vec_cap * element_size);
//  }
//  return true;
//}
// void VecAssign(VectorContext *vector_context, unsigned int n, const char
// *value,
//               unsigned int element_size, ObjectsHandler *objects_handler) {
//  char *vec_start = vector_context->vec_start;
//  char *vec_finish = vector_context->vec_finish;
//  auto vec_size = (vec_finish - vec_start) / element_size;
//  auto vec_cap = (vector_context->vec_end - vec_start) / element_size;
//  if (n == 0) {
//    objects_handler->Destroy(vec_start, vec_finish);
//    vector_context->vec_finish = vec_start;
//  } else if (n <= vec_size) {
//    objects_handler->Fill(vec_start, n, value);
//    if (n < vec_size) {
//      vector_context->vec_finish = vec_start + n * element_size;
//      objects_handler->Destroy(vector_context->vec_finish, vec_finish);
//    }
//  } else if (n <= vec_cap) {
//    objects_handler->Fill(vec_start, vec_size, value);
//    objects_handler->UninitializedFill(vec_finish, n - vec_size, value);
//    vector_context->vec_finish = vec_start + (n * element_size);
//  } else {
//    auto res = n - vec_cap;
//    auto new_vec_cap = vec_cap + (vec_cap > res ? vec_cap : res);
//    auto *allocator = vector_context->vec_allocator;
//    char *new_vec_start =
//        (char *)allocator->Allocate(new_vec_cap * element_size);
//    if (!new_vec_start) {
//      return;
//    }
//    objects_handler->UninitializedFill(new_vec_start, n, value);
//    objects_handler->Destroy(vec_start, vec_finish);
//    allocator->Deallocate(vec_start, vec_cap * element_size);
//    vector_context->vec_start = new_vec_start;
//    vector_context->vec_finish = new_vec_start + n * element_size;
//    vector_context->vec_end = new_vec_start + new_vec_cap * element_size;
//  }
//}
// void VecAssign(VectorContext *vector_context, const void *first,
//               const void *last, unsigned int element_size,
//               ObjectsHandler *objects_handler) {
//  char *vec_start = vector_context->vec_start;
//  char *vec_finish = vector_context->vec_finish;
//  auto vec_size = (vec_finish - vec_start) / element_size;
//  auto vec_cap = (vector_context->vec_end - vec_start) / element_size;
//  auto element_count =
//      ((const char *)last - (const char *)first) / element_size;
//  if (element_count == 0) {
//    objects_handler->Destroy(vec_start, vec_finish);
//    vector_context->vec_finish = vec_start;
//  } else if (element_count <= vec_size) {
//    objects_handler->Copy(vec_start, first, last);
//    vector_context->vec_finish = vec_start + (element_count * element_size);
//    if (element_count < vec_size) {
//      objects_handler->Destroy(vector_context->vec_finish, vec_finish);
//    }
//  } else if (element_count <= vec_cap) {
//    const void *middle = (const char *)first + (vec_finish - vec_start);
//    objects_handler->Copy(vec_start, first, middle);
//    objects_handler->UninitializedCopy(vec_finish, middle, last);
//    vector_context->vec_finish = vec_start + (element_count * element_size);
//  } else {
//    auto new_vec_cap = VecGrow(vec_size, element_count - vec_size, vec_cap);
//    auto *allocator = vector_context->vec_allocator;
//    char *new_vec_start =
//        (char *)allocator->Allocate(new_vec_cap * element_size);
//    if (!new_vec_start) {
//      return;
//    }
//    objects_handler->UninitializedCopy(new_vec_start, first, last);
//    objects_handler->Destroy(vec_start, vec_finish);
//    allocator->Deallocate(vec_start, vec_cap * element_size);
//    vector_context->vec_start = new_vec_start;
//    vector_context->vec_finish = new_vec_start + element_count * element_size;
//    vector_context->vec_end = new_vec_start + new_vec_cap * element_size;
//  }
//}
// void VecAssign(VectorContext *vector_context, PlainIterable *first,
//               const PlainIterable &last, unsigned int element_size,
//               ObjectsHandler *objects_handler) {
//  char *vec_start = vector_context->vec_start;
//  char *vec_finish = vector_context->vec_finish;
//  auto vec_size = (vec_finish - vec_start) / element_size;
//  auto vec_cap = (vector_context->vec_end - vec_start) / element_size;
//  auto element_count = last - (*first);
//  if (element_count == 0) {
//    objects_handler->Destroy(vec_start, vec_finish);
//    vector_context->vec_finish = vec_start;
//  } else if (element_count <= vec_size) {
//    vector_context->vec_finish = vec_start + element_count * element_size;
//    NormalPointerIterable it(vec_start, element_size);
//    objects_handler->Copy(&it, first, last);
//    if (element_count < vec_size) {
//      objects_handler->Destroy(it.Value(), vec_finish);
//    }
//  } else if (element_count <= vec_cap) {
//    NormalPointerIterable it(vec_start, element_size);
//    objects_handler->Copy(&it, NormalPointerIterable(vec_finish,
//    element_size),
//                          first, last);
//    objects_handler->UninitializedCopy(&it, first, last);
//    vector_context->vec_finish = vec_start + (element_count * element_size);
//  } else {
//    auto *allocator = vector_context->vec_allocator;
//    auto new_vec_cap = VecGrow(vec_size, element_count - vec_size, vec_cap);
//    auto *new_vec_start =
//        (char *)allocator->Allocate(new_vec_cap * element_size);
//    if (!new_vec_start) {
//      return;
//    }
//    NormalPointerIterable it(new_vec_start, element_size);
//    objects_handler->UninitializedCopy(&it, first, last);
//    objects_handler->Destroy(vec_start, vec_finish);
//    allocator->Deallocate(vec_start, vec_cap * element_size);
//    vector_context->vec_start = new_vec_start;
//    vector_context->vec_finish = new_vec_start + element_count * element_size;
//    vector_context->vec_end = new_vec_start + new_vec_cap * element_size;
//  }
//}
// void VecClear(VectorContext *vector_context, ObjectsHandler *objects_handler)
// {
//  auto *vec_finish = vector_context->vec_finish;
//  auto *vec_start = vector_context->vec_start;
//  if (vec_start != vec_finish) {
//    objects_handler->Destroy(vec_start, vec_finish);
//    vector_context->vec_finish = vec_start;
//  }
//}
// bool VecAdd(VectorContext *vector_context, char *pos, const void *value,
//            unsigned element_size, ObjectsHandler *objects_handler) {
//  auto offset = pos - vector_context->vec_start;
//  if (!VecReserveAtPosition(vector_context, pos, 1, element_size))
//    return false;
//  pos = vector_context->vec_start + offset;
//  objects_handler->UninitializedFill(pos, 1, value);
//  return true;
//}
// bool VecAdd(VectorContext *vector_context, char *pos,
//            unsigned int element_count, const void *value,
//            unsigned element_size, ObjectsHandler *objects_handler) {
//  if (element_count > 0) {
//    auto offset = pos - vector_context->vec_start;
//    if (!VecReserveAtPosition(vector_context, pos, element_count,
//    element_size))
//      return false;
//    pos = vector_context->vec_start + offset;
//    objects_handler->UninitializedFill(pos, element_count, value);
//  }
//  return true;
//}
// bool VecAdd(VectorContext *vector_context, char *pos, const void *first,
//            const void *last, unsigned int element_size,
//            ObjectsHandler *objects_handler) {
//  if (first != last) {
//    auto offset = pos - vector_context->vec_start;
//    auto element_count =
//        ((const char *)last - (const char *)first) / element_size;
//    if (!VecReserveAtPosition(vector_context, pos, element_count,
//    element_size))
//      return false;
//    pos = vector_context->vec_start + offset;
//    objects_handler->UninitializedCopy(pos, first, last);
//  }
//  return true;
//}
// bool VecAdd(VectorContext *vector_context, char *pos, PlainIterable *first,
//            const PlainIterable &last, unsigned int element_size,
//            ObjectsHandler *objects_handler) {
//  if (!(*first).Equal(last)) {
//    auto offset = pos - vector_context->vec_start;
//    auto element_count = last - (*first);
//    if (!VecReserveAtPosition(vector_context, pos, element_count,
//    element_size))
//      return false;
//    pos = vector_context->vec_start + offset;
//    NormalPointerIterable it(pos, element_size);
//    objects_handler->UninitializedCopy(&it, first, last);
//  }
//  return true;
//}
// bool VecMoveAssign(VectorContext *vector_context, void *first, void *last,
//                   unsigned int element_size, ObjectsHandler *objects_handler)
//                   {
//  auto *vec_start = vector_context->vec_start;
//  auto *vec_finish = vector_context->vec_finish;
//  auto vec_size = (vec_finish - vec_start) / element_size;
//  auto vec_cap = (vector_context->vec_end - vec_start) / element_size;
//  auto element_count = ((char *)last - (char *)first) / element_size;
//  if (element_count == 0) {
//    objects_handler->Destroy(vec_start, vec_finish);
//    vector_context->vec_finish = vec_start;
//  } else if (element_count <= vec_size) {
//    vector_context->vec_finish = vec_start + element_count * element_size;
//    objects_handler->Move(vec_start, first, last);
//    if (element_count < vec_size) {
//      objects_handler->Destroy(vector_context->vec_finish, vec_finish);
//    }
//  } else if (element_count <= vec_cap) {
//    void *middle = (char *)first + vec_size * element_size;
//    objects_handler->Move(vec_start, first, middle);
//    objects_handler->UninitializedMove(vec_finish, middle, last);
//    vector_context->vec_finish = vec_start + element_count * element_size;
//  } else {
//    auto new_vec_cap = VecGrow(vec_size, element_count - vec_size, vec_cap);
//    auto *allocator = vector_context->vec_allocator;
//    auto *new_vec_start =
//        (char *)allocator->Allocate(new_vec_cap * element_size);
//    if (!new_vec_start) {
//      return false;
//    }
//    objects_handler->UninitializedMove(new_vec_start, first, last);
//    objects_handler->Destroy(vec_start, vec_finish);
//    allocator->Deallocate(vec_start, vec_cap * element_size);
//    vector_context->vec_start = new_vec_start;
//    vector_context->vec_finish = new_vec_start + element_count * element_size;
//    vector_context->vec_end = new_vec_start + new_vec_cap * element_size;
//  }
//  return true;
//}
// bool VecMoveAssign(VectorContext *vector_context, PlainIterable *first,
//                   const PlainIterable &last, unsigned int element_size,
//                   ObjectsHandler *objects_handler) {
//  auto *vec_start = vector_context->vec_start;
//  auto *vec_finish = vector_context->vec_finish;
//  auto vec_size = (vec_finish - vec_start) / element_size;
//  auto vec_cap = (vector_context->vec_end - vec_start) / element_size;
//  auto element_count = last - (*first);
//  if (element_count == 0) {
//    objects_handler->Destroy(vec_start, vec_finish);
//    vector_context->vec_finish = vec_start;
//  } else if (element_count <= vec_size) {
//    vector_context->vec_finish = vec_start + element_count * element_size;
//    NormalPointerIterable it(vec_start, element_size);
//    objects_handler->Move(&it, first, last);
//    if (element_count < vec_size) {
//      objects_handler->Destroy(vector_context->vec_finish, vec_finish);
//    }
//  } else if (element_count <= vec_cap) {
//    NormalPointerIterable it(vec_start, element_size);
//    objects_handler->Move(&it, NormalPointerIterable(vec_finish,
//    element_size),
//                          first, last);
//    objects_handler->UninitializedMove(&it, first, last);
//    vector_context->vec_finish = vec_start + element_count * element_size;
//  } else {
//    auto new_vec_cap = VecGrow(vec_size, element_count - vec_size, vec_cap);
//    auto *allocator = vector_context->vec_allocator;
//    auto *new_vec_start =
//        (char *)allocator->Allocate(new_vec_cap * element_size);
//    if (!new_vec_start) {
//      return false;
//    }
//    NormalPointerIterable it(new_vec_start, element_size);
//    objects_handler->UninitializedMove(&it, first, last);
//    objects_handler->Destroy(vec_start, vec_finish);
//    allocator->Deallocate(vec_start, vec_cap * element_size);
//    vector_context->vec_start = new_vec_start;
//    vector_context->vec_finish = new_vec_start + element_count * element_size;
//    vector_context->vec_end = new_vec_start + new_vec_cap * element_size;
//  }
//  return true;
//}
// void *VecRemove(VectorContext *vector_context, char *pos,
//                unsigned int element_size, ObjectsHandler *objects_handler) {
//  auto *vec_start = vector_context->vec_start;
//  auto *vec_finish = vector_context->vec_finish;
//  if (pos >= vec_start && pos <= vec_finish - element_size) {
//    objects_handler->Destroy(pos, pos + element_size);
//    memcpy(pos, pos + element_size, vec_finish - pos - element_size);
//    vector_context->vec_finish = vec_finish - element_size;
//    if (pos == vec_finish) {
//      pos -= element_size;
//    }
//  }
//  return pos;
//}
// void VecRemove(VectorContext *vector_context, char *first, char *last,
//               ObjectsHandler *objects_handler) {
//  auto *vec_start = vector_context->vec_start;
//  auto *vec_finish = vector_context->vec_finish;
//  if (first == vec_start && last == vec_finish) {
//    objects_handler->Destroy(vec_start, vec_finish);
//    vector_context->vec_finish = vec_start;
//  } else if (first != last && first >= vec_start && last <= vec_finish) {
//    objects_handler->Destroy(first, last);
//    memcpy(first, last, vec_finish - last);
//    vector_context->vec_finish = vec_finish - (last - first);
//  }
//}
// void VecResize(VectorContext *vector_context, unsigned int n, const void
// *value,
//               unsigned int element_size, ObjectsHandler *objects_handler) {
//  if (n == 0) {
//    VecClear(vector_context, objects_handler);
//    return;
//  }
//  auto *vec_start = vector_context->vec_start;
//  auto *vec_finish = vector_context->vec_finish;
//  auto vec_size = (vec_finish - vec_start) / element_size;
//  if (n == vec_size) {
//    return;
//  }
//  if (n < vec_size) {
//    auto *new_vec_finish = vec_start + n * element_size;
//    objects_handler->Destroy(new_vec_finish, vec_finish);
//    vector_context->vec_finish = new_vec_finish;
//  } else {
//    auto vec_cap = (vector_context->vec_end - vec_start) / element_size;
//    if (n <= vec_cap) {
//      auto *new_vec_finish = vec_start + n * element_size;
//      objects_handler->UninitializedFill(vec_finish, n - vec_size, value);
//      vector_context->vec_finish = new_vec_finish;
//    } else {
//      auto new_vec_cap = VecGrow(vec_size, n - vec_size, vec_cap);
//      auto *allocator = vector_context->vec_allocator;
//      auto *new_vec_start =
//          (char *)allocator->Allocate(new_vec_cap * element_size);
//      if (!new_vec_start) {
//        return;
//      }
//      memcpy(new_vec_start, vec_start, vec_finish - vec_start);
//      objects_handler->UninitializedFill(
//          new_vec_start + (vec_finish - vec_start), n - vec_size, value);
//      allocator->Deallocate(vec_start, vec_cap * element_size);
//      vector_context->vec_start = new_vec_start;
//      vector_context->vec_finish = new_vec_start + n * element_size;
//      vector_context->vec_end = new_vec_start + new_vec_cap * element_size;
//    }
//  }
//}
// void VecReserve(VectorContext *vector_context, unsigned int n,
//                unsigned int element_size) {
//  auto *vec_start = vector_context->vec_start;
//  auto *vec_finish = vector_context->vec_finish;
//  auto vec_size = (vec_finish - vec_start) / element_size;
//  auto vec_cap = (vector_context->vec_end - vec_start) / element_size;
//  if (n <= vec_size || n <= vec_cap) {
//    return;
//  }
//  auto new_vec_cap = VecGrow(vec_size, n - vec_size, vec_cap);
//  auto *allocator = vector_context->vec_allocator;
//  auto *new_vec_start = (char *)allocator->Allocate(new_vec_cap *
//  element_size); if (!new_vec_start) {
//    return;
//  }
//  memcpy(new_vec_start, vec_start, vec_finish - vec_start);
//  allocator->Deallocate(vec_start, vec_cap * element_size);
//  vector_context->vec_start = new_vec_start;
//  vector_context->vec_finish = new_vec_start + n * element_size;
//  vector_context->vec_end = new_vec_start + new_vec_cap * element_size;
//}
// bool VecMoveAdd(VectorContext *vector_context, char *pos, void *value,
//                unsigned int element_size, ObjectsHandler *objects_handler) {
//  auto *vec_start = vector_context->vec_start;
//  auto *vec_finish = vector_context->vec_finish;
//  if (!(pos >= vec_start && pos <= vec_finish)) {
//    return false;
//  }
//  auto vec_size = (vec_finish - vec_start) / element_size;
//  auto vec_cap = (vector_context->vec_end - vec_start) / element_size;
//  if (vec_size < vec_cap) {
//    if (pos != vec_finish) {
//      memmove(pos + element_size, pos, vec_finish - pos);
//    }
//    objects_handler->UninitializedMove(pos, value,
//                                       (char *)value + element_size);
//    vector_context->vec_finish = vec_finish + element_size;
//  } else {
//    auto new_vec_cap = vec_cap << 1;
//    auto *allocator = vector_context->vec_allocator;
//    auto *new_vec_start =
//        (char *)allocator->Allocate(new_vec_cap * element_size);
//    if (!new_vec_start) {
//      return false;
//    }
//    auto offset = pos - vec_start;
//    memcpy(new_vec_start, vec_start, offset);
//    objects_handler->UninitializedMove(new_vec_start + offset, value,
//                                       (char *)value + element_size);
//    memcpy(new_vec_start + offset + element_size, pos, vec_finish - pos);
//    allocator->Deallocate(vec_start, vec_cap * element_size);
//    vector_context->vec_start = new_vec_start;
//    vector_context->vec_finish = new_vec_start + (vec_size + 1) *
//    element_size; vector_context->vec_end = new_vec_start + new_vec_cap *
//    element_size;
//  }
//  return true;
//}
// bool VecMoveAdd(VectorContext *vector_context, char *pos, void *first,
//                void *last, unsigned int element_size,
//                ObjectsHandler *objects_handler) {
//  auto *vec_start = vector_context->vec_start;
//  auto *vec_finish = vector_context->vec_finish;
//  if (!(first != last && pos >= vec_start && pos <= vec_finish))
//    return false;
//  auto vec_size = (vec_finish - vec_start) / element_size;
//  auto vec_cap = (vector_context->vec_end - vec_start) / element_size;
//  auto element_count = ((char *)last - (char *)first) / element_size;
//  if (vec_size <= vec_cap - element_count) {
//    if (pos != vec_finish) {
//      memmove(pos + element_count * element_size, pos, vec_finish - pos);
//    }
//    objects_handler->UninitializedMove(pos, first, last);
//    vector_context->vec_finish = vec_finish + ((char *)last - (char *)first);
//  } else {
//    auto new_vec_cap = VecGrow(vec_size, element_count, vec_cap);
//    auto *allocator = vector_context->vec_allocator;
//    auto *new_vec_start =
//        (char *)allocator->Allocate(new_vec_cap * element_size);
//    if (!new_vec_start) {
//      return false;
//    }
//    auto offset = pos - vec_start;
//    memcpy(new_vec_start, vec_start, offset);
//    objects_handler->UninitializedMove(new_vec_start + offset, first, last);
//    memcpy(new_vec_start + offset + ((char *)last - (char *)first), pos,
//           vec_finish - pos);
//    allocator->Deallocate(vec_start, vec_cap * element_size);
//    vector_context->vec_start = new_vec_start;
//    vector_context->vec_finish =
//        new_vec_start + (vec_size + element_count) * element_size;
//    vector_context->vec_end = new_vec_start + new_vec_cap * element_size;
//  }
//  return true;
//}
// bool VecMoveAdd(VectorContext *vector_context, char *pos, PlainIterable
// *first,
//                const PlainIterable &last, unsigned int element_size,
//                ObjectsHandler *objects_handler) {
//  auto *vec_start = vector_context->vec_start;
//  auto *vec_finish = vector_context->vec_finish;
//  if (last.Equal(*first) || pos < vec_start || pos > vec_finish)
//    return false;
//  auto vec_size = (vec_finish - vec_start) / element_size;
//  auto vec_cap = (vector_context->vec_end - vec_start) / element_size;
//  auto element_count = last - (*first);
//  auto add_bytes = element_count * element_size;
//  if (vec_size <= vec_cap - element_count) {
//    if (pos != vec_finish) {
//      memmove(pos + add_bytes, pos, vec_finish - pos);
//    }
//    NormalPointerIterable it(pos, element_size);
//    objects_handler->UninitializedMove(&it, first, last);
//    vector_context->vec_finish = vec_finish + add_bytes;
//  } else {
//    auto new_vec_cap = VecGrow(vec_size, element_count, vec_cap);
//    auto *allocator = vector_context->vec_allocator;
//    auto *new_vec_start =
//        (char *)allocator->Allocate(new_vec_cap * element_size);
//    if (!new_vec_start) {
//      return false;
//    }
//    auto offset = pos - vec_start;
//    memcpy(new_vec_start, vec_start, offset);
//    NormalPointerIterable it(new_vec_start + offset, element_size);
//    objects_handler->UninitializedMove(&it, first, last);
//    memcpy(new_vec_start + offset + add_bytes, pos, vec_finish - pos);
//    allocator->Deallocate(vec_start, vec_cap * element_size);
//    vector_context->vec_start = new_vec_start;
//    vector_context->vec_finish =
//        new_vec_start + vec_size * element_size + add_bytes;
//    vector_context->vec_end = new_vec_start + new_vec_cap * element_size;
//  }
//  return true;
//}
// bool VecPush(VectorContext *vector_context, const void *value,
//             unsigned int element_size, ObjectsHandler *objects_handler) {
//  auto *vec_start = vector_context->vec_start;
//  auto *vec_finish = vector_context->vec_finish;
//  auto vec_size = (vec_finish - vec_start) / element_size;
//  auto vec_cap = (vector_context->vec_end - vec_start) / element_size;
//  if (vec_size < vec_cap) {
//    objects_handler->UninitializedFill(vec_finish, 1, value);
//    vector_context->vec_finish = vec_finish + element_size;
//  } else {
//    auto new_vec_cap = vec_cap << 1;
//    auto *allocator = vector_context->vec_allocator;
//    auto *new_vec_start =
//        (char *)allocator->Allocate(new_vec_cap * element_size);
//    if (!new_vec_start) {
//      return false;
//    }
//    memcpy(new_vec_start, vec_start, vec_finish - vec_start);
//    allocator->Deallocate(vec_start, vec_cap * element_size);
//    objects_handler->UninitializedFill(new_vec_start + (vec_finish -
//    vec_start),
//                                       1, value);
//    vector_context->vec_start = new_vec_start;
//    vector_context->vec_finish = new_vec_start + (vec_size + 1) *
//    element_size; vector_context->vec_end = new_vec_start + new_vec_cap *
//    element_size;
//  }
//  return true;
//}
// bool VecPop(VectorContext *vector_context, unsigned int element_size,
//            ObjectsHandler *objects_handler) {
//  auto *vec_start = vector_context->vec_start;
//  auto *vec_finish = vector_context->vec_finish;
//  auto vec_size = (vec_finish - vec_start) / element_size;
//  if (vec_size) {
//    objects_handler->Destroy(vec_finish - element_size, vec_finish);
//    vector_context->vec_finish = vec_finish - element_size;
//    return true;
//  } else {
//    return false;
//  }
//}
// bool VecMovePush(VectorContext *vector_context, void *value,
//                 unsigned element_size, ObjectsHandler *objects_handler) {
//  auto *vec_start = vector_context->vec_start;
//  auto *vec_finish = vector_context->vec_finish;
//  auto vec_size = (vec_finish - vec_start) / element_size;
//  auto vec_cap = (vector_context->vec_end - vec_start) / element_size;
//  if (vec_size < vec_cap) {
//    objects_handler->UninitializedMove(vec_finish, value,
//                                       (char *)value + element_size);
//    vector_context->vec_finish = vec_finish + element_size;
//  } else {
//    auto new_vec_cap = vec_cap << 1;
//    auto *allocator = vector_context->vec_allocator;
//    auto *new_vec_start =
//        (char *)allocator->Allocate(new_vec_cap * element_size);
//    if (!new_vec_start) {
//      return false;
//    }
//    memcpy(new_vec_start, vec_start, vec_finish - vec_start);
//    allocator->Deallocate(vec_start, vec_cap * element_size);
//    objects_handler->UninitializedMove(new_vec_start + (vec_finish -
//    vec_start),
//                                       value, (char *)value + element_size);
//    vector_context->vec_start = new_vec_start;
//    vector_context->vec_finish = new_vec_start + (vec_size + 1) *
//    element_size; vector_context->vec_end = new_vec_start + new_vec_cap *
//    element_size;
//  }
//  return true;
//}
//} // namespace xcl
