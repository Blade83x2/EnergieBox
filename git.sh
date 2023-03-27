#!/bin/bash


cd 230V
sudo make
cd ..
cd 12V
sudo make
cd ..
cd h2o
sudo make
cd ..
cd Shutdown
sudo make
cd ..
cd Startup
sudo make
cd ..

sudo git add -A && sudo git commit -m "Version 3"
sudo git push -u origin main

