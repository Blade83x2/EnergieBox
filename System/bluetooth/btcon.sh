#!/usr/bin/expect -f

set prompt "#"
set address [lindex $argv 0]

spawn bluetoothctl
expect -re $prompt
send "remove $address\r"
sleep 1
expect -re $prompt
send "scan on\r"
send_user "\nSleeping\r"
sleep 5
send_user "\nDone sleeping\r"
send "scan off\r"
sleep 2
expect -re $prompt
sleep 2
send "trust $address\r"
sleep 1
expect -re $prompt
sleep 2
send "pair $address\r"
sleep 3
send_user "\nShould be paired now.\r"
sleep 5
send "connect $address\r"
sleep 5
expect "Connection successful"
sleep 2
send "info $address\r"
sleep 5
send "quit\r"
expect eof
