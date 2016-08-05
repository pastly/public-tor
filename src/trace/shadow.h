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

void shadow_tracing_init();

/* Helper function: standardize message for the trace output. */
#define shadow_log(domain, fmt, args...) \
  log_info(domain, "[shadow] %s: " fmt "\n", trace_add_ts(), ## args);

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

#define TOR_TRACE_HAS_COMMAND
#define tor_trace_command(name, ...) \
  tor_trace_command_##name(__VA_ARGS__)

#define TOR_TRACE_HAS_CONNECTION
#define tor_trace_connection(name, ...) \
  tor_trace_connection_##name(__VA_ARGS__)

#define TOR_TRACE_HAS_CHANNEL
#define tor_trace_channel(name, ...) \
  tor_trace_channel_##name(__VA_ARGS__)

#define TOR_TRACE_HAS_CHANNEL_TLS
#define tor_trace_channel_tls(name, ...) \
  tor_trace_channel_tls_##name(__VA_ARGS__)

#define TOR_TRACE_HAS_RELAY
#define tor_trace_relay(name, ...) \
  tor_trace_relay_##name(__VA_ARGS__)

void tor_trace_connection_cell_inbuf(cell_t *cell, connection_t *conn);
//void tor_trace_connection_var_cell_inbuf(var_cell_t *cell,
//                                         connection_t *conn);

//void tor_trace_connection_cell_write_buf(const cell_t *cell,
//                                         size_t cell_len,
//                                         connection_t *conn);
//void
//tor_trace_connection_cell_flush_buf(const connection_t *conn,
//                                    size_t num_written);

//void tor_trace_command_process_relay_cell(const cell_t *cell);
//void tor_trace_connectiona_write_to_buf(const size_t len);
//void tor_trace_channel_queue_cell(const cell_t *cell);
//void tor_trace_channel_queue_cell_need_to_queue(const cell_t *cell);
//void tor_trace_channel_process_cells();
//void tor_trace_channel_process_cells_fixed(const cell_t *cell);
//void tor_trace_channel_process_cells_var(const var_cell_t *cell);
//void tor_trace_channel_process_cells_packed(const packed_cell_t *cell);
//void tor_trace_channel_tls_write_cell(const cell_t *cell);
//void tor_trace_channel_tls_write_var_cell(const var_cell_t *cell);
void tor_trace_channel_tls_write_packed_cell(const packed_cell_t *cell);
//void tor_trace_channel_write_cell_queue_entry_fixed(const cell_t *cell);
//void tor_trace_channel_write_cell_queue_entry_var(const var_cell_t *cell);
//void tor_trace_channel_write_cell_queue_entry_packed(const packed_cell_t *cell);

//void tor_trace_relay_about_to_append(const cell_t *cell);
//void tor_trace_relay_appended(const packed_cell_t *cell);

#endif /* TOR_TRACE_EVENTS_SHADOW_H */
