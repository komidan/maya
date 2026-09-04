#ifndef LEXER_H_
#define LEXER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// All possible token types that can be found in a `.toml` file.
typedef enum {
    TOKEN_EOF,      // 0
    TOKEN_KEY,      // 1

    TOKEN_STRING,   // 2
    TOKEN_INTEGER,  // 3
    TOKEN_BOOLEAN,  // 4

    TOKEN_LBRACKET, // 5
    TOKEN_RBRACKET, // 6
    TOKEN_EQUALS,   // 7
    TOKEN_COMMA,    // 8
    TOKEN_PERIOD,   // 9
} TokenType;

typedef struct {
    TokenType type;  // TokenType enum
    size_t line;     // Used for debug messages
    size_t column;   // Used for debug messages
    size_t pos;      // index into source
    size_t len;      // length from index the token occupies
} Token;

typedef struct {
    Token *tokens;   // array of tokens
    size_t count;    // contain count of tokens in array
    size_t capacity; // used for dynamically allocating more space

    char *source;    // source string from file
} TokenStore;

/**
 * Runs the lexer on `fpath` populating `*store` with token data
 *
 * @param *store    pointer to a TokenStore struct
 * @param *fpath    string containing file path to .toml file
 *
 * @return errnum
 */
int lexer(TokenStore *store, const char *fpath);

/**
 * Frees all the data of a store safely
 *
 * @param *store    TokenStore struct
 *
 * @noreturn
 */
void token_store_free(TokenStore *store);

/**
 * Prints a single token's data
 *
 * @param *store   TokenStore struct
 * @param i        index of the token to print
 *
 * @noreturn
*/
void token_print(TokenStore *store, uint64_t index);

#endif // LEXER_H_