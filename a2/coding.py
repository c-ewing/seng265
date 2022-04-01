#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import os
import sys

def encode_main():

	output_name = sys.argv[1][:len(sys.argv[1])-3]+"mtf"
	input_name = sys.argv[1]
	word_lookup = []

	text_file = open(input_name, "r")
	encode_file = open(output_name, "wb")
	
	for i in [0xba, 0x5e, 0xba, 0x11]:
		encode_file.write(bytes((i,)))
	line = text_file.readline()
	while(line != ''):
		line_split = line.split(' ')
		
		if(line_split[0] == '\n'):
			encode_file.write(bytes('\n', "ascii"))
			line = text_file.readline()
			continue

		for word in line_split:
			word = word.rstrip()
			
			if(word in word_lookup):
				delta = len(word_lookup) - word_lookup.index(word) - 1
				word_lookup.remove(word)
				word_lookup.append(word)			
	
				encode_file.write(bytes([delta+129]))
				#add mtf, move to top of list
			else:
				word_lookup.append(word)
				encode_file.write(bytes([word_lookup.index(word)+129]))
				encode_file.write(bytes(word, "ascii"))
		line = text_file.readline()
		encode_file.write(bytes('\n', "ascii"))	
	text_file.close()
	encode_file.close()
	
		

def decode_main():
	
	output_name = sys.argv[1][:len(sys.argv[1])-3]+"txt"
	input_name = sys.argv[1]
	word_lookup = []
	
	mtf_file = open(input_name, "rb")
	decode_file = open(output_name, "w")

	#figure out how to get an int from a byte string 	
	for i in [0xba, 0x5e, 0xba, 0x11]:
		input_value = int.from_bytes(mtf_file.read(1), byteorder='big')
		if(input_value != i):
			print("Input file is not an mtf file")
			return -1;		
	

	byte = mtf_file.read(1)
	buff = []
	
	while byte:
		tmp = int.from_bytes(byte, byteorder='big')
		if(tmp > 128):
			buff.append(tmp-128)
		else:
			buff.append(chr(tmp))
		byte = mtf_file.read(1)	

	i = 0
	while(i < len(buff)):
		i_ = i+1
		if(type(buff[i]) is int and buff[i] > len(word_lookup)):
			build = ""

			while(i_ < len(buff) and type(buff[i_]) is str and buff[i_] != '\n'):
				build += buff[i_]
				i_ += 1
			
			word_lookup.append(build)
			decode_file.write(build)

			if(buff[i_] != '\n'):
				decode_file.write(" ")

			print("Added and wrote: " + build)
				

		elif(type(buff[i]) is int and buff[i] <= len(word_lookup)):

			temp = word_lookup[len(word_lookup)-buff[i]]
			decode_file.write(temp)

			if(buff[i+1] != '\n'):
				decode_file.write(" ")

			word_lookup.remove(temp)
			word_lookup.append(temp)
			
			print("Found and wrote: " + temp)

		if(i_ < len(buff) and type(buff[i_]) is str and buff[i_] == '\n'):
			decode_file.write('\n')
			print("Found newline and wrote")
			#i_ += 1 
		i = i_

	mtf_file.close()
	decode_file.close()
		

command = os.path.basename(__file__)

if(__name__ == "__main__" and command == "text2mtf.py"):
	encode_main()
elif(__name__ == "__main__" and command == "mtf2text.py"):
	decode_main()
