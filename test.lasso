<?LassoScript
lcapi_loadmodule('/home/lassopy.so')
local(py = python('datetime')) 
local(minyear = #py->load('MINYEAR'))
local(maxyear = #py->load('MAXYEAR'))
if (true) => {^
    '\n\n\n BEGIN LASSO TESTING (VALUE ADDED) \n\n\n'
    '\nexpecting minimal year: '
    #minyear->value()
    '\nexpecting maximal year: '
    #maxyear->value()
^}
#py = python('foo')
local(x = #py->load('intVar'))
local(y = #py->load('stringVar'))
if (true) => {^
    '\nexpecting integer: '
    #x->value()
    '\nexpecting string: '
    #y->value()
^}


local(rocky = #py->load('rocky'))
if (true) => {^
    '\nexpecting name: '
    #rocky->load('name')->value()
^}

local(m = #py->load('intVar'))
if (true) => {^
    '\nexpecting integer: '
    #m->value()
^}

local(arr = #py->load('listVar'))
if (true) => {^
    '\nexpecting array: '
    with element in #arr->value()
      select #element->value()
^}

#py->run("from datetime import datetime; print('\\n\\n\\nthis is not what you want', datetime.now(), '\\n\\n\\n')");
if (true) => {^
    '\n\nThe end.'
^}
?>