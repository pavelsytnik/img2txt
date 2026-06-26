/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef args_h__
#define args_h__

#define ARGS_KEY_ARG 0x80000000
#define ARGS_KEY_INIT 0x80000001
#define ARGS_KEY_END 0x80000002

struct args_option;
struct args_program;

typedef void (*args_parser)(int key, const char *arg, void *data);

struct args_option {
    int key;
    const char *name;
    const char *arg;
};

struct args_program {
    const struct args_option *options;
    args_parser parser;
    void *data;
};

void args_parse(const struct args_program *program, int argc, char const *const *argv);

#endif // args_h__
