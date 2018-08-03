#ifndef _LASSO9_PYTHON_3_6_H_
#define _LASSO9_PYTHON_3_6_H_

#include <LassoCAPI.h>

extern "C" void registerLassoModule(void);
osError python_typeinit(lasso_request_t token, tag_action_t action);
osError python_onCreate(lasso_request_t token, tag_action_t action);
osError python_import( lasso_request_t token, tag_action_t action );
osError python_load( lasso_request_t token, tag_action_t action );
osError python_save( lasso_request_t token, tag_action_t action );
osError python_value( lasso_request_t token, tag_action_t action );
osError python_call( lasso_request_t token, tag_action_t action );
osError python_run( lasso_request_t token, tag_action_t action );
#endif