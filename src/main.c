#include <stdio.h>

// #include "parser.h"
#include "lexer.h"

TokenStore tokens;

// NOTE: Eventually, we'll want to support passing arguments of some kind, like
// `maya --debug` to execute the debug parameters, or `maya --release`.
int main(void)
{
    // TODO: Make the check for the file more robust and give proper error
    // messages.
    if (lexer(&tokens, "maya_test.toml") != 0)
    {
        fprintf(stderr, "Something went wrong (file not found?).\n");
    }

    // parse(&config, &tokens);

    return 0;
}
