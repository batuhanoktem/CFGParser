#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

typedef struct Rule
{
	int id;
	char left;

	int rightCount;
	char *right;
} Rule;

typedef struct Parser
{
	int terminalCount;
	char *terminals;

	int nonTerminalCount;
	char *nonTerminals;

	int ruleCount;
	Rule *rules;
} Parser;

Parser parser;
void readGrammar(char *grammar);
void readWords(char *words, char *results);

int main(int argc, char *argv[])
{
	if (argc == 4)
	{
		readGrammar(argv[1]);
		readWords(argv[2], argv[3]);
	}
	else
	{
		readGrammar("grammar.txt");
		readWords("words.txt", "results.txt");
	}

	return 0;
}

void readGrammar(char *grammar)
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
					parser.terminals = (char*)malloc(2 * sizeof(char));
				else
					parser.terminals = realloc(parser.terminals, (parser.terminalCount + 1) * sizeof(char));

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
					parser.nonTerminals = (char*)malloc(2 * sizeof(char));
				else
					parser.nonTerminals = realloc(parser.nonTerminals, (parser.nonTerminalCount + 1) * sizeof(char));

				parser.nonTerminals[parser.nonTerminalCount - 1] = character;
			}
		}
	}
	parser.nonTerminals[parser.nonTerminalCount] = '\0';

	parser.ruleCount = 0;
	while (!feof(file))
	{
		Rule rule;

		fscanf(file, "%d %c", &rule.id, &rule.left);

		character = fgetc(file);
		if (feof(file))
			break;
		else if ((character != ' ') || (character == '\n') || (character != '\r'))
		{
			while ((character = fgetc(file)) != '>')
			{

			}

			rule.rightCount = 0;
			while (TRUE)
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
							rule.right = (char*)malloc(2 * sizeof(char));
						else
							rule.right = realloc(rule.right, (rule.rightCount + 1) * sizeof(char));

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

void readWords(char *words, char *results)
{
	int i, j;
	char character;
	int wordCount = 0;
	char *word = NULL;
	FILE *file = fopen(words, "r");
	FILE *outputFile = fopen(results, "w");

	while (!feof(file))
	{
		character = fgetc(file);

		if ((character != ' '))
		{
			if (character == '\n' || feof(file))
			{
				if (wordCount == 0)
					break;

				int illegalSymbol;
				word[wordCount] = '\0';

				for (i = 0; i < wordCount - 1; i++)
				{
					illegalSymbol = TRUE;
					for (j = 0; j < parser.nonTerminalCount; j++)
					{
						if (word[i] == parser.nonTerminals[j])
						{
							illegalSymbol = FALSE;
							break;
						}
					}

					if (illegalSymbol)
					{
						for (j = 0; j < parser.terminalCount; j++)
						{
							if (word[i] == parser.terminals[j])
							{
								illegalSymbol = FALSE;
								break;
							}
						}
					}

					if (illegalSymbol)
						break;
				}

				if (illegalSymbol)
					fprintf(outputFile, "NO\n");
				else
				{
					int accepted = FALSE;
					int *acceptedRules = NULL;
					int startIndex;
					int endIndex;
					int acceptedRule = 0;
					char *lastWord = (char*)malloc((wordCount + 1) * sizeof(char));
					char *tempWord;
					strcpy(lastWord, word);
					free(word);
					for (endIndex = wordCount - 1; endIndex >= 0; endIndex--)
					{
						for (startIndex = endIndex; startIndex >= 0; startIndex--)
						{
							int length = endIndex - startIndex + 1;
							char *wordPart = (char*)malloc((length + 1) * sizeof(char));
							strncpy(wordPart, lastWord + startIndex, length);
							wordPart[length] = '\0';
							for (j = 0; j < parser.ruleCount; j++)
							{
								Rule rule = parser.rules[j];

								if (strcmp(wordPart, rule.right) == 0)
								{
									acceptedRule++;
									if (acceptedRule == 1)
										acceptedRules = (int*)malloc(sizeof(int));
									else
										acceptedRules = realloc(acceptedRules, acceptedRule * sizeof(int));

									acceptedRules[acceptedRule - 1] = rule.id;

									tempWord = (char*)malloc((wordCount + 1) * sizeof(char));
									strncpy(tempWord, lastWord, startIndex);
									tempWord[startIndex] = rule.left;
									tempWord[startIndex + 1] = '\0';
									strcat(tempWord, lastWord + endIndex + 1);
									free(lastWord);
									lastWord = tempWord;
									wordCount -= length - 1;
									endIndex = wordCount - 1;
									startIndex = endIndex + 1;

									if ((wordCount == 1) && (lastWord[0] == parser.nonTerminals[0]))
										accepted = TRUE;

									break;
								}
								
								if (accepted)
									break;
							}
							free(wordPart);

							if (accepted)
								break;
						}

						if (accepted)
							break;
					}
					if (!accepted)
						fprintf(outputFile, "NO\n");
					else
					{
						fprintf(outputFile, "YES\n");
						for (j = acceptedRule - 1; j >= 0; j--)
						{
							fprintf(outputFile, "%d", acceptedRules[j]);
							if (j != 0)
								fprintf(outputFile, " ");
						}
						free(acceptedRules);
						fprintf(outputFile, "\n");
					}
				}

				wordCount = 0;
			}
			else if (character != '\r')
			{
				wordCount++;
				if (wordCount == 1)
					word = (char*)malloc(2 * sizeof(char));
				else
					word = realloc(word, (wordCount + 1) * sizeof(char));

				word[wordCount - 1] = character;
			}
		}
	}
	fclose(file);
	fclose(outputFile);
}