#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <maya/lexer.h>

void token_print(TokenStore *store, size_t index)
{
    Token *token = &store->tokens[index];

    printf("[MAYA] #%d (%4zu:%-4zu) src[%zu]\t=  \"%.*s\"\n",
        token->type,
        token->line,
        token->column,
        token->pos,
        (int)token->len,
        store->source + token->pos
    );
}

void token_store_free(TokenStore *store)
{
    free(store->tokens);
    free(store->source);

    store->tokens = NULL;
    store->count = 0;
    store->capacity = 0;
}

static int token_add(TokenStore *store, TokenType type, size_t line,
    size_t column, size_t pos, size_t len)
{
    // Dynamically realloc for size
    if (store->count >= store->capacity)
    {
        // If tokens->capacity is 0 (first initialization), then set it to
        // capacity=16 as default, otherwise multiply capacity by 2.
        size_t new_capacity = store->capacity ? store->capacity * 2 : 16;

        Token *new_tokens = realloc(
            store->tokens,
            new_capacity * sizeof(*new_tokens)
        );

        if (!new_tokens)
        {
            fprintf(stderr, "Failed to realloc new tokens array\n");
            return 1;
        }

        store->tokens = new_tokens;
        store->capacity = new_capacity;
    }


    store->tokens[store->count++] = (Token) {
        .type = type,
        .line = line,
        .column = column,
        .pos = pos,
        .len = len,
    };

    return 0;
}

static char *read_file(const char *fpath, size_t *len)
{
    // open file pointer
    FILE *fp = fopen(fpath, "rb");
    if (!fp)
    {
        return NULL;
    }

    // get file size
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size < 0)
    {
        fclose(fp);
        return NULL;
    }

    size_t fsize = (size_t)size;
    rewind(fp);

    char *buffer = malloc(fsize + 1);
    if (!buffer)
    {
        fclose(fp);
        return NULL;
    }

    size_t bytes = fread(buffer, 1, fsize, fp);
    if (bytes != fsize)
    {
        free(buffer);
        return NULL;
    }

    // null terminate it
    buffer[bytes] = '\0';
    if (len)
    {
        *len = bytes;
    }

    fclose(fp);
    return buffer;
}

static int32_t read_key(const char *src, size_t fsize, size_t *pos)
{
    int32_t len = 0;

    while (*pos < fsize) {
        char c = src[*pos];

        if (!(isalnum((unsigned char)c) || c == '_' || c == '-'))
        {
            break;
        }

        (*pos)++;
        len++;
    }

    return len;
}

static int32_t read_string(const char *src, size_t fsize, size_t *pos)
{
    int32_t len = 0;

    // add one for the opening quote
    (*pos)++;

    while (*pos < fsize && src[*pos] != '"')
    {
        if (src[*pos] == '\n')
        {
            // TODO: Add line, column information to this.
            fprintf(stderr, "Unterminated string literal.\n");
            return -1;
        }

        // ignore escaped characters
        if (src[*pos] == '\\' && *pos + 1 < fsize)
        {
            *pos += 2;
            len += 2;
        }
        else
        {
            (*pos)++;
            len++;
        }
    }

    if (*pos >= fsize)
    {
        return -1;
    }

    // skip closing quote
    (*pos)++;

    return len;
}

static int32_t read_int(const char *src, size_t fsize, size_t *pos)
{
    if (*pos >= fsize || !isdigit((unsigned char)src[*pos]))
    {
        return -1;
    }

    int32_t len = 0;
    while (*pos < fsize && isdigit((unsigned char)src[*pos]))
    {
        len++;
        (*pos)++;
    }

    return len;
}

int lexer(TokenStore *store, const char *fpath)
{
    // position inside file, (0, fsize);
    size_t pos = 0;

    // positional tracker for tokens
    size_t line = 1;
    size_t column = 1;

    // used for loopering
    size_t fsize;

    // get source string
    store->source = read_file(fpath, &fsize);
    if (!store->source)
    {
        fprintf(stderr, "Failed to read file.\n");
        return 1;
    }

    while (pos < fsize)
    {
        char c = store->source[pos];

        // TODO: Finish this lexing step.
        switch (c)
        {
            // New Line
            case '\n':
                pos++;
                line++;
                column = 1;
                break;

            case '\t':
            case ' ':
                pos++;
                column++;
                break;

            // Ignore all comments.
            case '#':
                while (pos < fsize && store->source[pos] != '\n')
                {
                    pos++;
                    column++;
                }
                break;

            case '[':
                token_add(store, TOKEN_LBRACKET, line, column, pos, 1);
                pos++;
                column++;
                break;

            case ']':
                token_add(store, TOKEN_RBRACKET, line, column, pos, 1);
                pos++;
                column++;
                break;

            case '=':
                token_add(store, TOKEN_EQUALS, line, column, pos, 1);
                pos++;
                column++;
                break;

            case ',':
                token_add(store, TOKEN_COMMA, line, column, pos, 1);
                pos++;
                column++;
                break;

            case '.':
                token_add(store, TOKEN_PERIOD, line, column, pos, 1);
                pos++;
                column++;
                break;

            case '"': {
                int32_t len = read_string(
                    store->source,
                    fsize,
                    &pos
                );

                if (len < 1)
                {
                    fprintf(stderr, "Failed to parse string (%zu:%zu)\n",
                        line, column);

                    token_store_free(store);
                    return 1;
                }

                token_add(store, TOKEN_STRING, line, column, pos-(len+1), len);
                column += len;
                break;
            }

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                int32_t len = read_int(
                    store->source,
                    fsize,
                    &pos
                );

                if (len < 1)
                {
                    fprintf(stderr, "Failed to parse number (%zu:%zu)\n",
                        line, column);
                    token_store_free(store);
                    return 1;
                }

                token_add(store, TOKEN_INTEGER, line, column, pos-len, len);
                column += len;
                break;
            }

            // Keys / EOF
            default: {
                size_t start = pos;

                int32_t len = read_key(
                    store->source,
                    fsize,
                    &pos
                );

                if (len < 1)
                {
                    fprintf(stderr, "Failed to parse key (%zu:%zu)\n",
                        line, column);
                    token_store_free(store);
                    return 1;
                }

                TokenType type = TOKEN_KEY;

                // Is it a boolean value?
                if (len == 4 && memcmp(store->source+start,"true",4)==0)
                {
                    type = TOKEN_BOOLEAN;
                }
                else if (len == 5 && memcmp(store->source+start,"false",5)==0)
                {
                    type = TOKEN_BOOLEAN;
                }

                token_add(
                    store,
                    type,
                    line,
                    column,
                    pos-len,
                    len
                );

                column += len;
                break;
            }
        }
    }

    return 0;
}
