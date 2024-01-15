#!/bin/bash

echo -e "\nStarting ${programmName} Ver.${programmVersion}";
sleep 0.6;

echo -e "\nDetect System: ";
sleep 0.3;

echo -e " + Distribution:\t\t$(detect_distro)";
sleep 0.3;

echo -e " + Release:\t\t\t$(detect_release)";
sleep 0.3;

echo -e " + Architekture:\t\t$(detect_arch)";
sleep 0.3;

echo -e " + Kernel:\t\t\t$(uname -r)";
sleep 0.3;

echo -e " + User:\t\t\t$(whoami)";
sleep 0.3;



echo -e " + Script Path:\t\t\t$scriptPath";
echo -e " + Libary Path:\t\t\t$libsPath";
echo -e " + Sound Path:\t\t\t$soundPath";
sleep 0.9
clear
