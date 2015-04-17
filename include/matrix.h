// Author: superguo
#ifndef _OML_MATRIX_H
#define _OML_MATRIX_H

#include "config.h"

typedef struct omlMatrix {
	float* value;
	size_t rows;
	size_t cols;
} omlMatrix;

OML_IMPORT omlMatrix* omlMatAlloc(size_t rows, size_t cols);
OML_IMPORT void omlMatFree(omlMatrix* m);
OML_IMPORT omlMatrix* omlMatMul_Naive(omlMatrix* a, omlMatrix* b);
#ifdef _OML_SSE
OML_IMPORT omlMatrix* omlMatMul_SSE(omlMatrix* a, omlMatrix* b);
#endif
/*
A , N x M
B , M x L 
R[i,k] = sum{A[i,j] * B[j,k] : j = 0..M-1}
R[i] = sum{A[i,j] * B[j] : j = 0..M-1}
https://github.com/tanner-wauchope/Fast-Matrix-Multiply
*/

#ifdef _OML_SSE
#define omlMatMul omlMatMul_SSE
#else
#define omlMatMul omlMatMul_Naive
#endif

#endif	// _OML_MATRIX_H
