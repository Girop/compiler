#!/bin/bash

(cd build && ninja compiler) && (rr record ./build/compiler --ssa test.c) && xdot .cfg.main.dot
