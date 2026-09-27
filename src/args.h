/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef args__h_
#define args__h_

#include <stdbool.h>

#define ARGS_KEY_ARG  0x40000000
#define ARGS_KEY_INIT 0x40000001
#define ARGS_KEY_END  0x40000002
#define ARGS_KEY_ERR  0x40000003
#define ARGS_KEY_FINI 0x40000004

#define ARGS_NO_EXIT 0x01u

struct args_option;
struct args_program;
struct args_state;

typedef bool (*args_parser)(int key, const char *arg, struct args_state *state);

struct args_option {
    int key;
    const char *name;
    const char *arg;
};

struct args_program {
    const struct args_option *options;
    args_parser parser;
    void *input;
};

struct args_state {
    int arg_num;
    void *input;
    const void *priv;
};

bool args_parse(
    const struct args_program *program,
    int argc,
    char const *const *argv,
    unsigned flags
);

#endif // args__h_
