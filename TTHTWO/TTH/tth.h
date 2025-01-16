#ifndef TTH_H
#define TTH_H

#include <stdint.h>
#include "TTH/tiger.h"

#ifdef __cplusplus
extern "C" {
#endif

/* algorithm context */
typedef struct tth_ctx
{
  tiger_ctx tiger;       /* context used to hash tree leaves */
  uint64_t block_count;  /* number of processed blocks */
  uint64_t stack[64*3];
} tth_ctx;

/* hash functions */

void tth_init(tth_ctx *ctx);
void tth_update(tth_ctx *ctx, const unsigned char* msg, unsigned size);
void tth_final(tth_ctx *ctx, unsigned char result[64]);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* TTH_H */
