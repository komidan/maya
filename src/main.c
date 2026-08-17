#include <stdio.h>

// #include "parser.h"
#include "lexer.h"

TokenStore tokens;
// maya_config_t config;

int main(int argc, char *argv[])
{
    if (lexer(&tokens, "maya.toml") != 0)
    {
        fprintf(stderr, "Something went wrong lexing.\n");
    }


    // parse(&config, &tokens);

    return 0;
}
