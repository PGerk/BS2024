#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LENGTH 101 //100 chars +1 for null terminator

static int elementsInArray = 0;

char* readInput(char* array) {
	
	char *temp = malloc(sizeof(char) * MAX_WORD_LENGTH);
	
	if (temp == NULL) {
		perror("Temporary Array Malloc Failure");
		exit(EXIT_FAILURE);
	}
	
	int charCount = 0;
	int arrayCounter = 0;
	
	while (true) {
		char c = getchar();
		
		//Problem with Input
		if (ferror(stdin)) {
			perror("Input Error");
			free(temp);
			free(array);
			exit(EXIT_FAILURE);
		}
		
		//End of Input
		if (feof(stdin)) {
			//Check if one last word exists
			if (charCount > 0) {
				array = realloc(array, sizeof(char)*(charCount + arrayCounter));
				
				if (array == NULL) {
					perror("Array Realloc Failure");
					free(temp);
					exit(EXIT_FAILURE);
				}
				
				memcpy(array + arrayCounter, temp, charCount);
				elementsInArray += 1;
				arrayCounter += charCount;
			}
			
			break;
		}
		
		//Maximum word length exceeded
		if (charCount == MAX_WORD_LENGTH) {
			fprintf(stderr, "\nThe word has exceeded the maximum allowed length.\n");
			charCount = 0;
			continue;
		}
		
		//All clear, add to temporary word
		temp[charCount] = c;
		charCount++;
		
		//If temporary word finished: Add to array
		if (c == '\n') {
			array = realloc(array, sizeof(char) * (charCount + arrayCounter));
				
			if (array == NULL) {
				perror("Array Realloc Failure");
				free(temp);
				exit(EXIT_FAILURE);
			}
				
			memcpy(array + arrayCounter, temp, charCount);
			arrayCounter += charCount;
			elementsInArray += 1;
			charCount = 0;
		}
	}
	
	free(temp);
	return array;
}

char** seperateWords(char* array) {
	char** words = malloc(elementsInArray * sizeof(char*));
	
	int wordCount = 0;
	
	if (words == NULL) {
    perror("Words Array Malloc Failed");
    exit(EXIT_FAILURE);
	}
	
	char* word = strtok(array, "\n");
	while (word != NULL) {
		//Duplicate word into array
		words[wordCount] = strdup(word);
		
		if (words[wordCount] == NULL) {
			perror("Word duplication Failure");
			exit(EXIT_FAILURE);
		}
		
		wordCount++;
		word = strtok(NULL, "\n");
    }
	
	return words;
}

int compareWords(const void* a, const void* b) {
	return strcmp(*(const char**)a, *(const char**)b);
}

int main() {
	char *array = NULL;	
	
	//Read User Input
	array = readInput(array);
	
	//Seperate Words
	char** words = seperateWords(array);
	free(array);
	
	//Sort Words alphabetically
	qsort(words, elementsInArray, sizeof(char*), compareWords);
	
	//Print resulting array
	for (int i = 0; i < elementsInArray; i++) {
		printf("%s\n", words[i]);
		free(words[i]);
	}
	
	free(words);
	
	return 0;
	}