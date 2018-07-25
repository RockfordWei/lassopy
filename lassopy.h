#ifndef _LASSO9_PYTHON_3_6_H_
#define _LASSO9_PYTHON_3_6_H_

#if MACHINE_MACOSX
#include <Lasso9/LassoCAPI.h>
#else
#include "LassoCAPI.h"
#endif

#ifdef WIN32
#define CAPIFILE_EXPORT __declspec(dllexport)
#else
#define CAPIFILE_EXPORT
#endif

extern "C" CAPIFILE_EXPORT void registerLassoModule(void);
osError	tagPyImportFunc( lasso_request_t token, tag_action_t action );

#endif