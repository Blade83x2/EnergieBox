#!/bin/bash

gpio -x mcp23017:100:0x27 mode 100 out
gpio -x mcp23017:100:0x27 write 100 1    << = LOW
gpio -x mcp23017:100:0x27 write 100 0    << = HIGH
