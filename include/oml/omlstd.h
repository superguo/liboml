#ifndef _OML_STD_H
#define _OML_STD_H

#include "oml/config.h"

#ifdef __cplusplus
	#ifdef _OML_USE_AS_STATIC_LIBRARY
		#define OML_EXPORT
		#define OML_IMPORT extern
	#else
		#define OML_EXPORT extern "C" __declspec(dllexport)
		#define OML_IMPORT extern "C" __declspec(dllimport)
	#endif
#else
	#ifdef _OML_USE_AS_STATIC_LIBRARY
		#define OML_EXPORT
		#define OML_IMPORT extern
	#else
		#define OML_EXPORT __declspec(dllexport)
		#define OML_IMPORT __declspec(dllimport)
	#endif
#endif

#ifdef _OML_USE_STDLIB_ALLOC
	#include <stdlib.h>
	#define oml_malloc malloc
	#define oml_free free
#ifdef _MSC_VER
	#define oml_aligned_malloc _aligned_malloc
	#define oml_aligned_free _aligned_free
#else
	#define oml_aligned_malloc aligned_malloc
	#define oml_aligned_free aligned_free
#endif
#else
	#error define your own memory allocation routines
#endif

#ifdef _OML_USE_ASSERT
	#include <assert.h>
#endif

#ifdef __GNUC__
	#define OML_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
	#define OML_INLINE __forceinline
#else
	#define OML_INLINE static inline
#endif

#ifdef __GNUC__
	#define OML_ALIGN16_DECL __attribute__ ((aligned (16))) 
#elif defined(_MSC_VER)
	#define OML_ALIGN16_DECL __declspec(align(16))
#else
	#error define your own 16 bytes aligned macro
#endif

#define OML_ALIGN(_X, _Y) (_X) & (~(_Y) + 1) 
#define OML_ALIGN_UP(_X, _Y) OML_ALIGN(_X + (_Y) - 1, _Y)

#endif // ! _OML_STD_H
