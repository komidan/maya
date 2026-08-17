#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "types.h"

char *read_file(const char *fpath, size_t *length)
{
    // create file pointer
    FILE *fp = fopen(fpath, "rb");
    if (!fp)
    {
        return NULL;
    }

    // get the file size
    fseek(fp, 0, SEEK_END);
    u64 size = ftell(fp);
    if (size < 0)
    {
        fclose(fp);
        return NULL;
    }
    rewind(fp);

    // create the buffer for holding the files contents
    char *buffer = malloc((size_t)size + 1);
    if (!buffer)
    {
        fclose(fp);
        return NULL;
    }

    // bytes read
    size_t bytes = fread(buffer, 1, (size_t)size, fp);
    if (bytes != (size_t)size)
    {
        free(buffer);
        return NULL;
    }

    // add a null terminator for easy while-loop
    buffer[bytes] = '\0';

    // assign length to size of file
    if (length)
    {
        *length = bytes;
    }

    return buffer;
}

static char* read_string(const char *source, size_t *pos, size_t *column,
    size_t flen)
{
    // Skip the opening quote.
    (*pos)++;
    (*column)++;

    size_t start = *pos;

    while (*pos < flen && source[*pos] != '"')
    {
        if (source[*pos] == '\n')
        {
            return NULL;
        }

        // ignore escaped characters
        if (source[*pos] == '\\' && *pos + 1 < flen)
        {
            *pos += 2;
            *column += 2;
        }
        else
        {
            (*pos)++;
            (*column)++;
        }
    }

    // no closing quote
    if (*pos > flen)
    {
        return NULL;
    }

    size_t len = *pos - start;

    char *str = malloc(len + 1);
    if (!str)
    {
        return NULL;
    }

    memcpy(str, source + start, len);
    str[len] = '\0';

    // skip closing quote
    (*pos)++;
    (*column)++;

    return str;
}

static int add_token(TokenStore *tokens, TokenType type,
    TokenValue value, size_t line, size_t column)
{
    if (tokens->count >= tokens->capacity)
    {
        // If tokens->capacity is 0 (when it first gets initialized),
        // then we set it to 16 as default, otherwise set it to capacity * 2.
        size_t new_capacity = tokens->capacity ? tokens->capacity * 2 : 16;

        // new list of token_t
        Token *new_tokens = realloc(
            tokens->tokens,
            new_capacity * sizeof(*new_tokens)
        );

        if (!new_tokens)
        {
            return 1;
        }

        tokens->tokens = new_tokens;
        tokens->capacity = new_capacity;
    }

    // add the token to the token array
    tokens->tokens[tokens->count++] = (Token) {
        .type = type,
        .value = value,
        .line = line,
        .column = column
    };

    return 0;
}

int lexer(TokenStore *tokens, const char *fpath)
{
    size_t pos = 0;
    size_t line = 1;
    size_t column = 1;

    size_t flen;
    char *source = read_file(fpath, &flen);
    if (!source)
    {
        fprintf(stderr, "Failed to read file.\n");
        return 1;
    }

    while (pos < flen)
    {
        char c = source[pos];
        size_t s_col = 0;

        switch (c)
        {
            // new line
            case '\n':
                pos++;
                line++;
                column = 1;
                break;

            // comments
            case '#':
                while (source[pos] != '\n')
                {
                    pos++;
                    column++;
                }
                break;

            case '[':
                add_token(tokens, TOKEN_LBRACKET, (TokenValue){0},
                    line, column);
                pos++;
                column++;
                break;

            case ']':
                add_token(tokens, TOKEN_RBRACKET, (TokenValue){0},
                    line, column);
                pos++;
                column++;
                break;

            case '=':
                add_token(tokens, TOKEN_EQUALS, (TokenValue){0}, line, column);
                pos++;
                column++;
                break;

            case ',':
                add_token(tokens, TOKEN_COMMA, (TokenValue){0}, line, column);
                pos++;
                column++;
                break;

            case '.':
                add_token(tokens, TOKEN_PERIOD, (TokenValue){0}, line, column);
                pos++;
                column++;
                break;

            // String Literals
            case '"':
                s_col = column;

                char *string = read_string(
                    source,
                    &pos,
                    &column,
                    flen
                );

                if (!string)
                {
                    fprintf(stderr, "Unterminated string at line %zu, column %zu\n",
                    line, s_col);

                    free(source);
                    return 1;
                }

                TokenValue value = {
                    .string = string
                };

                add_token(tokens, TOKEN_STRING, value, line, s_col);

            default:
                pos++;
                column++;
                continue;
        }
    }

    free(source);

    return 0;
}
