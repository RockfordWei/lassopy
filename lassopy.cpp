#include "lassopy.h"
#include <Python.h>
#include <iostream>
#include <list>

using namespace std;

enum PythonMethods {
    py_import = 0,
    py_load,
    py_save,
    py_value,
    py_call,
    py_run
};

static const char * kPython = "python";
static const char * kPyCreate = "onCreate";
static const char * kPyImport = "import";
static const char * kPyLoad = "load";
static const char * kPySave = "save";
static const char * kPyValue = "value";
static const char * kPyCall = "call";
static const char * kPyRun = "run";
// Private data member
static const char * kPyObjReference  = "_pyobject";
string getErrMsg(osError err);

void registerLassoModule( void )
{
  Py_Initialize();
	lasso_registerTypeModule("", kPython, python_typeinit, 
        REG_FLAGS_TYPE_DEFAULT|flag_prototype, 
        "Initializer for the python type.", NULL);
}

osError python_typeinit(lasso_request_t token, tag_action_t action)
{
  lasso_type_t self = NULL;
	osError err = lasso_typeAllocCustom(token, &self, kPython);
  if ( err != osErrNoErr ) return err;
  // add private data member
	lasso_type_t i;
	lasso_typeAllocInteger(token, &i, 0);
	lasso_typeAddDataMember(token, self, kPyObjReference, i);

  lasso_type_t tag = NULL;
  lasso_typeAllocTag(token, &tag, python_onCreate);
  lasso_typeAddMember(token, self, kPyCreate, tag);

  lasso_typeAllocTag(token, &tag, python_import);
  lasso_typeAddMember(token, self, kPyImport, tag);

  lasso_typeAllocTag(token, &tag, python_load);
  lasso_typeAddMember(token, self, kPyLoad, tag);

  lasso_typeAllocTag(token, &tag, python_save);
  lasso_typeAddMember(token, self, kPySave, tag);

  lasso_typeAllocTag(token, &tag, python_value);
  lasso_typeAddMember(token, self, kPyValue, tag);

  lasso_typeAllocTag(token, &tag, python_call);
  lasso_typeAddMember(token, self, kPyCall, tag);

  lasso_typeAllocTag(token, &tag, python_run);
  lasso_typeAddMember(token, self, kPyRun, tag);

  return lasso_returnTagValue(token, self);
}

osError python_onCreate(lasso_request_t token, tag_action_t action)
{
	int count = 0;
	lasso_getTagParamCount(token, &count);
	if (count)
		return python_import(token, 0);
	return osErrNoErr;
}

void python_release(void * ptr) 
{
  auto * obj = reinterpret_cast<PyObject *>(ptr);
  if (obj) Py_DecRef(obj);
}

osError python_import( lasso_request_t token, tag_action_t action )
{
  lasso_type_t self = NULL;
  auto err = lasso_getTagSelf(token, &self);
  if(err != osErrNoErr) return err;
  if(!self) return osErrInvalidParameter;
  lasso_type_t param_lib_path = NULL;
  err = lasso_getTagParam2(token, 0, &param_lib_path);
  if (err != osErrNoErr) return err;

  auto_lasso_value_t lib_path;
  err = lasso_typeGetString(token, param_lib_path, &lib_path);
  if (err != osErrNoErr) return err;

  auto * mod = PyImport_ImportModule(lib_path.name);
  if (!mod) return osErrFileInvalid;
  return lasso_setPtrMember(token, self, kPyObjReference, mod, &python_release);
}

osError python_load( lasso_request_t token, tag_action_t action )
{ 
  lasso_type_t self = NULL;
  auto err = lasso_getTagSelf(token, &self);
  if(err != osErrNoErr) return err;
  if(!self) return osErrInvalidParameter;
  lasso_type_t object = NULL;
  err = lasso_getTagParam2(token, 0, &object);
  if (err != osErrNoErr) return err;

  auto_lasso_value_t pyobj;
  err = lasso_typeGetString(token, object, &pyobj);
  if (err != osErrNoErr) return err;

  if (!pyobj.name) return osErrInvalidParameter;

  void * mod = NULL;
  err = lasso_getPtrMember(token, self, kPyObjReference, &mod);
  if (err != osErrNoErr) return err;

  auto * pmod = reinterpret_cast<PyObject *>(mod);
  if (!pmod) return osErrResource;

  auto * obj = PyObject_GetAttrString(pmod, pyobj.name);
  if (!obj) return osErrResNotFound;

  lasso_type_t child;
  err = lasso_typeAlloc(token, kPython, 0, NULL, &child);
  if (err != osErrNoErr) return err;
  err = lasso_setPtrMember(token, child, kPyObjReference, obj, &python_release);
  if (err != osErrNoErr) return err;
  return lasso_returnTagValue(token, child);
}

typedef osError (*pythonValueType) (lasso_request_t token, PyObject * obj, bool * matched);

osError python_value_type_int(lasso_request_t token, PyObject * obj, bool * matched)
{
  *matched = string (obj->ob_type->tp_name) == "int";
  if (!*matched) return osErrNoErr;
  auto value = PyLong_AsLongLong(obj);
  return lasso_returnTagValueInteger(token, value);
}

osError python_value_type_float(lasso_request_t token, PyObject * obj, bool * matched)
{
  *matched = string (obj->ob_type->tp_name) == "float";
  if (!*matched) return osErrNoErr;
  auto value = PyFloat_AsDouble(obj);
  return lasso_returnTagValueDecimal(token, value);
}

osError python_value_type_string(lasso_request_t token, PyObject * obj, bool * matched)
{
  *matched = string (obj->ob_type->tp_name) == "str";
  if (!*matched) return osErrNoErr;
  Py_ssize_t sz = 0;
  auto str = PyUnicode_AsUTF8AndSize(obj, &sz);
  if (str && sz > 0) {
      return lasso_returnTagValueString(token, str, sz);
  } else {
      return lasso_returnTagValueString(token, "", 0);
  }
}

osError python_value_type_list(lasso_request_t token, PyObject * obj, bool * matched)
{
  *matched = string (obj->ob_type->tp_name) == "list";
  if (!*matched) return osErrNoErr;
  auto size = PyList_Size(obj);
  lasso_type_t *elements = new lasso_type_t[size];
  auto i = size;
  for (i = 0; i < size; i++) {
    
    auto x = PyList_GetItem(obj, i);
    auto err = lasso_typeAlloc(token, kPython, 0, NULL, elements + i);
    if (err != osErrNoErr) return err;
    err = lasso_setPtrMember(token, elements[i], kPyObjReference, x, NULL);
    if (err != osErrNoErr) return err;
  }
  lasso_type_t array = NULL;
  auto err = lasso_typeAllocArray(token, &array, size, elements);
  if (err != osErrNoErr) return err;
  return lasso_returnTagValue(token, array);
}

osError python_value_type_dict(lasso_request_t token, PyObject * obj, bool * matched)
{
  *matched = string (obj->ob_type->tp_name) == "dict";
  if (!*matched) return osErrNoErr;
  
  auto keys = PyDict_Keys(obj);
  auto size = PyDict_Size(obj);
  auto i = size;
  lasso_type_t *elements = new lasso_type_t[size];
  for (i = 0; i < size; i++) {
    auto key = PyList_GetItem(keys, i);
    auto val = PyDict_GetItem(obj, key);
    Py_ssize_t sz = 0;
    auto name = PyUnicode_AsUTF8AndSize(key, &sz);
    lasso_type_t first = NULL;
    lasso_type_t second = NULL;
    auto err = lasso_typeAllocString(token, &first, name, sz);
    if (err != osErrNoErr) return err;
    err = lasso_typeAlloc(token, kPython, 0, NULL, &second);
    if (err != osErrNoErr) return err;
    err = lasso_setPtrMember(token, second, kPyObjReference, val, NULL);
    if (err != osErrNoErr) return err;
    err = lasso_typeAllocPair(token, elements + i, first, second);
    if (err != osErrNoErr) return err;
  }
  
  lasso_type_t array = NULL;
  auto err = lasso_typeAllocArray(token, &array, size, elements);
  if (err != osErrNoErr) return err;
  return lasso_returnTagValue(token, array);
}

osError python_value( lasso_request_t token, tag_action_t action )
{
    lasso_type_t self = NULL;
    auto err = lasso_getTagSelf(token, &self);

    void * pobj = NULL;
    err = lasso_getPtrMember(token, self, kPyObjReference, &pobj);
	  if (err != osErrNoErr) return err;

    auto obj = reinterpret_cast<PyObject *>(pobj);
    if (!obj) return osErrResNotFound;

    string tp_name = string (obj->ob_type->tp_name);
    pythonValueType protos[] = {
      &python_value_type_int,
      &python_value_type_float,
      &python_value_type_string,
      &python_value_type_list,
      &python_value_type_dict
    };

    list<pythonValueType> prototypes(protos, protos + sizeof(protos) / sizeof(pythonValueType));
    for(auto i = prototypes.begin(); i != prototypes.end(); i++) {
      bool matched = false;
      err = (*i)(token, obj, &matched);
      if (matched) {
        return err;
      }
    }
    
    return osErrNotImplemented;
}


osError python_save( lasso_request_t token, tag_action_t action )
{ return osErrNoErr; }
osError python_call( lasso_request_t token, tag_action_t action )
{ return osErrNoErr; }

osError python_run( lasso_request_t token, tag_action_t action )
{ 
  lasso_type_t self = NULL;
  auto err = lasso_getTagSelf(token, &self);
  if(err != osErrNoErr) return err;
  if(!self) return osErrInvalidParameter;
  lasso_type_t command_type = NULL;
  err = lasso_getTagParam2(token, 0, &command_type);
  if (err != osErrNoErr) return err;

  auto_lasso_value_t command_value;
  err = lasso_typeGetString(token, command_type, &command_value);
  if (err != osErrNoErr) return err;

  PyRun_SimpleString(command_value.name);
  return err;
}

string getErrMsg(osError err) 
{
    string msg = "";
    switch (err) {
        case osErrNoErr: msg = "osErrNoErr"; break;
        case osErrAssert: msg = "osErrAssert"; break;
        case osErrStreamReadError: msg = "osErrStreamReadError"; break;
        case osErrStreamWriteError: msg = "osErrStreamWriteError"; break;
        case osErrMemory: msg = "osErrMemory"; break;
        case osErrInvalidMemoryObject: msg = "osErrInvalidMemoryObject"; break;
        case osErrOutOfMemory: msg = "osErrOutOfMemory"; break;
        case osErrOutOfStackSpace: msg = "osErrOutOfStackSpace"; break;
        case osErrCouldNotLockMemory: msg = "osErrCouldNotLockMemory"; break;
        case osErrCouldNotUnlockMemory: msg = "osErrCouldNotUnlockMemory"; break;
        case osErrCouldNotDisposeMemory: msg = "osErrCouldNotDisposeMemory"; break;
        case osErrFile: msg = "osErrFile"; break;
        case osErrFileInvalid: msg = "osErrFileInvalid"; break;
        case osErrFileInvalidAccessMode: msg = "osErrFileInvalidAccessMode"; break;
        case osErrCouldNotCreateOrOpenFile: msg = "osErrCouldNotCreateOrOpenFile"; break;
        case osErrCouldNotCloseFile: msg = "osErrCouldNotCloseFile"; break;
        case osErrCouldNotDeleteFile: msg = "osErrCouldNotDeleteFile"; break;
        case osErrFileNotFound: msg = "osErrFileNotFound"; break;
        case osErrFileAlreadyExists: msg = "osErrFileAlreadyExists"; break;
        case osErrFileCorrupt: msg = "osErrFileCorrupt"; break;
        case osErrVolumeDoesNotExist: msg = "osErrVolumeDoesNotExist"; break;
        case osErrDiskFull: msg = "osErrDiskFull"; break;
        case osErrDirectoryFull: msg = "osErrDirectoryFull"; break;
        case osErrIOError: msg = "osErrIOError"; break;
        case osErrInvalidPathname: msg = "osErrInvalidPathname"; break;
        case osErrInvalidFilename: msg = "osErrInvalidFilename"; break;
        case osErrFileLocked: msg = "osErrFileLocked"; break;
        case osErrFileUnlocked: msg = "osErrFileUnlocked"; break;
        case osErrFileIsOpen: msg = "osErrFileIsOpen"; break;
        case osErrFileIsClosed: msg = "osErrFileIsClosed"; break;
        case osErrBOF: msg = "osErrBOF"; break;
        case osErrEOF: msg = "osErrEOF"; break;
        case osErrCouldNotWriteToFile: msg = "osErrCouldNotWriteToFile"; break;
        case osErrCouldNotReadFromFile: msg = "osErrCouldNotReadFromFile"; break;
        case osErrResNotFound: msg = "osErrResNotFound"; break;
        case osErrResource: msg = "osErrResource"; break;
        case osErrNetwork: msg = "osErrNetwork"; break;
        case osErrInvalidUsername: msg = "osErrInvalidUsername"; break;
        case osErrInvalidPassword: msg = "osErrInvalidPassword"; break;
        case osErrInvalidDatabase: msg = "osErrInvalidDatabase"; break;
        case osErrNoPermission: msg = "osErrNoPermission"; break;
        case osErrFieldRestriction: msg = "osErrFieldRestriction"; break;
        case osErrWebAddError: msg = "osErrWebAddError"; break;
        case osErrWebUpdateError: msg = "osErrWebUpdateError"; break;
        case osErrWebDeleteError: msg = "osErrWebDeleteError"; break;
        case osErrInvalidParameter: msg = "osErrInvalidParameter"; break;
        case osErrOverflow: msg = "osErrOverflow"; break;
        case osErrNilPointer: msg = "osErrNilPointer"; break;
        case osErrUnknownError: msg = "osErrUnknownError"; break;
        case osErrLoopAborted: msg = "osErrLoopAborted"; break;
        case osErrSyntaxError: msg = "osErrSyntaxError"; break;
        case osErrDivideByZero: msg = "osErrDivideByZero"; break;
        case osErrIllegalInstruction: msg = "osErrIllegalInstruction"; break;
        case osErrTagNotFound: msg = "osErrTagNotFound"; break;
        case osErrVarNotFound: msg = "osErrVarNotFound"; break;
        case osErrAborted: msg = "osErrAborted"; break;
        case osErrFailure: msg = "osErrFailure"; break;
        case osErrPreconditionFailed: msg = "osErrPreconditionFailed"; break;
        case osErrPostconditionFailed: msg = "osErrPostconditionFailed"; break;
        case osErrCriteriaNotMet: msg = "osErrCriteriaNotMet"; break;
        case osErrIllegalUseOfFrozenInstance: msg = "osErrIllegalUseOfFrozenInstance"; break;
        case osErrCompilationError: msg = "osErrCompilationError"; break;
        case osErrNotImplemented: msg = "osErrNotImplemented"; break;
        case osErrSyntaxWarning: msg = "osErrSyntaxWarning"; break;
        case osErrWebRequiredFieldMissing: msg = "osErrWebRequiredFieldMissing"; break;
        case osErrWebRepeatingRelatedField: msg = "osErrWebRepeatingRelatedField"; break;
        case osErrWebNoSuchObject: msg = "osErrWebNoSuchObject"; break;
        case osErrWebTimeout: msg = "osErrWebTimeout"; break;
        case osErrWebActionNotSupported: msg = "osErrWebActionNotSupported"; break;
        case osErrConnectionInvalid: msg = "osErrConnectionInvalid"; break;
        case osErrWebModuleNotFound: msg = "osErrWebModuleNotFound"; break;
        case osErrHTTPFileNotFound: msg = "osErrHTTPFileNotFound"; break;
        case osErrDatasourceError: msg = "osErrDatasourceError"; break;
        default:
            msg = "not defined";
    }
    return msg;
}