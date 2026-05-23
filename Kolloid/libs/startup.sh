#!/bin/bash
echo -e "\nStarting ${programmName} Ver.${programmVersion}";
sleep 1.1;
echo -e "Detect System:";
sleep 2.1;
echo -e " + Distribution:\t$(detect_distro)";
sleep 0.1;
echo -e " + Release:\t\t$(detect_release)";
sleep 0.1;
echo -e " + Architekture:\t$(detect_arch)";
sleep 0.1;
echo -e " + Kernel:\t\t$(uname -r)";
sleep 0.1;
echo -e " + User:\t\t$(whoami)";
sleep 0.1;
echo -e " + Project Path:\t$projectDir";
echo -e " + Script Path:\t\t$scriptPath";
echo -e " + Libary Path:\t\t$libsPath";
echo -e " + Sound Path:\t\t$soundPath";
sleep 5.2
clear
