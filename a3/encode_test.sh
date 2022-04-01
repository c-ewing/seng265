#!/bin/bash

encode="./encode.py"
compare="cmp"
test_dir="/home/zastre/seng265/a3/tests/"

mkdir testing

files=("test11" "test12" "test13" "test14" "test15" "test16" "test17" "test18" "test19")

for file in "${files[@]}"; do

	##copy zastre's file to temp folder, run command, compare to zastres, print result, remove created files

	input="$file.txt"
	output="$file.mtf"

	cp "$test_dir$input" "testing/"
	
	$encode "testing/$input"

	$compare "testing/$output" "$test_dir$output"

	returnValue=$?
	
	if [ $returnValue -ne 0 ]; then
		echo "$input failed"
	else
		echo "$input succeeded"
	fi
done

rm -r "testing"

exit 0
