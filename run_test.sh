#!/bin/bash

(cd build && ninja compiler) && ./build/compiler --ssa test.c && xdot cfg.main.dot
