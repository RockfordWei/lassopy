#include "lassopy.h"
#include <Python.h>
#include <iostream>
using namespace std;

void registerLassoModule( void )
{
	lasso_registerTagModule( "py", "import", tagPyImportFunc, REG_FLAGS_TAG_DEFAULT, "Import Library" );
    Py_Initialize();
}

osError
tagPyImportFunc( lasso_request_t token, tag_action_t action )
{
	lasso_type_t theParam = NULL;
	// get the first parameter passed to the tag
	osError err = lasso_getTagParam2(token, 0, &theParam);
	if (err != osErrNoErr) // if no parameter was given, return the error
		return err;
		
	auto_lasso_value_t str;
	lasso_typeGetString(token, theParam, &str);
    // cout << "lasso is calling import: " << str.name << endl;
    PyObject * mod = PyImport_ImportModule(str.name);
    int result = mod == NULL;
	return lasso_returnTagValueInteger(token, result);
}