#include "args.h"

#include <argp.h>
#include <stdio.h>
#include <stdlib.h>

const char       *argp_program_version     = "mousesim 1.0.0";
const char       *argp_program_bug_address = "<jtdubs@gmail.com>";
static const char doc[]                    = "mousesim -- a simulator for the mouse firmware";
static const char args_doc[]               = "FIRMWARE_FILE";

static struct argp_option options[] = {
    {.name = "gdb", .key = 'g', .arg = 0, .flags = 0, .doc = "Enable GDB server"},
    {0},
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  arguments_t *arguments = state->input;
  switch (key) {
    case 'g':
      arguments->gdb_enabled = true;
      break;
    case ARGP_KEY_ARG:
      switch (state->arg_num) {
        case 0:
          arguments->firmware_path = arg;
          break;
        default:
          // Too many args!
          argp_usage(state);
          break;
      }
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 1) {
        // Too few args!
        argp_usage(state);
        break;
      }
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {
    .doc      = doc,
    .args_doc = args_doc,
    .options  = options,
    .parser   = parse_opt,
};

arguments_t arguments = {
    .firmware_path = NULL,
    .gdb_enabled   = false,
};

arguments_t *parse_args(int argc, char *argv[]) {
  argp_parse(&argp, argc, argv, 0, 0, &arguments);
  return &arguments;
}
