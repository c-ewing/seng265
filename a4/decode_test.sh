#!/bin/bash

encode="./mtf2text2"
compare="cmp"
test_dir_1="/home/zastre/seng265/a1/tests/"
test_dir_2="/home/zastre/seng265/a3/tests/"


mkdir testing

files=("test00" "test01" "test02" "test03" "test04" "test05" "test06" "test07" "test08" "test09" "test10")

for file in "${files[@]}"; do

	##copy zastre's file to temp folder, run command, compare to zastres, print result, remove created files

	input="$file.mtf"
	output="$file.txt"

	cp "$test_dir_1$input" "testing/"
	
	$encode "testing/$input"

	$compare "testing/$output" "$test_dir_1$output"

	returnValue=$?
	
	if [ $returnValue -ne 0 ]; then
		echo "$input failed"
	else
		echo "$input succeeded"
	fi
done

files=("test11" "test12" "test13" "test14" "test15" "test16" "test17" "test18" "test19")

for file in "${files[@]}"; do

	##copy zastre's file to temp folder, run command, compare to zastres, print result, remove created files

	input="$file.mtf"
	output="$file.txt"

	cp "$test_dir_2$input" "testing/"
	
	$encode "testing/$input"

	$compare "testing/$output" "$test_dir_2$output"

	returnValue=$?
	
	if [ $returnValue -ne 0 ]; then
		echo "$input failed"
	else
		echo "$input succeeded"
	fi
done

rm -r "testing"

exit 0
