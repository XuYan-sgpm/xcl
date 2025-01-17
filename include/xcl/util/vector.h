//
// Created by 徐琰 on 2023/11/16.
//

#pragma once

#include <stdbool.h>
#include <xcl/lang/define.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Vector Vector;
Vector* Vec_new(unsigned item_size);
Vector* Vec_new2(unsigned item_size, unsigned cap);
void Vec_free(Vector* vec);
bool Vec_push(Vector* vec, const void* item);
bool Vec_pop(Vector* vec, void* item);
unsigned Vec_size(Vector* vec);
unsigned Vec_cap(Vector* vec);
bool Vec_empty(Vector* vec);
bool Vec_addn(Vector* vec, unsigned pos, unsigned n, const void* item);
bool Vec_add(Vector* vec, unsigned pos, const void* start, const void* finish);
bool Vec_remove(Vector* vec, unsigned pos, unsigned n);
bool Vec_filln(Vector* vec, unsigned n, const void* item);
bool Vec_fill(Vector* vec, const void* start, const void* finish);
void Vec_clear(Vector* vec);
bool Vec_reserve(Vector* vec, unsigned size);
Vector* Vec_copy(const Vector* vec);
void* Vec_pos(Vector* vec, unsigned pos);
bool Vec_resize(Vector* vec, unsigned n, const void* item);
void* Vec_back(Vector* vec);

#ifdef __cplusplus
};
#endif
