#include "oml/matrix.h"
#include <string.h>
#ifdef _OML_MATRIX_SSE
#include <emmintrin.h>
#endif

OML_EXPORT omlMatrix* omlMatAlloc(size_t rows, size_t cols) {
	// Force to add 16 bytes-aligned to make it easy to do multiply
	size_t alloc_size = rows * cols * sizeof(float) + 4 * sizeof(size_t);
	omlMatrix* result = (omlMatrix*) oml_aligned_malloc(alloc_size, 16);
	if (result == NULL)
		return NULL;
	memset(result, 0, alloc_size);
	result->rows = rows;
	result->cols = cols;
	return result;
}

OML_EXPORT void omlMatFree(omlMatrix* m) {
	oml_aligned_free(m);
}

OML_EXPORT omlMatrix* omlMatZero(omlMatrix* a) {
	memset(a->value, 0, a->rows * a->cols * sizeof(float));
	return a;
}

OML_EXPORT omlMatrix* omlMatIdent(omlMatrix* a) {
	const size_t n = a->rows;
	const size_t size = n * n;
	float* A = a->value;
	size_t i;
	
	assert(n == a->cols);

	omlMatZero(a);
	for (i=0; i<size; i+=n+1) {
		A[i] = 0.0f;
	}

	return a;
}

OML_EXPORT omlMatrix* omlMatTr(omlMatrix* ta, const omlMatrix* a) {
	const float* A = a->value;
	float* TA = ta->value;
	const int ta_n = ta->rows;
	const int ta_m = ta->cols;
	int i, j;

	assert(ta->rows == a->cols);
	assert(ta->cols == a->rows);
	for (j=0; j<ta_m; ++j) 
		for (i=0; i<ta_n; ++i) {
			TA[i*ta_m+j] = *A++;
		}

	return ta;
}

OML_EXPORT omlMatrix* omlMatAdd_Naive(omlMatrix* c, const omlMatrix* a, const omlMatrix* b) {
	const float* A = a->value;
	const float* B = b->value;
	float* C = c->value;
	const int n = a->rows;
	const int m = a->cols;
	const int count = n * m;
	int i;
	
	assert(n == b->rows);
	assert(m == b->cols);

	c->rows = n;
	c->cols = m;
	C = c->value;
	for (i=0; i<count; i++, A++, B++, C++) {
		*C = *A + *B;
	}
	return c;
}

OML_EXPORT omlMatrix* omlMatSub_Naive(omlMatrix* c, const omlMatrix* a, const omlMatrix* b) {
	const float* A = a->value;
	const float* B = b->value;
	float* C = c->value;
	const int n = a->rows;
	const int m = a->cols;
	const int count = n * m;
	int i;
	
	assert(n == b->rows);
	assert(m == b->cols);

	c->rows = n;
	c->cols = m;
	C = c->value;
	for (i=0; i<count; i++, A++, B++, C++) {
		*C = *A - *B;
	}
	return c;
}

OML_EXPORT omlMatrix* omlMatMul_Naive(omlMatrix* c, const omlMatrix* a, const omlMatrix* b) {
	int i, j, k;
	int n = a->rows;
	int m = a->cols;
	int l = b->cols;
	const float* A = a->value;
	const float* B = b->value;
	float* C = c->value;

	assert(m == b->rows);
	assert(c->rows == n);
	assert(c->cols == l);

	C = c->value;
	omlMatZero(c);
	for (i = 0; i < n; ++i) {
		for (j = 0; j < m; ++j) {
			const float A_i_j = A[i * m + j];
			const float* B_j = &B[j*l];
			for (k = 0; k < l; ++k) {
 				C[i * l + k] += A_i_j * B_j[k];
			}
		}
	}
	return c;
}

OML_EXPORT omlMatrix* omlMatMul1_Naive(omlMatrix* c, float a, const omlMatrix* b) {
	const float* B = b->value;
	float* C = c->value;
	const int count = b->rows * b->cols;
	int i;
	assert(c->rows == b->rows);
	assert(c->cols == b->cols);
	for (i=0; i<count; i++, B++, C++) {
		*C = *B * a;
	}
	return c;
}

#ifdef _OML_MATRIX_SSE

OML_EXPORT omlMatrix* omlMatAdd_SSE(omlMatrix* c, const omlMatrix* a, const omlMatrix* b) {
	const float* A = a->value;
	const float* B = b->value;
	float* C = c->value;
	const int n = a->rows;
	const int m = a->cols;
	const int count = n * m;
	int i;

	assert(n == b->rows);
	assert(m == b->cols);

	c->rows = n;
	c->cols = m;
	C = c->value;
	for (i=0; i<count; i+=4, A+=4, B+=4, C+=4) {
		_mm_store_ps(C, _mm_add_ps(_mm_load_ps(A), _mm_load_ps(B)));		
	}
	return c;
}

OML_EXPORT omlMatrix* omlMatSub_SSE(omlMatrix* c, const omlMatrix* a, const omlMatrix* b) {
	const float* A = a->value;
	const float* B = b->value;
	float* C = c->value;
	const int n = a->rows;
	const int m = a->cols;
	const int count = n * m;
	int i;
	
	assert(n == b->rows);
	assert(m == b->cols);

	c->rows = n;
	c->cols = m;
	C = c->value;
	for (i=0; i<count; i+=4, A+=4, B+=4, C+=4) {
		_mm_store_ps(C, _mm_sub_ps(_mm_load_ps(A), _mm_load_ps(B)));		
	}
	return c;
}

OML_EXPORT omlMatrix* omlMatMul_SSE(omlMatrix* c, const omlMatrix* a, const omlMatrix* b) {
	int i, j, k;
	int n = a->rows;
	int m = a->cols;
	int l = b->cols;
	int ll = l >> 2 << 2;
	
	const float* A = a->value;
	const float* B = b->value;
	float* C = c->value;
	__m128 vec_a_i_j;
	__m128 vec_c_i_k;
	const float* b_j;
	float* c_i;

	assert(m == b->rows);
	assert(c->rows == n);
	assert(c->cols == l);

	omlMatZero(c);

	if ((m&3) || (l&3)) {
		for (i = 0; i < n; ++i) {		
			for (j = 0; j < m; j++) {
				vec_a_i_j = _mm_load_ps1(&A[i * m + j]);
				c_i = &C[i * l];
				b_j = &B[j * l];
				for (k = 0; k < ll; k += 4) {
					// C[i * l + k] += A[i * m + j] * B[j * l + k];
					vec_c_i_k = _mm_loadu_ps(c_i + k);
					vec_c_i_k = _mm_add_ps(vec_c_i_k, _mm_mul_ps(vec_a_i_j, _mm_loadu_ps(b_j + k)));
					_mm_storeu_ps(c_i + k, vec_c_i_k);
				}
				if (k < l) {
//					OML_ALIGN16_DECLARE(float edges[4]);
					for (; k < l; ++k) {
						C[i * l + k] += A[i * m + j] * B[j * l + k];
					}
				}

			}
		}
	} else {
		for (i = 0; i < n; ++i) {		
			for (j = 0; j < m; j++) {
				vec_a_i_j = _mm_load_ps1(&A[i * m + j]);
				c_i = &C[i * l];
				b_j = &B[j * l];
				for (k = 0; k < l; k += 4) {
					// C[i * l + k] += A[i * m + j] * B[j * l + k];
					vec_c_i_k = _mm_load_ps(c_i + k);
					vec_c_i_k = _mm_add_ps(vec_c_i_k, _mm_mul_ps(vec_a_i_j, _mm_load_ps(b_j + k)));
					_mm_store_ps(c_i + k, vec_c_i_k);
				}

			}
		}
	}

	return c;
}

OML_EXPORT omlMatrix* omlMatMul1_SSE(omlMatrix* c, float a, const omlMatrix* b) {
	const float* B = b->value;
	__m128 vec_a = _mm_load_ps1(&a);
	float* C = c->value;
	const int count = b->rows * b->cols;
	int i;
	assert(c->rows == b->rows);
	assert(c->cols == b->cols);
	for (i=0; i<count; i+=4, B+=4, C+=4) {
		_mm_store_ps(C,	_mm_mul_ps(vec_a, _mm_load_ps(B)));
	}
	return c;
}
#endif


#ifdef _OML_MATRIX_OMP
OML_EXPORT omlMatrix* omlMatMul_OMP(omlMatrix* c, const omlMatrix* a, const omlMatrix* b) {
	int i, j, k;
	int n = a->rows;
	int m = a->cols;
	int l = b->cols;
	const float* A = a->value;
	const float* B = b->value;
	float* C = c->value;

	assert(m == b->rows);
	assert(c->rows == n);
	assert(c->cols == l);
	omlMatZero(c);
	#pragma omp parallel for schedule(static, 4) private(i,j,k) shared(A,B,C)
	for (i = 0; i < n; ++i) {
		for (j = 0; j < m; ++j) {
			const float A_i_j = A[i * m + j];
			const float* B_j = &B[j*l];
			for (k = 0; k < l; ++k) {
				C[i * l + k] += A_i_j * B_j[k];
			}
		}
	}
	return c;
}
#endif

#if defined(_OML_MATRIX_SSE) && defined(_OML_MATRIX_OMP)
/*
Reference
NxM matrix multiplied by MxN matrix https://github.com/tanner-wauchope/Fast-Matrix-Multiply
*/
OML_EXPORT omlMatrix* omlMatMul_SSE_OMP(omlMatrix* c, const omlMatrix* a, const omlMatrix* b) {
	int i, j, k;
	int n = a->rows;
	int m = a->cols;
	int l = b->cols;
	int ll = l >> 2 << 2;
	
	const float* A = a->value;
	const float* B = b->value;
	float* C = c->value;
	__m128 vec_a_i_j;
	__m128 vec_c_i_k;
	const float* b_j;
	float* c_i;

	assert(m == b->rows);
	assert(c->rows == n);
	assert(c->cols == l);
	omlMatZero(c);
	if ((m&3) || (l&3)) {
#pragma omp parallel for schedule(static, 4) private(i,j,k,vec_a_i_j,c_i,b_j,vec_c_i_k)
		for (i = 0; i < n; ++i) {
			for (j = 0; j < m; j++) {
				vec_a_i_j = _mm_load_ps1(&A[i * m + j]);
				c_i = &C[i * l];
				b_j = &B[j * l];
				for (k = 0; k < ll; k += 4) {
					// C[i * l + k] += A[i * m + j] * B[j * l + k];
					vec_c_i_k = _mm_loadu_ps(c_i + k);
					vec_c_i_k = _mm_add_ps(vec_c_i_k, _mm_mul_ps(vec_a_i_j, _mm_loadu_ps(b_j + k)));
					_mm_storeu_ps(c_i + k, vec_c_i_k);
				}
				if (k < l) {
					// OML_ALIGN16_DECL float edges[4];
					for (; k < l; ++k) {
						C[i * l + k] += A[i * m + j] * B[j * l + k];
					}
				}

			}
		}
	} else {
#pragma omp parallel for schedule(dynamic, 4) private(i,j,k,vec_a_i_j,c_i,b_j,vec_c_i_k)
		for (i = 0; i < n; ++i) {
			for (j = 0; j < m; j++) {
				vec_a_i_j = _mm_load_ps1(&A[i * m + j]);
				c_i = &C[i * l];
				b_j = &B[j * l];
				for (k = 0; k < l; k += 4) {
					// C[i * l + k] += A[i * m + j] * B[j * l + k];
					vec_c_i_k = _mm_load_ps(c_i + k);
					vec_c_i_k = _mm_add_ps(vec_c_i_k, _mm_mul_ps(vec_a_i_j, _mm_load_ps(b_j + k)));
					_mm_store_ps(c_i + k, vec_c_i_k);
				}
			}
		}
	}
	return c;
}

OML_EXPORT omlMatrix* omlMatMul1_SSE_OMP(omlMatrix* c, float a, const omlMatrix* b) {
	const float* B = b->value;
	__m128 vec_a = _mm_load_ps1(&a);
	float* C = c->value;
	const int count = b->rows * b->cols;
	int i;
	assert(c->rows == b->rows);
	assert(c->cols == b->cols);
#pragma omp parallel for schedule(static, 4) private(i)
	for (i=0; i<count; i+=4) {
		_mm_store_ps(&C[i],_mm_mul_ps(vec_a, _mm_load_ps(&B[i])));
	}
	return c;
}

#endif // _OML_MATRIX_SSE && _OML_MATRIX_OMP
