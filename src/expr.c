#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strmap.h>
#include <expr.h>

void initStaticObjects(void) {
	// Create Boolean True object
	boolT = malloc(sizeof(expr));
	boolT->type = Boolean;
	boolT->data.boolean = true;

	// Create Boolean False object
	boolF = malloc(sizeof(expr));
	boolF->type = Boolean;
	boolF->data.boolean = false;

	// Create Nil object
	nil = malloc(sizeof(expr));
	nil->type = Nil;

	// Create Identifier Lookup Table
	identifiers = sm_new(100);
}

expr* newInteger(long value) {
	expr* expr = malloc(sizeof(expr));

	expr->type = Integer;
	expr->data.integer = value;
	return expr;
}

expr* newCharacter(char value) {
	expr* expr = malloc(sizeof(expr));

	expr->type = Character;
	expr->data.character = value;
	return expr;
}

expr* newString(char* value) {
	expr* expr = malloc(sizeof(expr));

	expr->type = String;
	expr->data.string = value;
	return expr;
}

expr* newPair(expr* head, expr* tail) {
	expr* expr = malloc(sizeof(expr));

	expr->type = Pair;
	expr->data.pair.head = head;
	expr->data.pair.tail = tail;
	return expr;
}

expr* newError(char* error) {
	expr* expr = malloc(sizeof(expr));

	expr->type = Error;
	expr->data.bottom = error;
	return expr;
}

expr* newIdentifier(char* name) {
	expr* expr = malloc(sizeof(expr));

	// Add identifier to the lookup table.
	sm_put(identifiers, name, NULL);

	expr->type = Identifier;
	expr->data.identifier = name;
	return expr;
}

bool isInteger(expr* e) {
	return e->type == Integer;
}

bool isBoolean(expr* e) {
	return e->type == Boolean;
}

bool isCharacter(expr* e) {
	return e->type == Character;
}

bool isIdentifier(expr* e) {
	return e->type == Identifier;
}

bool isString(expr* e) {
	return e->type == String;
}

bool isPair(expr* e) {
	return e->type == Pair;
}

bool isNil(expr* e) {
	return e->type == Nil;
}

bool isError(expr* e) {
	return e->type == Error;
}
