// Author: superguo
#ifndef _OML_CONFIG_H
#define _OML_CONFIG_H

#define _OML_SSE 1

#ifdef __cplusplus
	#ifdef _OML_BUILD_SHARED_LIBRARY
		#define OML_EXPORT extern "C" __declspec(dllexport)
		#define OML_IMPORT extern "C" __declspec(dllimport)
	#else
		#define OML_EXPORT extern "C"
		#define OML_IMPORT extern "C"
	#endif
#else
	#ifdef _OML_BUILD_SHARED_LIBRARY
		#define OML_EXPORT __declspec(dllexport)
		#define OML_IMPORT __declspec(dllimport)
	#else
		#define OML_EXPORT
		#define OML_IMPORT extern
	#endif
#endif

#define omlAlloc malloc

#endif	// _OML_CONFIG_H
