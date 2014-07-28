#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strmap.h>
#include <expr.h>

// ◥▶◀◤ \\

expr* head(expr* e) {
	if (isPair(e)) {
		return e->data.pair.head;
	} else {
		fprintf(stderr, "Cannot get head of non-pair\n");
		exit(1);
	}
}

expr* tail(expr* e) {
	if (isPair(e)) {
		return e->data.pair.tail;
	} else {
		fprintf(stderr, "Cannot get tail of non-pair\n");
		exit(1);
	}
}

expr* eval(expr* input);

expr* readExpr(FILE* stream);

expr* readInteger(FILE* stream);

expr* readCharacter(FILE* stream);

expr* readString(FILE* stream);

expr* readIdentifier(FILE* stream);

expr* readPair(FILE* stream);

void trimWhitespace(FILE* stream);

bool taggedWith(expr* expr, char* tag);

void print(expr* prgm);

char peek(FILE* stream)
{
	char c;
	c = getc(stream);
	ungetc(c, stream);
	return c;
}

int main()
{
	initStaticObjects();

	printf("Welcome to CAN-Scheme. \n⤿ ");

	while(1)
	{
		print(eval(readExpr(stdin)));
		printf("\n⤿ ");
	}

	return 0;
}

void trimWhitespace(FILE* stream)
{
	char c;

	do
	{
		c = fgetc(stream);
	} while (c == ' ' || c =='\n' || c == EOF);
	ungetc(c, stream);
}

bool isIdentChar(char c) {
	return isalpha(c)
		|| c == '+'
		|| c == '-'
		|| c == '!'
		|| c == '?'
		|| c == '*'
		|| c == '/';
}

bool selfEvaluating(expr* expr) {
	return isBoolean(expr)
		|| isCharacter(expr)
		|| isError(expr)
		|| isInteger(expr)
		|| isString(expr);
}

bool taggedWith(expr* input, char* tag) {
	expr* h;
	
	if (isPair(input)) {
		h = head(input);
		return isIdentifier(h)
			&& (strcmp((*h).data.identifier, tag) == 0);
	} else {
		return false;
	}
}

expr* eval(expr* input) {
	if (selfEvaluating(input)) {
		return input;
	} else if (taggedWith(input, "quote")) {
		return head(tail(input));
	} else {
		return newError("Cannot evaluate expression.");
	}
}

void printList(expr* list) {
		print(head(list));
	if (isNil(tail(list))) {
		return;
	} else if (isPair(tail(list))) {
		printf(" ");
		printList(tail(list));
	} else {
		printf(" . ");
		print(tail(list));
	}
}

void print(expr* prgm) {
	expr* cur = prgm;

	if (cur->type == Integer) {
		printf("%ld", (*cur).data.integer);
	} else if (cur->type == Boolean) {
		if (cur == boolT) {
			printf("#t");
		} else if (cur == boolF) {
			printf("#f");
		} else {
			printf("Unexpected boolean\n");
		}
	} else if (cur->type == Character) {
		printf("#'%c", (*cur).data.character);
	} else if (cur->type == String) {
		printf("\"%s\"", (*cur).data.string);
	} else if (cur->type == Identifier) {
		printf("%s", (*cur).data.identifier);
	} else if (cur->type == Pair) {
		printf("(");
        printList(cur);
		printf(")");
	} else if (cur->type == Nil) {
		printf("()");
	} else if (cur->type == Error) {
		printf("Error: %s", cur->data.bottom);
	} else {
		printf("Error: Unknown expression type.\n0");
	}
}

int parseInt(char* intString)
{
	int base = 10;
	char* leftover;

	return strtol(intString, &leftover, base);
}

expr* readExpr(FILE* stream) {
	size_t size = 32 * sizeof(char);
	char c;
	expr* expr;
	char* s = malloc(size);

	trimWhitespace(stream);

	c = getc(stream);

	if (isdigit(c) || c == '-') {
		ungetc(c, stream);
		expr = readInteger(stream);
	} else if (c == '#') {
		c = getc(stream);
		if (c == 't') {
			expr = boolT;
		} else if (c == 'f') {
			expr = boolF;
		} else if (c == '\'') {
			expr = readCharacter(stream);
		} else {
			sprintf(s, "Unexpected character '%c', expecting 't' or 'f'.", c);
			expr = newError(s);
		}
	} else if (c == '"') {
		expr = readString(stream);
	} else if (c == '(') {
		expr = readPair(stream);
	} else if (isIdentChar(c)) {
		ungetc(c, stream);
		expr = readIdentifier(stream);
	} else {
		sprintf(s, "Unexpected character '%c'", c);
		expr = newError(s);
	}
	return expr;
}

expr* readInteger(FILE* stream) {
	size_t size = 32 * sizeof(char);
	char* num = malloc(size);
	expr* result = malloc(sizeof(expr));
	char c;
	int pos = 0;
	
	c = fgetc(stream);
	while (c != ' ' && c != ')' && c != '\n') 
	{
		num[pos++] = c;
		if (pos == sizeof(num))
		{
			realloc(num, sizeof(num) + size);
		}
		c = fgetc(stream);
	}
    ungetc(c, stream);

	result = newInteger(parseInt(num));
	return result;
}

/*maybe*/char readEscape(FILE* stream) {
	char character;
	char c;
	
	c = fgetc(stream);
	if (c == 'n') {
		character = '\n';
	} else if (c == 'a') {
		character = '\a';
	} else if (c == 'b') {
		character = '\b';
	} else if (c == 'f') {
		character = '\f';
	} else if (c == 't') {
		character = '\t';
	} else if (c == '\\') {
		character = '\\';
	} else if (c == 's') {
		character = ' ';
	} else {
		return 0;
	}
	return character;
}

expr* readCharacter(FILE* stream) {
	char character;
	expr* result = malloc(sizeof(expr));
	char c;
	
	c = fgetc(stream);
	if (c == '\\') {
		character = readEscape(stream);
		if (!character) {
			return newError("Invalid escape sequence");
		}
	} else {
		character = c;
	}
	result = newCharacter(character);
	return result;
}

expr* readString(FILE* stream) {
	size_t size = 32 * sizeof(char);
	char* string = malloc(size);
	expr* result = malloc(sizeof(expr));
	char c;
	int pos = 0;
	
	c = fgetc(stream);
	while (c != '"') 
	{
		if (c == '\\') {
			c = readEscape(stream);
			if (!c) {
				// Consume the remainder of the string.
				while (c != '"') {
					c = fgetc(stream);
				}
				return newError("Invalid escape sequence in string");
			}
		}
		string[pos++] = c;
		if (pos == sizeof(string))
		{
			realloc(string, sizeof(string) + size);
		}
		c = fgetc(stream);
	}
	
	result = newString(string);
	return result;
}

expr* readIdentifier(FILE* stream) {
	size_t size = 32 * sizeof(char);
	char* string = malloc(size);
	expr* result = malloc(sizeof(expr));
	char c;
	int pos = 0;
	
	c = fgetc(stream);
	while (isIdentChar(c)) 
	{
		string[pos++] = c;
		if (pos == sizeof(string))
		{
			realloc(string, sizeof(string) + size);
		}
		c = fgetc(stream);
	}
	ungetc(c, stream);
	
	result = newIdentifier(string);
	return result;
}

expr* readPair(FILE* stream) {
	expr* h = malloc(sizeof(expr));
	expr* t = malloc(sizeof(expr));
	expr* result = malloc(sizeof(expr));
	char c;

	c = getc(stream);
	if (c == ')') {
		return nil;
	}
	ungetc(c, stream);

	h = readExpr(stream);
	trimWhitespace(stream);
	
	c = fgetc(stream);
	if (c == '.') {
		trimWhitespace(stream);
		t = readExpr(stream);
		c = getc(stream);
		if (c != ')') {
			result = newError("Expected character ')'");
		} else {
			result = newPair(h, t);
		}
	} else {
		ungetc(c, stream);
		t = readPair(stream);
		result = newPair(h, t);
	}
	
	return result;
}

