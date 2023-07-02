#!/usr/bin/sh

set -xe

g++ -std=c++17 -g -Wall -Wextra -O3 -o main src/* -Iinclude -lm
