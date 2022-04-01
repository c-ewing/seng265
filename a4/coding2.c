#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int  magic_number_1[] = {0xBA, 0x5E, 0xBA, 0x11, 0x00};
int  magic_number_2[] = {0xBA, 0x5E, 0xBA, 0x12, 0x00};




/*
 * The encode() function may remain unchanged for A#4.
 */

int encode(FILE *input, FILE *output) {
	return 0;
}

/*
Given the first number > 128 read, the pointer to the buffer, and the index of the first number
interpret the encoded integers according to the mtf specification in assignment 3

returns the integer value of the encoded number
*/
int read_encoded_integer(int initial_number, int *buffer, int index){
	if(initial_number > 128 && initial_number < 249){
		return initial_number - 128;
	}else if(initial_number == 249){
		int next_num = buffer[index+1];
		return 121+next_num;
	}else if(initial_number == 250){
		int next_num_1 = buffer[index+1];
		int next_num_2 = buffer[index+2];
		return 256*next_num_1 + next_num_2 + 376;
	}
}

/*
Given a pointer to the buffer, start index in the buffer, and the size of the buffer
From the given start point in the buffer find the next word

return a char array containing the found word
*/
char *read_encoded_word(int *buffer, int start_index, int buffer_size){
	int iterator;
	char *word_storage = malloc(sizeof(char));
	
	if(word_storage == NULL){
		printf("fail malloc, word storage");
		exit(1);
	}
	
	//iterate through indices in the buffer from start_index to buffer_size
	for(iterator = start_index; iterator<buffer_size; iterator++){
		int value = buffer[iterator];
		//if the value at iterator is a ascii character and not a newline or space 
		//add it to word_storage
		if(value < 129 && buffer[iterator] != '\n' && buffer[iterator] != ' '){
			word_storage = realloc(word_storage, (iterator-start_index+1)*sizeof(char));
			
			if(word_storage == NULL){
				printf("fail realloc, word storage");
				exit(1);
			}
			
			word_storage[iterator-start_index] = buffer[iterator];
		}else{
		//otherwise word finding is finished, expand the array one more time and append the null terminator
		//and return
			word_storage = realloc(word_storage, (iterator-start_index+1)*sizeof(char));
			
			if(word_storage == NULL){
				printf("fail realloc, word storage");
				exit(1);
			}	
			word_storage[iterator-start_index] = '\0';			
			return word_storage;
		}
	}
}
/*
Given a char[][], index to be moved to top, and the maximum size of word_lookup
move the given index to the top of the array by shifting all elements down and readding at the top

return the reordered lookup table
*/
char **move_word_to_top(char **word_lookup, int move_top, int word_lookup_count){
	if(move_top == word_lookup_count-1){
		return word_lookup;
	}
	
	char **new_word_lookup = calloc(word_lookup_count, sizeof(char *));
	
	if(new_word_lookup == NULL){
		printf("fail malloc, move to top");
		exit(1);
	}
	
	//temp < var to top
	//copy all elements less than move_top into new array
	int temp;
	for(temp = 1; temp < move_top; temp++){
		new_word_lookup[temp] = calloc((strlen(word_lookup[temp])+1), sizeof(char));
		
		if(new_word_lookup[temp] == NULL){
			printf("fail malloc, move to top, array assignment\n");
			exit(1);
		}
		
		strncpy(new_word_lookup[temp], word_lookup[temp], strlen(word_lookup[temp]));
	}
	//get the word we are looking for and hold onto it
	char *word = calloc((strlen(word_lookup[temp])+1),sizeof(char));
	if(word == NULL){
		printf("fail malloc, word");
		exit(1);
	}
	strncpy(word, word_lookup[temp], strlen(word_lookup[temp]));
	
	//add the next values shifted down
	for(temp = move_top; temp < word_lookup_count-1; temp++){
		new_word_lookup[temp] = calloc((strlen(word_lookup[temp+1])+1), sizeof(char));
		
		if(new_word_lookup[temp] == NULL){
			printf("fail malloc, move to top, array assignment 2\n");
			exit(1);
		}
		strncpy(new_word_lookup[temp], word_lookup[temp+1], strlen(word_lookup[temp+1]));
	}
	
	//add the index we were looking for to the top of the array
	new_word_lookup[temp] = calloc((strlen(word_lookup[temp])+1),sizeof(char));
	if(new_word_lookup[temp] == NULL){
		printf("fail malloc, move to top, array assignment 3\n");
		exit(1);
	}
	strncpy(new_word_lookup[word_lookup_count-1], word, strlen(word));
	
	return new_word_lookup;
}

/*
Given a file input stream check if it is an mtf

return 1 if it is an mtf, 0 otherwise
*/
int check_mtf(FILE *input){
	int temp;
	int read_character;
	//first three bytes are always the same check
	for(temp = 0; temp < 3; temp++){
		read_character = fgetc(input);
		
		if(read_character != magic_number_1[temp]){
			printf("Not an mtf file");
			return 0;
		}
	}
	
	read_character = fgetc(input);
	//fourht byte can be either 0x11 or 0x12
	if(!(read_character == magic_number_1[3] || read_character == magic_number_2[3])){
		printf("Not an mtf file");
		return 0;
	}
}

int decode(FILE *input, FILE *output) {
    
	int read_character;
	int *input_buffer = (int *)malloc(sizeof(int));
	int current_position = 0;

	if(input_buffer == NULL){
		printf("fail malloc");
		exit(1);
	}
	
	//check it is an mtf file
	if(check_mtf(input) == 0){
		return -1;
	}
	
	//build the buffer of inputs
	while((read_character = fgetc(input)) != EOF){
		input_buffer = realloc(input_buffer, (current_position+1)*sizeof(int));

		if(input_buffer == NULL){
			printf("Failed realloc: input_buffer");
			exit(1);
		}	
		
		input_buffer[current_position++] = read_character;
	}
	
	int word_lookup_count = 1;
	char **word_lookup;
	word_lookup = malloc(sizeof(char *));
	
	if(word_lookup == NULL){
		printf("Failed malloc: word_lookup");
		exit(1);
	}

	//iterate through input_buffer
	int iterator;
	for(iterator = 0; iterator < current_position; iterator++){
		int char_value = input_buffer[iterator];
		//if the value in the buffer is an encoded integer
		if(char_value > 128){
			//get the value of the integer
			char_value = read_encoded_integer(char_value, input_buffer, iterator);
			
			//shift the iterator given the number of bytes required to encoded the integer
			if(121 <= char_value && char_value <= 375){
				iterator = iterator + 1;
			}else if(376 <= char_value && char_value <= 65912){
				iterator = iterator + 2;
			}
			
			//if the encoded integer is larger than the number of words in the word_lookup
			//then we are adding a new word to word_lookup
			if(char_value > word_lookup_count-1){				
				word_lookup = realloc(word_lookup, (word_lookup_count+1)*sizeof(char *));
			
				if(input_buffer == NULL){
					printf("Fail realloc: word_lookup, Adding word");
					exit(1);
				}
				
				//read the encoded word
				char *word_search = read_encoded_word(input_buffer, iterator+1, current_position);
				word_lookup[word_lookup_count] = calloc((strlen(word_search)+1),sizeof(char));
				
				if(word_lookup[word_lookup_count] == NULL){
					printf("Fail malloc, word_lookup[word_lookup_count]");
					exit(1);
				}
				
				//copy the string into word_lookup
				strncpy(word_lookup[word_lookup_count], word_search, strlen(word_search));
			
				//write the found string to the output file
				fprintf(output, "%s", word_lookup[word_lookup_count]);
				//increase word_lookup_count by one
				word_lookup_count++;
				//shift the iterator by the number of characters in the encoded string
				iterator = iterator + strlen(word_search);
				
				free(word_search);
				
				//if the next character is not a newline put a space
				if(input_buffer[iterator+1] != '\n'){
					fprintf(output, " ");
				}
			}else{				
			//otherwise the word is in the lookup table
				//output the word to the file
				fprintf(output, "%s", word_lookup[word_lookup_count - char_value]);
				//move the found word to the top
				word_lookup = move_word_to_top(word_lookup, word_lookup_count - char_value, word_lookup_count);
				
				//if the next character is not a newline write a space
				if(input_buffer[iterator+1] != '\n'){
					fprintf(output, " ");
				}
			}
		}
		//if the iterator will not overrun the buffer and the next character is '\n' write a newline to the output
		if(iterator+1 < current_position && input_buffer[iterator+1] == '\n'){
			fprintf(output, "\n");
		}
	}
	
	//memory cleanup
	for(iterator = 1; iterator < word_lookup_count; iterator++){
		free(word_lookup[iterator]);
	}
	
	free(word_lookup);
	free(input_buffer);
	
    return 0;
}
