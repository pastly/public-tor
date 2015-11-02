/* Copyright (c) 2015, The Tor Project, Inc. */
/* See LICENSE for licensing information */

#include "shadow.h"
#include <stdio.h>

void tor_trace_shadow_main_start(int argc)
{
  fprintf(stderr, "ARGC: %d\n", argc);
}
