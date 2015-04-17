// Author: superguo
#ifndef _OML_MATRIX_H
#define _OML_MATRIX_H

#include "oml/omlstd.h"

typedef struct omlMatrix {
	size_t rows;
	size_t cols;
	size_t padding1;
	size_t padding2;
	float value[0];
} omlMatrix;

OML_IMPORT omlMatrix* omlMatAlloc(size_t rows, size_t cols);
OML_IMPORT void omlMatFree(omlMatrix* m);

OML_IMPORT omlMatrix* omlMatZero(omlMatrix* a);	// Make a zero matrix
OML_IMPORT omlMatrix* omlMatIdent(omlMatrix* a);// Make an identity matrix
OML_IMPORT omlMatrix* omlMatTr(omlMatrix* ta, const omlMatrix* a);	// tranpose 
OML_INLINE omlMatrix* omlMatAdd(omlMatrix* c, const omlMatrix* a, const omlMatrix* b);	// c = a + b
OML_INLINE omlMatrix* omlMatSub(omlMatrix* c, const omlMatrix* a, const omlMatrix* b);	// c = a - b
OML_INLINE omlMatrix* omlMatMul(omlMatrix* c, const omlMatrix* a, const omlMatrix* b);	// c = a * b
OML_INLINE omlMatrix* omlMatMul1(omlMatrix* c, float a, const omlMatrix* b);	// c = a * b (scalor multiplication)

// Implementations for Naive/OMP/SSE  
OML_IMPORT omlMatrix* omlMatAdd_Naive(omlMatrix* c, const omlMatrix* a, const omlMatrix* b);
OML_IMPORT omlMatrix* omlMatSub_Naive(omlMatrix* c, const omlMatrix* a, const omlMatrix* b);
OML_IMPORT omlMatrix* omlMatMul_Naive(omlMatrix* c, const omlMatrix* a, const omlMatrix* b);
OML_IMPORT omlMatrix* omlMatMul1_Naive(omlMatrix* c, float a, const omlMatrix* b);

#ifdef _OML_MATRIX_SSE
OML_IMPORT omlMatrix* omlMatAdd_SSE(omlMatrix* c, const omlMatrix* a, const omlMatrix* b);
OML_IMPORT omlMatrix* omlMatSub_SSE(omlMatrix* c, const omlMatrix* a, const omlMatrix* b);
OML_IMPORT omlMatrix* omlMatMul_SSE(omlMatrix* c, const omlMatrix* a, const omlMatrix* b);
OML_IMPORT omlMatrix* omlMatMul1_SSE(omlMatrix* c, float a, const omlMatrix* b);
#endif

#ifdef _OML_MATRIX_OMP
OML_IMPORT omlMatrix* omlMatMul_OMP(omlMatrix* c, const omlMatrix* a, const omlMatrix* b);
#endif

#if defined(_OML_MATRIX_SSE) && defined(_OML_MATRIX_OMP)
OML_IMPORT omlMatrix* omlMatMul_SSE_OMP(omlMatrix* c, const omlMatrix* a, const omlMatrix* b);
OML_IMPORT omlMatrix* omlMatMul1_SSE_OMP(omlMatrix* c, float a, const omlMatrix* b);
#endif

// Definitions of inline functions

OML_INLINE omlMatrix* omlMatAdd(omlMatrix* c, const omlMatrix* a, const omlMatrix* b) {
#ifdef _OML_MATRIX_SSE
	return omlMatAdd_SSE(c, a, b);
#else
	return omlMatAdd_Naive(c, a, b);
#endif
}

OML_INLINE omlMatrix* omlMatSub(omlMatrix* c, const omlMatrix* a, const omlMatrix* b) {
#ifdef _OML_MATRIX_SSE
	return omlMatSub_SSE(c, a, b);
#else
	return omlMatSub_Naive(c, a, b);
#endif
}

OML_INLINE omlMatrix* omlMatMul(omlMatrix* c, const omlMatrix* a, const omlMatrix* b) {
#if defined(_OML_MATRIX_SSE) && defined(_OML_MATRIX_OMP)
	return omlMatMul_SSE_OMP(c, a, b);
#elif defined(_OML_MATRIX_SSE)
	return omlMatMul_SSE(c, a, b)
#else
	return omlMatMul_OMP(c, a, b);
#endif
}

OML_INLINE omlMatrix* omlMatMul1(omlMatrix* c, float a, const omlMatrix* b) {
#if defined(_OML_MATRIX_SSE) && defined(_OML_MATRIX_OMP)
	return omlMatMul1_SSE_OMP(c, a, b);
#elif defined(_OML_MATRIX_SSE)
	return omlMatMul1_SSE(c, a, b)
#else
	return omlMatMul1_Naive(c, a, b);
#endif
}

#endif	// _OML_MATRIX_H
