#ifndef LEXER_H_
#define LEXER_H_

#include <stdio.h>
#include "types.h"

typedef enum {
    TOKEN_EOF,

    TOKEN_IDENTIFIER,

    TOKEN_STRING,
    TOKEN_INTEGER,
    TOKEN_BOOL,

    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_EQUALS,
    TOKEN_COMMA,
    TOKEN_PERIOD,

    TOKEN_ERRORS
} TokenType;

typedef struct {
    const char *data;
    size_t len;
} String;

typedef union {
    String string;
    i64 integer;
} TokenValue;

typedef struct {
    TokenType type;
    TokenValue value;
    size_t line;
    size_t column;
} Token;

typedef struct {
    Token *tokens;
    size_t count;
    size_t capacity;
} TokenStore;

int lexer(TokenStore *tokens, const char *fpath);

#endif

