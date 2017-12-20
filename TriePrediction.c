// Jonathon Sauers
// jo046326
// COP 3502, Spring 2017
// TriePrediction.c

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TriePrediction.h"

// Recursive function to print the trie.
void printTrieHelper(TrieNode *root, char *buffer, int k)
{
	int i;

	if(root == NULL)
		return;

	if(root->count > 0)
		printf("%s (%d)\n", buffer, root->count);

	buffer[k + 1] = '\0';

	for(i = 0; i < 26; i++)
	{
		buffer[k] = 'a' + i;
		printTrieHelper(root->children[i], buffer, k + 1);
	}
	buffer[k] = '\0';
}


// If printing a subtrie, the second parameter should be 1; otherwise, if
// printing the main trie, the second parameter should be 0.
void printTrie(TrieNode *root, int useSubtrieFormatting)
{
	char buffer[1026];

	if(useSubtrieFormatting)
	{
		strcpy(buffer, "- ");
		printTrieHelper(root, buffer, 2);
	}
	else
	{
		strcpy(buffer, "");
		printTrieHelper(root, buffer, 0);
	}
}

// Creates a new TrieNode and returns it.
TrieNode *createTrieNode(void)
{
    TrieNode *trie = calloc(1,sizeof(TrieNode));
    return (trie == NULL) ? NULL : trie;
}

// Returns the index for the first character in letter. If
// not between a-z or A-Z, then returns -1.
int findIndex(char *letter)
{
	if((letter[0]>='A' && letter[0]<='Z') || (letter[0]>='a' && letter[0]<='z'))
		return (letter[0] > 'Z') ? letter[0] - 'a' : letter[0] - 'A';
	else
		return -1;
}

// Recursive function to insert str into the trie.
TrieNode *insertStringHelper(TrieNode *root, char *str)
{
	if(root == NULL)
		return NULL;

  // If empty string, increment count and return it.
	if(strlen(str) <= 0)
	{
		root->count++;
		return root;
	}

  // Putting the string into the correct index.
	if(root->children[findIndex(str)] == NULL)
		root->children[findIndex(str)] = createTrieNode();

  // Go through the entire string.
	return insertStringHelper(root->children[findIndex(str)], str + 1);
}

// Inserts the given string into the root.
void insertString(TrieNode *root, char *str)
{
	if(root == NULL || !strcmp(str, ""))
		return;

  // Calls the recursive helper function.
	insertStringHelper(root, str);
}

// Searches for str and returns a pointer to its terminal node, or NULL.
TrieNode *getNode(TrieNode *root, char *str)
{
	if(!strcmp(str, "") || root->children[findIndex(str)] == NULL)
		return NULL;

  // If the string exists, recursively call until the last index is found.
	if(strlen(str) > 1)
		return getNode(root->children[findIndex(str)], str + 1);

  // Checks the count for that index.
	if(root->children[findIndex(str)]->count > 0)
		return root->children[findIndex(str)];

	return NULL;
}

// Strips all punctuation and returns the str.
char *stripPunctuation(char *str)
{
	int i, j, len = strlen(str);
	char *new = malloc(sizeof(char) * MAX_CHARACTERS_PER_WORD + 2);

	// Takes the junk out of str.
	for(i = 0, j = 0; i < len; i++)
	{
		if(isalpha(str[i]))
		{
			new[j] = str[i];
			j++;
		}
	}
	new[j] = '\0';

	strcpy(str, new);
	free(new);
	return str;
}

// Returns 1 if str has a punctuator at the end, 0 otherwise.
int endSentence(char *str)
{
	if(strchr(str,'.')==NULL && strchr(str,'!')==NULL && strchr(str,'?')==NULL)
		return 0;
	else
		return 1;
}


// Opens file and creates a trie, returns the root.
TrieNode *buildTrie(char *filename)
{
	FILE *ifp;
	TrieNode *root, *temp;

  int flag;
	char buffer[MAX_CHARACTERS_PER_WORD + 2];
	char strippedStr[MAX_CHARACTERS_PER_WORD + 2];
	char tempStr[MAX_CHARACTERS_PER_WORD + 2];

	root = createTrieNode();

	if(root == NULL)
		return NULL;

	ifp = fopen(filename, "r");
	strcpy(tempStr, "");

	// Reads the file into buffer, which is inserted into root.
	// Creates subtrie with buffer if needed.
	while(fscanf(ifp, "%s", buffer) >= 0)
	{
		flag = endSentence(buffer);
		strcpy(strippedStr, stripPunctuation(buffer));
		strcpy(buffer, strippedStr);
		insertString(root, buffer);
		temp = getNode(root, tempStr);

    // Sets up subtrie.
		if(temp != NULL)
		{
			if(temp->subtrie == NULL)
				temp->subtrie = createTrieNode();

			insertString(temp->subtrie, buffer);
		}

    // If it is the end of a sentence.
		if(!flag)
			strcpy(tempStr, buffer);
		else
			strcpy(tempStr, "");
	}
	fclose(ifp);
	return root;
}

// Recursively destroys root and returns NULL.
TrieNode *destroyTrie(TrieNode *root)
{
	int i;

	if(root == NULL)
		return NULL;

	for(i = 0; i < 26; i++)
	{
		destroyTrie(root->children[i]);
	}
	destroyTrie(root->subtrie);

	free(root);
	root = NULL;

	return root;
}

// Checks if a str is in the trie.
int containsWord(TrieNode *root, char *str)
{
  // If this returns NULL, then the node was not found in root.
  return (getNode(root, str) == NULL) ? 0 : 1;
}


// Stores the most frequent word in mostFreq.
void mostFreqHelp(TrieNode *root, char *buffer, int num, char *mostFreq, int *count)
{
	int i;

	if(root == NULL)
		return;

	if(root->count > count[0])
	{
		count[0] = root->count;
		strcpy(mostFreq, buffer);
	}
	buffer[num + 1] = '\0';

	for (i = 0; i < 26; i++)
	{
		buffer[num] = 'a' + i;
		mostFreqHelp(root->children[i], buffer, num + 1, mostFreq, count);
	}
	buffer[num] = '\0';
}


// Finds most frequent word and copies it into str.
void getMostFrequentWord(TrieNode *root, char *str)
{
	int countArray[1];
	char mostFrequent[MAX_CHARACTERS_PER_WORD + 2];
	char buffer[MAX_CHARACTERS_PER_WORD + 2];

	if(root == NULL)
	{
		strcpy(str, "");
		return;
	}

	countArray[0] = 0;
	mostFreqHelp(root, buffer, 0, mostFrequent, countArray);
	strcpy(str, mostFrequent);
}

// If given the string command, prints trie and subtrie.
void stringCommand(TrieNode *root, char *str)
{
	printf("%s\n", str);
	root = getNode(root, str);

	if(root == NULL)
	{
		printf("(INVALID STRING)\n");
		return;
	}

	if(root->subtrie != NULL)
    printTrie(root->subtrie, 1);
	else
    printf("(EMPTY)\n");
}

// If given the @ command
void atCommand(FILE *file, TrieNode *root, char *buffer)
{
	int i, num = 0;
	char str[MAX_CHARACTERS_PER_WORD + 2];
	TrieNode *trie;

	// Stores given string in str.
	fscanf(file, "%s", buffer);
	strcpy(str, buffer);

	// Stores in buffer to put in num (for loop).
	fscanf(file, "%s", buffer);

	for(i = 0; i < strlen(buffer); i++)
		num = (num * 10) + (buffer[i] - '0');

	printf("%s", str);
	trie = getNode(root, str);

	if(trie == NULL)
	{
		printf("\n");
		return;
	}

	// Prints the str.
	for(i = 1; i <= num; i++)
	{
		if(trie->subtrie == NULL)
			break;

		printf(" ");
		getMostFrequentWord(trie->subtrie, str);
		printf("%s", str);
		trie = getNode(root, str);
	}
	printf("\n");
}

// Counts the total number of occurances of str
// :(
int prefixCount(TrieNode *root, char *str)
{
  TrieNode *temp;
  temp = getNode(root, str);

  return (temp == NULL) ? 0 : temp->count;
}


// How difficult I found this assignment.
double difficultyRating(void)
{
	return 4.8;
}

// How much time I spend on this assignment.
double hoursSpent(void)
{
	return 20.0;
}


// My main function
int main(int argc, char **argv)
{
	FILE *ifp;
	char buffer[MAX_CHARACTERS_PER_WORD + 2];
	TrieNode *trie;

	if(argc < 3)
		return 0;

	trie = buildTrie(argv[1]);
	ifp = fopen(argv[2], "r");

	while(fscanf(ifp, "%s", buffer) >= 0)
	{
		if(!strcmp(buffer, "!"))
			printTrie(trie, 0);
		else if(!strcmp(buffer, "@"))
			atCommand(ifp, trie, buffer);
		else
			stringCommand(trie, buffer);
	}
	destroyTrie(trie);
	fclose(ifp);
	return 0;
}
