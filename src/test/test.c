#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "oml/matrix_util.h"

static int isSameMatrix(const omlMatrix* a, const omlMatrix* b) {
	const float MAX_ERROR = 1e-4f;
	const size_t size = a->rows * a->cols;
	const float* pA = a->value;
	const float* pEndA = pA + size;
	const float* pB = b->value;

	if (a->rows != b->rows)
		return 0;

	if (a->cols != b->cols)
		return 0;

	for (; pA < pEndA; pA++, pB++) {
		if (-1e-6 < *pB && *pB < 1e-6) {
			if (-1e-6 < *pA && *pA < 1e-6) {
				continue;
			} else {
				return 0;
			}
		} else {
			float rel_err = *pA / *pB - 1.0f;
			if (rel_err >  MAX_ERROR || rel_err < -MAX_ERROR)
				return 0;
		}
	}

	return 1;
}

int assertEqualMatrix(const omlMatrix* a, const omlMatrix* b, const char* file, int lineNo) {
	if (!isSameMatrix(a, b)) {
		fprintf(stderr, "%s(%d): matrix equalation assertion failed.\n", file, lineNo);
		puts("Left matrix:");
		omluPrintMatrix(a);
		puts("Right matrix:");
		omluPrintMatrix(b);
		return 0;
	}
	return 1;
}

#define ASSERT_EQUAL_MATRIX(_A, _B) assertEqualMatrix((_A), (_B), __FILE__, __LINE__)

void test1() {
	const int TEST1_N = 501;
	const int TEST1_M = 6002;
	const int TEST1_L = 703;
	int i;
	omlMatrix* m1 = omlMatAlloc(TEST1_N, TEST1_M);
	omlMatrix* m2 = omlMatAlloc(TEST1_M, TEST1_L);
	omlMatrix* prod1 = omlMatAlloc(TEST1_N, TEST1_L);
	omlMatrix* prod2 = omlMatAlloc(TEST1_N, TEST1_L);
#ifdef _OML_MATRIX_OMP
	omlMatrix* prod3 = omlMatAlloc(TEST1_N, TEST1_L);
	omlMatrix* prod4 = omlMatAlloc(TEST1_N, TEST1_L);
#endif 

	float* v;
	clock_t startTime, endTime;

	v = m1->value;
	for (i=0; i<TEST1_N * TEST1_M; ++i)
		*v++ = (float)rand() / ( rand() + 1);

	v = m2->value;
	for (i=0; i<TEST1_M * TEST1_L; ++i)
		*v++ = (float)rand() / ( rand() + 1);

	startTime = clock();
	omlMatMul_Naive(prod1, m1, m2);
	endTime = clock();
//	omluPrintMatrix(prod1);
	printf("Elapsed of MUL(naive) %ld msec\n", endTime - startTime);

	startTime = clock();
	omlMatMul_SSE(prod2, m1, m2);
	endTime = clock();
//	omluPrintMatrix(prod2);
	ASSERT_EQUAL_MATRIX(prod2, prod1);
	printf("Elapsed of MUL(SSE) %ld msec\n", endTime - startTime);
	omlMatFree(prod2);

#ifdef _OML_MATRIX_OMP
	startTime = clock();
	omlMatMul_OMP(prod3, m1, m2);
	endTime = clock();
//	omluPrintMatrix(prod3);
	ASSERT_EQUAL_MATRIX(prod3, prod1);
	printf("Elapsed of MUL(OMP) %ld msec\n", endTime - startTime);
	omlMatFree(prod3);

	startTime = clock();
	omlMatMul_SSE_OMP(prod4, m1, m2);
	endTime = clock();
	ASSERT_EQUAL_MATRIX(prod4, prod1);

//	omluPrintMatrix(prod4);
	printf("Elapsed of MUL(SSE+OMP) %ld msec\n", endTime - startTime);
	omlMatFree(prod4);
#endif 

	omlMatFree(prod1);
	omlMatFree(m1);
	omlMatFree(m2);
}

void test2() {
	const int TEST2_N = 5000;
	const int TEST2_M = 6000;
	int i;
	omlMatrix* m1 = omlMatAlloc(TEST2_N, TEST2_M);
	omlMatrix* m2 = omlMatAlloc(TEST2_N, TEST2_M);
	omlMatrix* sum1 = omlMatAlloc(TEST2_N, TEST2_M);
	omlMatrix* sum2 = omlMatAlloc(TEST2_N, TEST2_M);
	float* v;
	clock_t startTime, endTime;

	v = m1->value;
	for (i=0; i<TEST2_N * TEST2_M; ++i)
		*v++ = (float)rand() / ( rand() + 1);

	v = m2->value;
	for (i=0; i<TEST2_N * TEST2_M; ++i)
		*v++ = (float)rand() / ( rand() + 1);

	startTime = clock();
	omlMatAdd_Naive(sum1, m1, m2);
	endTime = clock();
//	omluPrintMatrix(sum1);
	printf("Elapsed of ADD(naive) %ld msec\n", endTime - startTime);

	startTime = clock();
	omlMatAdd_SSE(sum2, m1, m2);
	endTime = clock();
//	omluPrintMatrix(sum2);
	ASSERT_EQUAL_MATRIX(sum1, sum2);
	printf("Elapsed of ADD(SSE) %ld msec\n", endTime - startTime);
	omlMatFree(sum2);

	omlMatFree(sum1);
}

void test3() {
	const int TEST3_N = 5001;
	const int TEST3_M = 6001;
	int i;
	omlMatrix* m1 = omlMatAlloc(TEST3_N, TEST3_M);
	omlMatrix* prod1 = omlMatAlloc(TEST3_N, TEST3_M);
	omlMatrix* prod2 = omlMatAlloc(TEST3_N, TEST3_M);
	omlMatrix* prod3 = omlMatAlloc(TEST3_N, TEST3_M);
	float a = (float)rand() / ( rand() + 1);
	float* v;
	clock_t startTime, endTime;

	v = m1->value;
	for (i=0; i<TEST3_N * TEST3_M; ++i)
		*v++ = (float)rand() / ( rand() + 1);

	startTime = clock();
	omlMatMul1_Naive(prod1, a, m1);
	endTime = clock();
//	omluPrintMatrix(sum1);
	printf("Elapsed of MUL1(naive) %ld msec\n", endTime - startTime);

	startTime = clock();
	omlMatMul1_SSE(prod2, a, m1);
	endTime = clock();
//	omluPrintMatrix(sum2);
	ASSERT_EQUAL_MATRIX(prod1, prod2);
	printf("Elapsed of MUL1(SSE) %ld msec\n", endTime - startTime);
	omlMatFree(prod2);

	startTime = clock();
	omlMatMul1_SSE_OMP(prod3, a, m1);
	endTime = clock();
//	omluPrintMatrix(sum2);
	ASSERT_EQUAL_MATRIX(prod1, prod3);
	printf("Elapsed of MUL1(SSE+OMP) %ld msec\n", endTime - startTime);
	omlMatFree(prod3);

	omlMatFree(prod1);
}

int main() {
	test2();
	test3();
	test1();
	return 0;
}
