/* Copyright (c) 2016, The Tor Project, Inc. */
/* See LICENSE for licensing information */

/**
 * \file events.h
 * \brief Header file for Tor event tracing.
 **/

#ifndef TOR_TRACE_EVENTS_H
#define TOR_TRACE_EVENTS_H

/*
 * The following defines the generic tracing function name that is used
 * accross the whole code base. Depending on the tracing framework enabled
 * at compile time, a tracepoint is specialized accoring to the first two
 * arguments being the subsystem and the name of the event.
 *
 * By default, every trace events are NOP. See doc/HACKING/Tracing.md for
 * more information on how to use tracing or add events.
 */

#ifdef TOR_TRACING_ENABLED
#include "common/torlog.h"
/* Map every trace event to a per subsystem macro. */
#define tor_trace(subsystem, name, ...) \
  tor_trace_##subsystem(name, __VA_ARGS__)

/* Enable tracing events. */
#ifdef USE_CELL_TRACING
#include "cell-tracing.h"
#endif
#ifdef USE_LOG_DEBUG_TRACING
#include "log-debug.h"
#endif

#else /* TOR_TRACING_ENABLED */

/*
 * Reaching this point, we NOP every tracing declaration because tracing has
 * not been enabled at compile time.
 */
#define tor_trace(subsystem, name, args...)

#endif /* TOR_TRACING_ENABLED */

#endif /* TOR_TRACE_EVENTS_H */
