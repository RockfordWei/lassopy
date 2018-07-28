<?LassoScript
protect => {^
    lcapi_loadmodule('/home/lassopy.so')
    local(py = python('datetime'), 
        minyear = #py->load('MINYEAR'),
        maxyear = #py->load('MAXYEAR'))
    #minyear
    '\n'
    #maxyear
^}
?>