#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <expr.h>

void initStaticObjects(void) {
	// Create Boolean True object
	boolT = malloc(sizeof(Expr));
	boolT->type = Boolean;
	boolT->data.boolean = true;

	// Create Boolean False object
	boolF = malloc(sizeof(Expr));
	boolF->type = Boolean;
	boolF->data.boolean = false;

	// Create Nil object
	nil = malloc(sizeof(Expr));
	nil->type = Nil;
}

Expr* newInteger(long value) {
	Expr* expr = malloc(sizeof(Expr));

	expr->type = Integer;
	expr->data.integer = value;
	return expr;
}

Expr* newCharacter(char value) {
	Expr* expr = malloc(sizeof(Expr));

	expr->type = Character;
	expr->data.character = value;
	return expr;
}

Expr* newString(char* value) {
	Expr* expr = malloc(sizeof(Expr));

	expr->type = String;
	expr->data.string = value;
	return expr;
}

Expr* newPair(Expr* head, Expr* tail) {
	Expr* expr = malloc(sizeof(Expr));

	expr->type = Pair;
	expr->data.pair.head = head;
	expr->data.pair.tail = tail;
	return expr;
}

Expr* newError(char* error) {
	Expr* expr = malloc(sizeof(Expr));

	expr->type = Error;
	expr->data.bottom = error;
	return expr;
}

Expr* newIdentifier(char* name) {
	Expr* expr = malloc(sizeof(Expr));

	expr->type = Identifier;
	expr->data.identifier = name;
	return expr;
}

bool isInteger(Expr* e) {
	return e->type == Integer;
}

bool isBoolean(Expr* e) {
	return e->type == Boolean;
}

bool isCharacter(Expr* e) {
	return e->type == Character;
}

bool isIdentifier(Expr* e) {
	return e->type == Identifier;
}

bool isString(Expr* e) {
	return e->type == String;
}

bool isPair(Expr* e) {
	return e->type == Pair;
}

bool isNil(Expr* e) {
	return e->type == Nil;
}

bool isError(Expr* e) {
	return e->type == Error;
}
