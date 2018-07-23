#pragma once

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdlib.h>
#include <assert.h>

#define kvec_roundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))

#define KVEC_TYPE(type)  \
	typedef  struct {\
		size_t n, m; \
		type *a;\
	} kvec_##type##_t;

#define kvec_t(type) kvec_##type##_t
#define kvec_size(v) ((v).n)
#define kvec_max(v) ((v).m)
#define kvec_A(v, i) ((v).a[(i)])

#define KVEC_IMPL(type,SCOPE)\
	SCOPE int kvec_init_##type(kvec_##type##_t *v)\
	{\
		assert(v != NULL);\
		v->n=0;\
		v->m=0;\
		v->a=(type*)malloc(4*sizeof(type));\
		if(v->a == NULL) return -1;\
		return 0;\
	}\
	SCOPE int kvec_resize_##type(kvec_##type##_t *v,size_t s)\
    {\
		assert(v != NULL);\
    	v->m = s;\
    	v->a = (type*)realloc(v->a,sizeof(type)*(v->m));\
    	if(v->a == NULL) return -1;\
    	return 0;\
    }\
	SCOPE void kvec_destroy_##type(kvec_##type##_t *v)\
	{\
		assert(v != NULL);\
		free(v->a); \
	}\
	SCOPE void kvec_clean_##type(kvec_##type##_t *v)\
	{\
		assert(v != NULL);\
		v->n = 0;\
		return;\
	}\
	SCOPE int kvec_push_##type(kvec_##type##_t *v,type x)\
	{\
		assert(v != NULL);\
		if(v->n == v->m){\
			v->m = v->m ? v->m << 1 : 2;\
			v->a = (type*)realloc(v->a,sizeof(type)*(v->m));\
			if(v->a == NULL) return -1;\
        }\
        v->a[v->n ++] = x;\
        return 0;\
	}\
	SCOPE int kvec_pushp_##type(kvec_##type##_t *v,type **p)\
	{\
		assert(v != NULL);\
		assert(p != NULL);\
		if(v->n == v->m){\
			v->m = v->m ? v->m << 1 : 2;\
			v->a = (type*)realloc(v->a,sizeof(type)*(v->m));\
			if(v->a == NULL) return -1;\
        }\
		(*p) = &(v->a[v->n ++]) ;\
        return 0;\
	}\
	SCOPE int kvec_pop_##type(kvec_##type##_t *v,type *p)\
	{\
		assert(v != NULL);\
		if(v->n == 0) return -1;\
		if(p != NULL)\
            *p = v->a[v->n];\
        v->n -= 1;\
        return 0;\
	}\
	SCOPE int kvec_copy_##type(kvec_##type##_t *target,kvec_##type##_t *source)\
	{\
		assert(target != NULL);\
		assert(source != NULL);\
		int result;\
		if ((target->m) < (source->n)){\
            result = kvec_resize_##type(target,(source->n));\
            if(result != 0) return -1;\
        }\
        target->n = source->n;\
        memcpy(target->a,source->a,sizeof(type)*(source->n));\
        return 0;\
	}\
	SCOPE int kvec_a_##type(kvec_##type##_t *v,size_t i,type a)\
	{\
		assert(v != NULL);\
		if(v->m <= i){\
            v->m = v->n = i+1;\
            kvec_roundup32(v->m);\
            v->a = (type*)realloc(v->a,sizeof(type)*(v->m));\
            if(v->a == NULL) return -1;\
        }\
        else{\
        	if(v->n <= i) v->n = i+1;\
        }\
        (v->a[i]) = a;\
        return 0;\
	}\
	SCOPE int kvec_ap_##type(kvec_##type##_t *v,size_t i,type** p)\
	{\
		assert(v != NULL);\
		assert(p != NULL);\
		if(v->m <= i){\
            v->m = v->n = i+1;\
            kvec_roundup32(v->m);\
            v->a = (type*)realloc(v->a,sizeof(type)*(v->m));\
            if(v->a == NULL) return -1;\
        }\
        else{\
        	if(v->n <= i) v->n = i+1;\
        }\
        *p = &(v->a[i]);\
        return 0;\
	}\

#define KVEC_INIT2(type, SCOPE) \
	KVEC_TYPE(type) \
	KVEC_IMPL(type, SCOPE)

#ifndef klib_unused
#if (defined __clang__ && __clang_major__ >= 3) || (defined __GNUC__ && __GNUC__ >= 3)
#define klib_unused __attribute__ ((__unused__))
#else
#define klib_unused
#endif
#endif /* klib_unused */

#define KVEC_INIT(type) KVEC_INIT2(type, static inline klib_unused)

#define KVEC_DECLARE(type,SCOPE)\
	KVEC_TYPE(type)\
	SCOPE int kvec_init_##type(kvec_##type##_t *v);\
	SCOPE int kvec_resize_##type(kvec_##type##_t *v,size_t s);\
	SCOPE void kvec_destroy_##type(kvec_##type##_t *v);\
	SCOPE void kvec_clean_##type(kvec_##type##_t *v);\
	SCOPE int kvec_push_##type(kvec_##type##_t *v,type x);\
	SCOPE int kvec_pushp_##type(kvec_##type##_t *v,type **p);\
	SCOPE int kvec_pop_##type(kvec_##type##_t *v,type *p);\
	SCOPE int kvec_copy_##type(kvec_##type##_t *target,kvec_##type##_t *source);\
	SCOPE int kvec_ap_##type(kvec_##type##_t *v,size_t i,type** p);

#ifdef __cplusplus
}
#endif // __cplusplus
