#!/bin/bash

encode="./text2mtf.py"
compare="cmp"

mkdir testing

files=("test00" "test01" "test02" "test03" "test04" "test05" "test06" "test07" "test08" "test09" "test10")

for file in "${files[@]}"; do

	##copy zastre's file to temp folder, run command, compare to zastres, print result, remove created files

	input="$file.txt"
	output="$file.mtf"

	cp "/home/zastre/seng265/a1/tests/$input" "testing/"
	
	$encode "testing/$input"

	$compare "testing/$output" "/home/zastre/seng265/a1/tests/$output"

	returnValue=$?
	
	if [ $returnValue -ne 0 ]; then
		echo "$input failed"
	else
		echo "$input succeeded"
	fi
done

rm -r "testing"

exit 0
