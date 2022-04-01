/*
	Need to fix the assignment, clean up code first. need to make a non pointer array so dat can be modified. issue currently is that strtok changes the value at the pointer screwing up strcmp
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_LENGTH 80
#define MAX_LINES 100 
#define MAX_UNIQUE_WORD 120
#define MAX_WORD_LENGTH 20

int main(int argc, char *argv[]){
	
	char buf[LINE_LENGTH];
	char file_name[strlen(argv[1])];
	char magic_number[4] = {0xba,0x5e,0xba,0x11};
	char word_lookup[MAX_UNIQUE_WORD][MAX_WORD_LENGTH] = {};

	if(argc < 2){
		fprintf(stderr, "Must specifify a filename\n");
		exit(1);
	}

	FILE *data_fp = fopen(argv[1], "r");

	if(data_fp == NULL){
		fprintf(stderr, "Unable to open file: %s\n", argv[1]);
		exit(1);
	}

	/* copy the read only argument to name variable */

	strcpy(file_name, argv[1]);
	
	/* This is a dumb method, doesn't account for '.' in file names and also assumes the extension after the . is 3 characters already*/
	int i;
	for(i = 0; i < strlen(argv[1]); i++){
		if(file_name[i] == '.'){
			file_name[i+1] = 'm';
			file_name[i+2] = 't';
			file_name[i+3] = 'f';
		}
	}

	/* open output file */
	FILE *output = fopen(file_name, "w");

	if(output == NULL){
		fprintf(stderr, "Unable to create/write file: %s\n", file_name);
		exit(1);
	}	
	
	/* write the .mtf header to the file */
	fwrite(magic_number, 1, 4, output);

	/* read the input file line by line */
	int top = -1;
	
	while(fgets(buf, sizeof(char)*LINE_LENGTH, data_fp)){
		char *tokenized_line = strtok(buf, " ");

		while(tokenized_line){
			int token_key = -1;
			int token_status = 0;
			
			if(tokenized_line[0] == '\n'){
				break;
			}
			if(tokenized_line[strlen(tokenized_line)-1] == '\n'){
				tokenized_line[strlen(tokenized_line)-1] = 0;
			} 
						
			for(int i = 0; i < MAX_UNIQUE_WORD; i++){
				//if the current word in the table is empty add a new word
				if(word_lookup[i][0] == 0){
					
					//insert tokenized_line into index i
					for(int j = 0; j < MAX_WORD_LENGTH; j++){
						
						//break once the string terminator is found
						if(tokenized_line[j] == '\0'){
							top++;
							//printf("reached, top: %d\n", top);
							break;
						}
						
						word_lookup[i][j] = tokenized_line[j];
					}
					
					token_status = 1;
					token_key = 128+i+1;
					
					//d printf("Add token: %d Value: %s\n", i, tokenized_line);

					break;
				} else if(word_lookup[i][0] == tokenized_line[0]){
					//d printf("WL: %s TL: %s\n",word_lookup[i], tokenized_line); 
					// if the first character of lookup table at i is equal to the first character of the token check the rest of the token in i 
					
					//compare the two strings
					for(int j = 0; j < MAX_WORD_LENGTH; j++){
						
						if(word_lookup[i][j] != tokenized_line[j]){
							//if they differ then exit the comparison
							break;
						} else if(tokenized_line[j] == '\0'){
							//tokens match and it is the string term char
							
							
							//move index to the top by shifting all elements above it down
							for(int index = i; index < top; index++){
								for(int word_index = 0; word_index < MAX_WORD_LENGTH; word_index++){									
									word_lookup[index][word_index] = word_lookup[index+1][word_index];
								}
							}
							//place tokenized_line at the top
							for(int word_index = 0; word_index < strlen(tokenized_line); word_index++){
								word_lookup[top][word_index] = tokenized_line[word_index];
							}
							for(int word_index = strlen(tokenized_line); word_index < MAX_WORD_LENGTH; word_index++){
								word_lookup[top][word_index] = 0;
							}
							
							token_status = 2;
							break;
						}						
					}
					
					if(token_status == 2){
						//d printf("Found token: %d Value: %s\n", top-i, tokenized_line);
						token_key = 128+top-i+1;
						break;
					}
				}				
			}
			//file writting
			if(token_status < 1){
					
				fprintf(output, "%s", tokenized_line);
			}else if(token_status == 1){
					
				fputc(token_key, output);
					
				fprintf(output, "%s", tokenized_line);
			}else if(token_status == 2){
				fputc(token_key, output);
			}
			
			tokenized_line = strtok(NULL, " ");
		}
		fputc('\n', output);
	}
	
	/* print the name of the written file */
	//d printf("File name: %s\n", file_name); 

	/* cleanup input/output streams */
	fclose(data_fp);
	fclose(output);

	/* clean exit */
	return 0;
}


