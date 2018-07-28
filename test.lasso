<?LassoScript
protect => {
    handle => {^
        //error_msg()
    ^}
    lcapi_loadmodule('/home/lassopy.so')
    local(py = python('datetime'))
    if(true) => {^
        #py
    ^}
}
?>