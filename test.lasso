<?LassoScript
lcapi_loadmodule('/home/lassopy.so')
local(py = python('datetime')) 
local(minyear = #py->load('MINYEAR'))
local(maxyear = #py->load('MAXYEAR'))
if (true) => {^
    '\n\n\n BEGIN LASSO TESTING \n\n\n'
    '\nexpecting minimal year: '
    #minyear
    '\nexpecting maximal year: '
    #maxyear
^}
#py = python('foo')
local(x = #py->load('intVar'))
local(y = #py->load('stringVar'))
if (true) => {^
    '\nexpecting integer: '
    #x
    '\nexpecting string: '
    #y
^}


local(n = #py->load('listVar'))
if (true) => {^
    '\nexpecting list: '
    #n
^}

local(m = #py->load('intVar'))
if (true) => {^
    '\nexpecting integer: '
    #m
^}

#py->run("from datetime import datetime; print('\\n\\n\\nthis is not what you want', datetime.now(), '\\n\\n\\n')");
if (true) => {^
    '\n\nThe end.'
^}
?>