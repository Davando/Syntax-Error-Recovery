# Trivial makefile for the calculator scanner/parser.

# Note that the rule for the goal (parse)
# must be the first one in this file.
#
CC = g++
CFLAGS = -g -O2 -Wall -Wpedantic
EXEC = parse

parse:
	g++ parse.cpp scan.cpp -o parse
