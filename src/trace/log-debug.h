/* Copyright (c) 2015, The Tor Project, Inc. */
/* See LICENSE for licensing information */

#ifndef TOR_TRACE_LOG_DEBUG_H
#define TOR_TRACE_LOG_DEBUG_H

#include "torlog.h"

/* Stringify pre-processor trick. */
#define XSTR(d) STR(d)
#define STR(s) #s

/* Send every event to a debug log level. This is useful to debug new trace
 * events without implementing them for a specific tracing framework. Note
 * that the arguments are ignored since at this step we do not know the
 * types and amount there is. */
#undef tor_trace_crypto
#define tor_trace_crypto(name, args...) \
  log_debug(LD_GENERAL, "Trace event \"" XSTR(name) "\" from crypto " \
                        "subsystem hit. (line " XSTR(__LINE__) ")")

#endif /* TOR_TRACE_LOG_DEBUG_H */
