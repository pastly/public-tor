/* Copyright (c) 2015, The Tor Project, Inc. */
/* See LICENSE for licensing information */

#ifndef TOR_TRACE_EVENTS_SHADOW_H
#define TOR_TRACE_EVENTS_SHADOW_H

/*
 * Map the tracing event to a more specific one using the subsystem and
 * passing the name as argument. This is a way to void some subsystems for
 * shadow instead of declaring a function for each single event of that
 * subsystem.
 */

/*
 * For instance, to NOP the "circuit" subsystem, simply define this:
 *    #define tor_trace_shadow_circuit(name, ...)
 *
 * To support an event:
 *    #define tor_trace_shadow_circuit(name, ##__VA_ARGS__) \
 *      tor_trace_shadow_circuit_##name(__VA_ARGS__)
 *
 * and just declare the function above to actually do something when the
 * event is hit.
 */

/* Support the connection subsystem. */
#define TOR_TRACE_HAS_CONNECTION
#define tor_trace_connection(name, ...) \
  tor_trace_connection_##name(__VA_ARGS__)

#include <stdio.h>

static inline void
tor_trace_connection_()
{
}

#endif /* TOR_TRACE_EVENTS_SHADOW_H */
