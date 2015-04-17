#ifndef _MATRIX_UTIL_H
#define _MATRIX_UTIL_H

#include "oml/matrix.h"

OML_IMPORT omlMatrix* omluMakeMatrix(const float* v, size_t rows, size_t cols);
OML_IMPORT void omluPrintMatrix(const omlMatrix* m);

#endif // ! _MATRIX_UTIL_H
