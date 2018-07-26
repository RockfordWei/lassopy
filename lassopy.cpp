#include "lassopy.h"
#include <Python.h>
#include <iostream>
using namespace std;

enum PythonMethods {
    py_import = 0,
    py_load,
    py_save,
    py_call,
    py_run
};

static const char * kPython = "python";
static const char * kPyCreate = "onCreate";
static const char * kPyImport = "import";
static const char * kPyLoad = "load";
static const char * kPySave = "save";
static const char * kPyCall = "call";
static const char * kPyRun = "run";
// Private data member
static const char * kPrivateMember  = "_privatePython";

void registerLassoModule( void )
{
    Py_Initialize();
	lasso_registerTypeModule("", kPython, python_typeinit, 
        REG_FLAGS_TYPE_DEFAULT|flag_prototype, 
        "Initializer for the python type.", NULL);
}

osError python_typeinit(lasso_request_t token, tag_action_t action)
{
    cout << "loading type" << endl;
	lasso_type_t self = NULL;
	osError err = lasso_typeAllocCustom(token, &self, kPython);
    if ( err != osErrNoErr )
	{
        cerr << "custom type failure" << endl;
		lasso_setResultMessage(token, "Could not allocate new custom type instance.");
		return err;
	}
    lasso_type_t tag = NULL;
    lasso_typeAllocTag(token, &tag, python_onCreate);
    lasso_typeAddMember(token, self, kPyCreate, tag);

    lasso_typeAllocTag(token, &tag, python_import);
    lasso_typeAddMember(token, self, kPyImport, tag);

    lasso_typeAllocTag(token, &tag, python_load);
    lasso_typeAddMember(token, self, kPyLoad, tag);

    lasso_typeAllocTag(token, &tag, python_save);
    lasso_typeAddMember(token, self, kPySave, tag);

    lasso_typeAllocTag(token, &tag, python_call);
    lasso_typeAddMember(token, self, kPyCall, tag);

    lasso_typeAllocTag(token, &tag, python_run);
    lasso_typeAddMember(token, self, kPyRun, tag);

    err = lasso_returnTagValue(token, self);
    cout << "empty done:" << err << endl;
    return err;
}

osError python_onCreate(lasso_request_t token, tag_action_t action)
{
	return osErrNoErr;
}

void python_release(void * ptr) 
{
    PyObject * obj = reinterpret_cast<PyObject *>(ptr);
    if (obj) Py_DecRef(obj);
}

osError python_import( lasso_request_t token, tag_action_t action )
{
    lasso_type_t self = NULL;
    lasso_getTagSelf(token, &self);
	if(!self) return osErrInvalidParameter;
    lasso_type_t param_lib_path = NULL;
    osError err = lasso_getTagParam2(token, 0, &param_lib_path);
	if (err != osErrNoErr) return err;

    auto_lasso_value_t lib_path;
    lasso_typeGetString(token, param_lib_path, &lib_path);
    PyObject * mod = PyImport_ImportModule(lib_path.name);
    lasso_setPtrMember(token, self, kPrivateMember, mod, &python_release);
    return osErrNoErr;
}
osError python_load( lasso_request_t token, tag_action_t action )
{ return osErrNoErr; }
osError python_save( lasso_request_t token, tag_action_t action )
{ return osErrNoErr; }
osError python_call( lasso_request_t token, tag_action_t action )
{ return osErrNoErr; }
osError python_run( lasso_request_t token, tag_action_t action )
{ return osErrNoErr; }
