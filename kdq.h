#pragma once

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define KDQ_TYPE(type) \
	typedef struct { \
		size_t front:58, bits:6, count, mask; \
		type *a; \
	} kdq_##type##_t;

#define kdq_t(type) kdq_##type##_t
#define kdq_size(q) ((q)->count)
#define kdq_first(q) ((q)->a[(q)->front])
#define kdq_last(q) ((q)->a[((q)->front + (q)->count - 1) & (q)->mask])
#define kdq_at(q, i) ((q)->a[((q)->front + (i)) & (q)->mask])

#define KDQ_IMPL(type, SCOPE) \
	SCOPE int kdq_init_##type(kdq_##type##_t *q) \
	{ \
		assert(q != NULL);\
		q->front = 0, q->count = 0, q->bits = 2, q->mask = (1ULL<<q->bits) - 1; \
		q->a = (type*)malloc((1<<q->bits) * sizeof(type)); \
		if(q->a == NULL) return -1;\
		return 0; \
	} \
	\
	SCOPE void kdq_destroy_##type(kdq_##type##_t *q) \
	{ \
		assert(q != NULL);\
		free(q->a); \
	} \
	\
	SCOPE int kdq_resize_##type(kdq_##type##_t *q, int new_bits) \
	{ \
		assert(q != NULL);\
		assert(new_bits < 64);\
		assert(new_bits > 0);\
		size_t new_size = 1ULL<<new_bits, old_size = 1ULL<<q->bits; \
		if (new_size < q->count) { /* not big enough */ \
			int i; \
			for (i = 0; i < 64; ++i) \
				if (1ULL<<i > q->count) break; \
			new_bits = i, new_size = 1ULL<<new_bits; \
		} \
		if (new_bits == q->bits) \
            return 0;\
		if (new_bits > q->bits) {\
            q->a = (type*)realloc(q->a, (1ULL<<new_bits) * sizeof(type)); \
            if(q->a == NULL) return -1;\
        }\
		if (q->front + q->count <= old_size) { /* unwrapped */ \
			if (q->front + q->count > new_size) /* only happens for shrinking */ \
				memmove(q->a, q->a + new_size, (q->front + q->count - new_size) * sizeof(type)); \
		} \
		else { /* wrapped */ \
			memmove(q->a + (new_size - (old_size - q->front)), q->a + q->front, (old_size - q->front) * sizeof(type)); \
			q->front = new_size - (old_size - q->front); \
		} \
		q->bits = new_bits, q->mask = (1ULL<<q->bits) - 1; \
		if (new_bits < q->bits) {\
            q->a = (type*)realloc(q->a, (1ULL<<new_bits) * sizeof(type)); \
            if(q->a == NULL) return -1;\
        }\
		return 0; \
	} \
	\
	SCOPE void kdq_clean_##type(kdq_##type##_t *q)\
    {\
		assert(q != NULL);\
		q->count = 0;\
		q->front = 0;\
		return;\
    }\
    \
	SCOPE int kdq_pushp_##type(kdq_##type##_t *q,type **p) \
	{ \
		assert(q != NULL);\
		int result;\
		if (q->count == 1ULL<<q->bits){ \
            result = kdq_resize_##type(q, q->bits + 1); \
            if( result == -1) return -1;\
        }\
		*p = &q->a[((q->count++) + q->front) & (q)->mask]; \
		return 0;\
	} \
	\
	SCOPE int kdq_push_##type(kdq_##type##_t *q, type v) \
	{ \
		assert(q != NULL);\
		int result;\
		if (q->count == 1ULL<<q->bits) {\
            result = kdq_resize_##type(q, q->bits + 1); \
            if( result == -1) return -1;\
        }\
		q->a[((q->count++) + q->front) & (q)->mask] = v; \
		return 0;\
	} \
	\
	SCOPE int kdq_unshiftp_##type(kdq_##type##_t *q,type **p) \
	{ \
		assert(q != NULL);\
		int result;\
		if (q->count == 1ULL<<q->bits){\
            result = kdq_resize_##type(q, q->bits + 1); \
            if( result == -1) return -1;\
        }\
		++q->count; \
		q->front = q->front? q->front - 1 : (1ULL<<q->bits) - 1; \
		*p = &q->a[q->front]; \
		return 0;\
	} \
	\
	SCOPE int kdq_unshift_##type(kdq_##type##_t *q, type v) \
	{ \
		assert(q != NULL);\
		int result;\
		type *p; \
		result = kdq_unshiftp_##type(q,&p); \
        if( result == -1) return -1;\
		*p = v; \
		return 0;\
	} \
	SCOPE type *kdq_pop_##type(kdq_##type##_t *q) \
	{ \
		assert(q != NULL);\
		return q->count? &q->a[((--q->count) + q->front) & q->mask] : NULL; \
	} \
	SCOPE type *kdq_shift_##type(kdq_##type##_t *q) \
	{ \
		assert(q != NULL);\
		type *d = 0; \
		if (q->count == 0) return 0; \
		d = &q->a[q->front++]; \
		q->front &= q->mask; \
		--q->count; \
		return d; \
	}

#define KDQ_INIT2(type, SCOPE) \
	KDQ_TYPE(type) \
	KDQ_IMPL(type, SCOPE)

#ifndef klib_unused
#if (defined __clang__ && __clang_major__ >= 3) || (defined __GNUC__ && __GNUC__ >= 3)
#define klib_unused __attribute__ ((__unused__))
#else
#define klib_unused
#endif
#endif /* klib_unused */

#define KDQ_INIT(type) KDQ_INIT2(type, static inline klib_unused)

#define KDQ_DECLARE(type) \
	KDQ_TYPE(type) \
    SCOPE int kdq_init_##type(kdq_##type##_t *q) ;\
	SCOPE void kdq_destroy_##type(kdq_##type##_t *q) ;\
	SCOPE int kdq_resize_##type(kdq_##type##_t *q, int new_bits) ;\
	SCOPE void kdq_clean_##type(kdq_##type##_t *q);\
	SCOPE int kdq_pushp_##type(kdq_##type##_t *q,type **p) ;\
	SCOPE int kdq_push_##type(kdq_##type##_t *q, type v) ;\
	SCOPE int kdq_unshiftp_##type(kdq_##type##_t *q,type **p) ;\
	SCOPE int kdq_unshift_##type(kdq_##type##_t *q, type v) ;\
	SCOPE type *kdq_pop_##type(kdq_##type##_t *q) ;\
	SCOPE type *kdq_shift_##type(kdq_##type##_t *q);

#ifdef __cplusplus
}
#endif // __cplusplus
