#include <stdio.h>
#include "oml/matrix_util.h"

OML_EXPORT omlMatrix* omluMakeMatrix(const float* v, size_t rows, size_t cols) {
	size_t i, j;
	float* tv;
	omlMatrix* m = omlMatAlloc(rows, cols);
	if (m==NULL)
		return NULL;
	tv = m->value;
	for (i=0; i<rows; ++i)
		for (j=0; j<cols; ++j)
			*tv++ = *v++;
	m->rows = rows;
	m->cols = cols;
	return m;
}

OML_EXPORT void omluPrintMatrix(const omlMatrix* m) {
	size_t i, j;
	const float *v = m->value;
	for (i=0; i<m->rows; ++i) {
		for (j=0; j<m->cols; ++j) {
			if (j>0)
				putchar(' ');
			printf("%.3f", *v++);
		}
		putchar('\n');
	}
}

