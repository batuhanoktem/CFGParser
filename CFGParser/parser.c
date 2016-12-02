#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule
{
	int id;
	char left;

	int rightCount;
	char* right;
} Rule;

typedef struct Parser
{
	int terminalCount;
	char* terminals;
	
	int nonTerminalCount;
	char* nonTerminals;

	int ruleCount;
	Rule* rules;
} Parser;

Parser parser;
void readGrammar(char* grammar);
void readWords(char* words);

int main(int argc, char* argv[])
{
	readGrammar(argv[1]);
	readWords(argv[2]);
	return 0;
}

void readGrammar(char* grammar)
{
	char character;
	FILE *file = fopen(grammar, "r");

	parser.terminalCount = 0;
	while (!feof(file))
	{
		character = fgetc(file);
		if ((character != ' '))
		{
			if (character == '\n')
				break;
			else if (character != '\r')
			{
				parser.terminalCount++;
				if (parser.terminalCount == 1)
					parser.terminals = (char*)malloc(sizeof(char));
				else
					parser.terminals = realloc(parser.terminals, parser.terminalCount * sizeof(char));

				parser.terminals[parser.terminalCount - 1] = character;
			}
		}
	}
	parser.terminals[parser.terminalCount] = '\0';

	parser.nonTerminalCount = 0;
	while (!feof(file))
	{
		character = fgetc(file);
		if ((character != ' '))
		{
			if (character == '\n')
				break;
			else if (character != '\r')
			{
				parser.nonTerminalCount++;
				if (parser.nonTerminalCount == 1)
					parser.nonTerminals = (char*)malloc(sizeof(char));
				else
					parser.nonTerminals = realloc(parser.nonTerminals, parser.nonTerminalCount * sizeof(char));

				parser.nonTerminals[parser.nonTerminalCount - 1] = character;
			}
		}
	}
	parser.nonTerminals[parser.nonTerminalCount] = '\0';

	parser.ruleCount = 0;
	while (!feof(file))
	{
		Rule rule;

		fscanf(file, "%d    %c", &rule.id, &rule.left);

		character = fgetc(file);
		if ((character != ' ') || (character == '\n') || (character != '\r'))
		{
			while ((character = fgetc(file)) != '>')
			{

			}

			rule.rightCount = 0;
			while (1)
			{
				character = fgetc(file);
				if (feof(file))
					break;
				else if ((character != ' '))
				{
					if (character == '\n')
						break;
					else if (character != '\r')
					{
						rule.rightCount++;
						if (rule.rightCount == 1)
							rule.right = (char*)malloc(sizeof(char));
						else
							rule.right = realloc(rule.right, rule.rightCount * sizeof(char));

						rule.right[rule.rightCount - 1] = character;
					}
				}
			}
			rule.right[rule.rightCount] = '\0';

			parser.ruleCount++;
			if (parser.ruleCount == 1)
				parser.rules = (Rule*)malloc(sizeof(Rule));
			else
				parser.rules = realloc(parser.rules, parser.ruleCount * sizeof(Rule));

			parser.rules[parser.ruleCount - 1] = rule;
		}
	}
	fclose(file);
}

void readWords(char* words)
{
	char character;
	int wordCount = 0;
	char *word = NULL;
	FILE *file = fopen(words, "r");

	while (!feof(file))
	{
		character = fgetc(file);
		if (feof(file))
			break;

		else if ((character != ' '))
		{
			if (character == '\n')
			{
				word[wordCount] = '\0';
				wordCount = 0;
			}
			else if (character != '\r')
			{
				wordCount++;
				if (wordCount == 1)
					word = (char*)malloc(sizeof(char));
				else
					word = realloc(word, wordCount * sizeof(char));

				word[wordCount - 1] = character;
			}
		}
	}
	fclose(file);
}