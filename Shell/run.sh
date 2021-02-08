#!/bin/bash

MYFILE=mine.ans
YOURFILE=right.ans
NORMALIZER=normalize.cpp

while getopts t: flag
do
  case "${flag}" in
    t) testn=${OPTARG};;
  esac
done

rm $MYFILE $YOURFILE
touch mine.ans
touch right.ans
g++ $NORMALIZER -o norm

case $testn in
  1)
    make test01 | ./norm > $MYFILE
    make rtest01 | ./norm > $YOURFILE
    ;;
  2)
    make test02 | ./norm > $MYFILE
    make rtest02 | ./norm > $YOURFILE
    ;;
  3)
    make test03 | ./norm > $MYFILE
    make rtest03 | ./norm > $YOURFILE
    ;;
  4)
    make test04 | ./norm > $MYFILE
    make rtest04 | ./norm > $YOURFILE
    ;;
  5)
    make test05 | ./norm > $MYFILE
    make rtest05 | ./norm > $YOURFILE
    ;;
  6)
    make test06 | ./norm > $MYFILE
    make rtest06 | ./norm > $YOURFILE
    ;;
  7)
    make test07 | ./norm > $MYFILE
    make rtest07 | ./norm > $YOURFILE
    ;;
  8)
    make test08 | ./norm > $MYFILE
    make rtest08 | ./norm > $YOURFILE
    ;;
  9)
    make test09 | ./norm > $MYFILE
    make rtest09 | ./norm > $YOURFILE
    ;;
  10)
    make test10 | ./norm > $MYFILE
    make rtest10 | ./norm > $YOURFILE
    ;;
  11)
    make test11 | ./norm > $MYFILE
    make rtest11 | ./norm > $YOURFILE
    ;;
  12)
    make test12 | ./norm > $MYFILE
    make rtest12 | ./norm > $YOURFILE
    ;;
  13)
    make test13 | ./norm > $MYFILE
    make rtest13 | ./norm > $YOURFILE
    ;;
  14)
    make test14 | ./norm > $MYFILE
    make rtest14 | ./norm > $YOURFILE
    ;;
  15)
    make test15 | ./norm > $MYFILE
    make rtest15 | ./norm > $YOURFILE
    ;;
  16)
    make test16 | ./norm > $MYFILE
    make rtest16 | ./norm > $YOURFILE
    ;;
  17)
    make test17 | ./norm > $MYFILE
    make rtest17 | ./norm > $YOURFILE
    ;;
esac

diff *.ans
