#!/bin/bash
source "$(pwd)/spinner.sh"
start_spinner 'sleeping for 2 secs...'
sleep 2
stop_spinner $?

#start_spinner 'copying non-existen files...'
# use sleep to give spinner time to fork and run
# because cp fails instantly
#sleep 1
#cp 'file1' 'file2' > /dev/null 2>&1
#stop_spinner $?

