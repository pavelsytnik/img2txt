/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (c) 2026 Pavlo Sytnyk.                                      *
 * Licensed under the MIT License. See LICENSE for license information.  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef args__h_
#define args__h_

#include <stdbool.h>

// Argument parser events
#define ARGS_KEY_ARG  0x40000000  // Non-option argument
#define ARGS_KEY_INIT 0x40000001  // Parser initialization
#define ARGS_KEY_END  0x40000002  // Parsing completed successfully
#define ARGS_KEY_ERR  0x40000003  // Parsing failed
#define ARGS_KEY_FINI 0x40000004  // Parser finalization, called even after an error

// Parser flags
#define ARGS_NO_EXIT 0x01u  // Do not terminate if an error occurred

struct args_option;
struct args_program;
struct args_state;

// Called by the argument parser for each option and event
typedef bool (*args_parser_fn)(int key, const char *arg, struct args_state *state);

struct args_option {
    int key;           // The short option name and a key for the parser
    const char *name;  // The long option name
    bool has_arg;      // Whether the option requires an argument
};

struct args_program {
    const struct args_option *options;  // Option table
    args_parser_fn parser;              // Parsing callback
    void *input;                        // User-defined input passed to the callback
};

struct args_state {
    int arg_num;       // Number of positional arguments processed
    void *input;       // User-defined input
    const void *priv;  // Should be used internally by the parser in the future
};

bool args_parse(
    const struct args_program *program,
    int argc,
    char const *const *argv,
    unsigned flags
);

#endif // args__h_
