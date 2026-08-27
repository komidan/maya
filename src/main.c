#include <stdio.h>

// #include <maya/parser.h>
#include <maya/lexer.h>

// NOTE: Eventually, we'll want to support passing arguments of some kind, like
// `maya --debug` to execute the debug parameters, or `maya --release`.
int main(void)
{
    TokenStore store = {0};

    if (lexer(&store, "maya_test.toml"))
    {
        return 1;
    }

    for (size_t i = 0; i < store.count; i++)
    {
        token_print(&store, i);
    }

    // token_parse(&config, &tokens);

    token_store_free(&store);

    return 0;
}
