/**
 * @author xuyan
 * @brief simple c rbtree implementation with high performance
 * @version 0.1
 * @date 2022-08-05
 */

#include "xcl/lang/XclDef.h"

typedef enum
{
    RED,
    BLACK
} CRbColor;

typedef struct _CRbNode_st CRbNode;
struct _CRbNode_st {
    CRbNode *left, *right, *par;
    char attach[0];
};