#include <stdio.h>
#include <stdlib.h>
#include <maya/lexer.h>

void token_print(TokenStore *store, size_t index)
{
    Token *token = &store->tokens[index];

    printf("[MAYA] #%d (%zu:%zu) src[%zu] = \"%.*s\"\n",
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

        size_t start_pos = pos;

        // TODO: Finish this lexing step.
        switch (c)
        {
            // New Line
            case '\n':
                pos++;
                line++;
                column = 1;
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

            default:
                pos++;
                column++;
                break;
        }
    }

    return 0;
}
