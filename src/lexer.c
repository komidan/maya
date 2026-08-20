#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"
#include "types.h"
#define MAYA_IMPLEMENTATION
#define MAYA_MODULE_LOGS
// #include "../lib/libmaya.h"

static char *read_file(const char *fpath, size_t *length)
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
    if (size < 1)
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

static int read_string(const char *src, size_t *pos, size_t *col, size_t flen,
    String *string)
{
    // Skip the opening quote.
    (*pos)++;
    (*col)++;

    size_t start = *pos;

    while (*pos <= flen && src[*pos] != '"')
    {
        if (src[*pos] == '\n')
        {
            return 1;
        }

        // ignore escaped characters
        if (src[*pos] == '\\' && *pos + 1 < flen)
        {
            *pos += 2;
            *col += 2;
        }
        else
        {
            (*pos)++;
            (*col)++;
        }
    }

    // no closing quote
    if (*pos >= flen)
    {
        return 1;
    }

    string->data = src + start;
    string->len = *pos - start;

    // skip closing quote
    (*pos)++;
    (*col)++;

    return 0;
}

// static float read_float(const char* src, size_t *pos, size_t *col, size_t flen)

static i32 read_int(const char* src, size_t flen, size_t *pos, size_t *col,
    i64 *value)
{
    i64 result = 0;

    // safe guard against not being digit
    if (*pos >= flen || !isdigit((uchar)src[*pos]))
    {
        return 1;
    }

    while (*pos <= flen && isdigit((uchar)src[*pos]))
    {
        result = result * 10 + (src[*pos] - '0');

        (*pos)++;
        (*col)++;
    }

    *value = result;
    return 0;
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

    // Temp print token info
    printf("%zu:%zu %d %s\n", line, column, type, value);

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
            case '"': {
                s_col = column;
                String string;

                if (read_string( source, &pos, &column, flen, &string))
                {
                    fprintf(stderr, "Unterminated string at %zu:%zu\n", line,
                        s_col);

                    free(source);
                    return 1;
                }

                TokenValue value = {
                    .string = string
                };
                add_token(tokens, TOKEN_STRING, value, line, s_col);
                break;
            }

            // Integer Literal
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                printf("Number found at %zu:%zu\n", line, column);

                s_col = column;
                i64 result;

                i32 r = read_int(
                    source,
                    flen,
                    &pos,
                    &column,
                    &result
                );

                if (r)
                {
                    fprintf(stderr, "Number failed to parse at %zu:%zu\n", line, s_col);
                }
                TokenValue value = {
                    .integer = result
                };
                add_token(tokens, TOKEN_INTEGER, value, line, s_col);
                break;
            }

            default:
                pos++;
                column++;
                continue;
        }
    }

    free(source);

    return 0;
}
