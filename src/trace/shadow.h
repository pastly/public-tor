/* Copyright (c) 2015, The Tor Project, Inc. */
/* See LICENSE for licensing information */

#ifndef TOR_TRACE_EVENTS_SHADOW_H
#define TOR_TRACE_EVENTS_SHADOW_H

#include "or.h"

/*
 * Map the tracing event to a more specific one using the subsystem and
 * passing the name as argument. This is a way to void some subsystems for
 * shadow instead of declaring a function for each single event of that
 * subsystem.
 */

const char *trace_add_ts(void);

/* Helper function: standardize message for the trace output. */
#define shadow_log(domain, fmt, args...) \
  log_debug(domain, "[shadow] %s: " fmt "\n", trace_add_ts(), ## args);

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

void tor_trace_connection_cell_inbuf(cell_t *cell, connection_t *conn);
void tor_trace_connection_var_cell_inbuf(var_cell_t *cell,
                                         connection_t *conn);

void tor_trace_connection_cell_write_buf(const cell_t *cell,
                                         size_t cell_len,
                                         connection_t *conn);
void
tor_trace_connection_cell_flush_buf(const connection_t *conn,
                                    size_t num_written);

#endif /* TOR_TRACE_EVENTS_SHADOW_H */
