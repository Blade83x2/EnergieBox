#!/bin/bash

function showLogo() {
    echo -e " _  __     _ _       _     _ ____  _        _   _             "
    echo -e "| |/ /___ | | | ___ (_) __| / ___|| |_ __ _| |_(_) ___  _ __  "
    echo -e "| ' // _ \| | |/ _ \| |/ _  \___ \| __/ _  | __| |/ _ \|  _ \ "
    echo -e "| . \ (_) | | | (_) | | (_| |___) | || (_| | |_| | (_) | | | |"
    echo -e "|_|\_\___/|_|_|\___/|_|\__,_|____/ \__\__,_|\__|_|\___/|_| |_|"
}


function ProgressBar() {
    let _progress=(${1}*100/${2}*100)/100;
    let _done=(${_progress}*4)/10;
    let _left=40-$_done;
    _fill=$(printf "%${_done}s");
    _empty=$(printf "%${_left}s")  ;                    
    # 1.2.1.1 Progress : [########################################] 100%
    printf "\r > ${3}: [${_fill// /#}${_empty// /-}] ${_progress}%%";
    # usage
    #_start=1;
    #_end=500;
    #for number in $(seq ${_start} ${_end})
    #do
    #    sleep 0.1;
    #    ProgressBar ${number} ${_end};
    #done
    #printf '\nFinished!\n';
}
