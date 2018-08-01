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
osError python_typeinit(lasso_request_t token, tag_action_t action);
osError python_onCreate(lasso_request_t token, tag_action_t action);
osError python_import( lasso_request_t token, tag_action_t action );
osError python_load( lasso_request_t token, tag_action_t action );
osError python_save( lasso_request_t token, tag_action_t action );
osError python_value( lasso_request_t token, tag_action_t action );
osError python_call( lasso_request_t token, tag_action_t action );
osError python_run( lasso_request_t token, tag_action_t action );
#endif