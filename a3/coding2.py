#!/usr/bin/python3 

import os
import sys

MAGIC_NUMBER_1 = [0xba, 0x5e, 0xba, 0x11]
MAGIC_NUMBER_2 = [0xba, 0x5e, 0xba, 0x12]

#TODO: comment
#Write an integer value to the file stream using the mtf specification, exit with error if integer is out of range
def write_integer(file_stream, integer):	
	if(integer < 121):
		file_stream.write(bytes([128+integer]))
	elif(integer < 376):
		file_stream.write(bytes([128+121]))
		file_stream.write(bytes([integer-121]))
	elif(integer < 65913):
		file_stream.write(bytes([128+122]))
		file_stream.write(bytes([(integer-376) // 256]))
		file_stream.write(bytes([(integer-376)%256]))
	else:
		print("Words out of range")
		sys.exit(1)
		
#Take a file stream and turn it into a list of integers and ascii characters
def create_input_buffer(file_stream):
	#Initialize the while loop values and a list to store all of the characters from the input
	byte = file_stream.read(1)
	buff = []
	
	#for every byte in the input file add it to a buffer list. 
	#(1)Convert values 128<x<249 to the lookup index values 1-120
	#(2)Convert values x=249 and the next value to index values 121-375
	#(3)Convert values x=250 and the next two values to index values 376-65912
	#(2)Convert values <= 128 to their ascii character
	while byte:
		tmp = int.from_bytes(byte, byteorder='big')
		#(1)
		if(tmp > 128 and tmp < 249):
			buff.append(tmp-128)
		#(2)
		elif(tmp == 249):
			next_int = int.from_bytes(file_stream.read(1), byteorder='big')
			buff.append(121+next_int)
		elif(tmp == 250):
			next_int_1 = int.from_bytes(file_stream.read(1), byteorder='big')
			next_int_2 = int.from_bytes(file_stream.read(1), byteorder='big')
			buff.append(256*next_int_1+next_int_2+376)
		#(3)
		else:
			buff.append(chr(tmp))
		#iterate the byte value
		byte = file_stream.read(1)	
		
	return buff

#Check to see if the next bytes of a file_stream contain either of the two mtf headers, exit with error if they dont
def check_mtf(file_stream):
	#Check if the first three bytes of the file match the special mtf header, if they dont then exit 	
	for i in [0xba, 0x5e, 0xba]:
		input_value = int.from_bytes(file_stream.read(1), byteorder='big')
		if(input_value != i):
			print("Input file is not an mtf file")
			sys.exit(1)	
	
	#Check for both the older mtf header 0x11 and the new 0x12
	next_byte = file_stream.read(1)
	if( not(next_byte == b'\x11' or next_byte == b'\x12')):
		print("Input file is not an mtf file")
		sys.exit(1)
		
def encode(input_name):
	(base_name, _, _) = input_name.rpartition(".")
	output_name = base_name + "." + "mtf"
	
	#offset word lookup to start at 1
	word_lookup = ["",]

	text_file = open(input_name, "r")
	encode_file = open(output_name, "wb")
	
	#Write special mtf file header
	for i in MAGIC_NUMBER_2:
		encode_file.write(bytes((i,)))
		
	#Read each line of the file to be encoded
	line = text_file.readline()
	while(line != ''):
		#Split the line into words
		line_split = line.split(' ')
		
		#Check if the first word is a newline - if it is skip the rest of the line processing
		if(line_split[0] == '\n'):
			encode_file.write(bytes('\n', "ascii"))
			line = text_file.readline()
			continue

		#For every word in the list of split words strip the whitespace/newline
		#(1) then check if the word is in the lookup table, if it is then remove it from the lookup table and move it to the top and write the offset to the output file
		#(2) If the word is not in the lookup tabble, add it. Then write the index value and the word to the output file
		for word in line_split:
			word = word.rstrip()
			
			#(1)
			if(word in word_lookup):
				delta = len(word_lookup) - word_lookup.index(word)
				word_lookup.remove(word)
				word_lookup.append(word)			

				write_integer(encode_file, delta)
			#(2)
			else:
				word_lookup.append(word)
				
				write_integer(encode_file, word_lookup.index(word))
				encode_file.write(bytes(word, "ascii"))
				
		#Increment the line being read then write a newline to the output file
		line = text_file.readline()
		encode_file.write(bytes('\n', "ascii"))
	#Close the resources
	text_file.close()
	encode_file.close()



def decode(input_name):
	(base_name, _, _) = input_name.rpartition(".")
	output_name = base_name + "." + "txt"

	word_lookup = []
	
	mtf_file = open(input_name, "rb")
	decode_file = open(output_name, "w")

	#Check to see if the file is an mtf file
	check_mtf(mtf_file)
		
	#Get a list of the file_stream converted to integers and ascii characters
	buff = create_input_buffer(mtf_file)

	#Iterator for the buffer
	i = 0
	while(i < len(buff)):
		#The index of the next value
		i_ = i+1
		
		#Converts the ints and chars of the buffer to a textfile
		#(1) If the values at i is an int and greater than the size of the lookup table, add a new word to the lookup table and then write it to the output file
		#(2) Else if the value at i is an int and less than the size of the lookup table, find the word, write it to the output file, and move it to the top of the lookup table
		#(1)
		if(type(buff[i]) is int and buff[i] > len(word_lookup)):
			build = ""

			#build the new word to be added by looking through the buffer until it finds an int or the newline character
			while(i_ < len(buff) and type(buff[i_]) is str and buff[i_] != '\n'):
				build += buff[i_]
				i_ += 1
			
			#add the word to the lookup table
			word_lookup.append(build)
			#write the word to the output file
			decode_file.write(build)

			#if the last character is not the newline then write a space
			if(buff[i_] != '\n'):
				decode_file.write(" ")
				
		#(2)
		elif(type(buff[i]) is int and buff[i] <= len(word_lookup)):

			#Get the word at the index specified by the buffer from the lookup table
			temp = word_lookup[len(word_lookup)-buff[i]]
			#Write it to the output file
			decode_file.write(temp)

			#if the last character is not the newline then write a space
			if(buff[i+1] != '\n'):
				decode_file.write(" ")
			
			#Move the word to the top of the lookup table
			word_lookup.remove(temp)
			word_lookup.append(temp)
		
		#if the next character is a newline write it to the output file
		if(i_ < len(buff) and type(buff[i_]) is str and buff[i_] == '\n'):
			decode_file.write('\n')

		#increment i
		i = i_

	#Close the resources
	mtf_file.close()
	decode_file.close()
