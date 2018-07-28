<?LassoScript
protect => {^
    lcapi_loadmodule('/home/lassopy.so')
    local(py = python('datetime'))
    local(minyear = #py->load('MINYEAR'))
    #minyear
^}
?>