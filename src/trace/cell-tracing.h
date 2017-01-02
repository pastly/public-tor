/* Copyright (c) 2015, The Tor Project, Inc. */
/* See LICENSE for licensing information */

#ifndef TOR_TRACE_EVENTS_CELL_TRACING_H
#define TOR_TRACE_EVENTS_CELL_TRACING_H

#include "or.h"

/*
 * Map the tracing event to a more specific one using the subsystem and
 * passing the name as argument. This is a way to void some subsystems for
 * cell tracing instead of declaring a function for each single event of that
 * subsystem.
 */

void cell_tracing_init(void);

#define TOR_TRACE_HAS_CHANNEL_TLS
#define tor_trace_channel_tls(name, ...) \
  tor_trace_channel_tls_##name(__VA_ARGS__)
/*
 * For instance, to NOP the "circuit" subsystem, simply define this:
 *    #define tor_trace_circuit(name, ...)
 *
 * To support an event:
 *    #define tor_trace_circuit(name, ##__VA_ARGS__) \
 *      tor_trace_circuit_##name(__VA_ARGS__)
 *
 * and just declare the function above to actually do something when the
 * event is hit.
 */

#define TOR_TRACE_HAS_CONNECTION
#define tor_trace_connection(name, ...) \
  tor_trace_connection_##name(__VA_ARGS__)

void tor_trace_channel_tls_write_packed_cell(connection_t *conn,
    const packed_cell_t *cell, int wide_circ_ids);
void tor_trace_connection_cell_inbuf(cell_t *cell);
void tor_trace_connection_write_to_buf_flushed(connection_t *conn, int amount);

#endif /* TOR_TRACE_EVENTS_CELL_TRACING_H */
