/* Copyright (c) 2015, The Tor Project, Inc. */
/* See LICENSE for licensing information */

#include "shadow.h"

#include "buffers.h"
#include "or.h"

/* In tracing mode, a cell and var cell data structure have an ID field.
 * This is a global counter indicating the next ID a cell should get. It is
 * incremented every time the inbuf trace event is hit for either types. */
static uint32_t var_cell_next_id = 1;
static uint32_t cell_next_id = 1;

/* This value determines what cell IDs get traced. Essentially, if
 * cellid % TRACE_EVERY_N_CELLS == 0, then it will be traced. This cuts down
 * on log spam. Every cell gets an id, but only some get logged. */
#define TRACE_EVERY_N_CELLS 1

static digest256map_t *connections;

struct cell_info {
  uint32_t id;
  ssize_t outbuf_pos;
};

/* Thread local storage buffer that contains a formatted timestamp suitable
 * for tracing message output. */
__thread char time_buf[32];

/* Helper function: return a string formatted timestamp put in a thread
 * local storage buffer that is overwritten at every call. */
const char *
trace_add_ts(void)
{
  int ret;
  struct timespec tp;

  ret = clock_gettime(CLOCK_REALTIME, &tp);
  if (ret < 0) {
    goto error;
  }

  ret = tor_snprintf(time_buf, sizeof(time_buf), "%lu.%lu", tp.tv_sec,
                     tp.tv_nsec);
  if (ret < 0) {
    goto error;
  }

  return time_buf;

 error:
  /* Unable to get the time, return empty string so output does not fail. */
  return "";
}

/* Trace event hit when a var_cell_t is read from inbuf. */
void
tor_trace_connection_var_cell_inbuf(var_cell_t *cell, connection_t *conn)
{
  cell->id = var_cell_next_id++;
  shadow_log(LD_OR, "id=%" PRIu32 " read from "
                    "connection %" PRIu64 " inbuf",
             cell->id, conn->global_identifier);
}

/* Trace event hit when a cell_t is read from inbuf. */
void
tor_trace_connection_cell_inbuf(cell_t *cell, connection_t *conn)
{
  cell->id = cell_next_id++;
  if (cell->id % TRACE_EVERY_N_CELLS != 0) return;
  shadow_log(LD_OR, "id=%" PRIu32 " read from "
                    "connection %" PRIu64 " inbuf",
             cell->id, conn->global_identifier);
}

void
tor_trace_connection_cell_write_buf(const cell_t *cell, size_t cell_len,
                                    connection_t *conn)
{
  shadow_log(LD_OR, "id=%" PRIu32, cell->id);
  uint8_t key[DIGEST256_LEN] = {0};
  digest256map_t *infos;
  size_t outbuf_len = buf_datalen(conn->outbuf);
  struct cell_info *c_info;

  /* Outbound cells created by us means that we don't have a cell ID but we
   * also don't want to track it since we only track cells from inbuf to
   * outbuf. */
  if (cell->id == 0) {
    return;
  }

  if (connections == NULL) {
    connections = digest256map_new();
  }

  set_uint32(key, conn->global_identifier);
  infos = digest256map_get(connections, key);
  if (infos == NULL) {
    infos = digest256map_new();
    digest256map_set(connections, key, infos);
  }

  memset(key, 0, sizeof(key));
  set_uint32(key, cell->id);
  c_info = digest256map_get(infos, key);
  if (c_info != NULL) {
    /* XXX: Already in our state... why are we being called? */
    shadow_log(LD_OR, "cell_t id %" PRIu32 " already in our state.",
               c_info->id);
    return;
  }
  c_info = tor_malloc_zero(sizeof(struct cell_info));
  c_info->id = cell->id;
  c_info->outbuf_pos = outbuf_len - cell_len;
  digest256map_set(infos, key, c_info);
  shadow_log(LD_OR, "cell_t id %" PRIu32 " added to our state for "
                    "connection id %" PRIu64,
             c_info->id, conn->global_identifier);
  shadow_log(LD_OR, "\t position in outbuf: %lu", c_info->outbuf_pos);
}

void
tor_trace_connection_cell_flush_buf(const connection_t *conn,
                                    size_t num_written)
{
  uint8_t key[DIGEST256_LEN] = {0};

  if (connections == NULL) {
    connections = digest256map_new();
  }

  set_uint32(key, conn->global_identifier);
  digest256map_t *infos = digest256map_get(connections, key);
  if (infos == NULL) {
    return;
  }

  /* Go over all cells and update bytes written. */
  DIGEST256MAP_FOREACH_MODIFY(infos, k, struct cell_info *, c_info) {
    c_info->outbuf_pos -= num_written;
    shadow_log(LD_OR, "cell_t id %" PRIu32 " outbuf pos changed from %ld "
                      "to %ld for connection id %" PRIu64,
               c_info->id, c_info->outbuf_pos + num_written,
               c_info->outbuf_pos, conn->global_identifier);
    if (c_info->outbuf_pos <= 0) {
      shadow_log(LD_OR, "cell_t id %" PRIu32 " written to kernel "
                        "for connection id %" PRIu64,
                 c_info->id, conn->global_identifier);
      tor_free(c_info);
      MAP_DEL_CURRENT(k);
    }
  } DIGEST256MAP_FOREACH_END;
}

void tor_trace_channel_queue_cell(const cell_t *cell)
{
	//shadow_log(LD_OR, "id=%" PRIu32, cell->id);
}

void tor_trace_channel_queue_cell_need_to_queue(const cell_t *cell)
{
	//shadow_log(LD_OR, "id=%" PRIu32, cell->id);
}

void tor_trace_channel_process_cells()
{
	shadow_log(LD_OR, "");
}

void tor_trace_channel_process_cells_fixed(const cell_t *cell)
{
	shadow_log(LD_OR, "id=%" PRIu32, cell->id);
}

void tor_trace_channel_process_cells_var(const var_cell_t *cell)
{
	shadow_log(LD_OR, "id=%" PRIu32, cell->id);
}

void tor_trace_channel_process_cells_packed(const packed_cell_t *cell)
{
	shadow_log(LD_OR, "id=%" PRIu32, cell->id);
}

void tor_trace_channel_tls_write_cell(const cell_t *cell)
{
	shadow_log(LD_OR, "id=%" PRIu32, cell->id);
}

void tor_trace_channel_tls_write_var_cell(const var_cell_t *cell)
{
	shadow_log(LD_OR, "id=%" PRIu32, cell->id);
}

void tor_trace_channel_tls_write_packed_cell(const packed_cell_t *cell)
{
	if (cell->id == 48)
	{
		uint8_t command;
		char *fixed_cell = cell->body;
		fixed_cell += 2;
		command = get_uint8(fixed_cell);
		shadow_log(LD_OR, "FOOBAR id=48 command=%" PRIu8, command);
		fixed_cell += 2;
		command = get_uint8(fixed_cell);
		shadow_log(LD_OR, "FOOBAR id=48 command=%" PRIu8, command);
		shadow_log(LD_OR, "FOOBAR");
	}
	if (cell->id % TRACE_EVERY_N_CELLS != 0) return;
	shadow_log(LD_OR, "id=%" PRIu32, cell->id);
}

void tor_trace_channel_write_cell_queue_entry_fixed(const cell_t *cell)
{
    shadow_log(LD_OR, "id=%" PRIu32, cell->id);
}

void tor_trace_channel_write_cell_queue_entry_var(const var_cell_t *cell)
{
    shadow_log(LD_OR, "id=%" PRIu32, cell->id);
}

void tor_trace_channel_write_cell_queue_entry_packed(const packed_cell_t *cell)
{
    //shadow_log(LD_OR, "id=%" PRIu32, cell->id);
}


void tor_trace_command_process_relay_cell(const cell_t *cell)
{
	//shadow_log(LD_OR, "id=%" PRIu32, cell->id);
}

void tor_trace_connection_write_to_buf(const size_t len)
{
	shadow_log(LD_OR, "len=%lu", len);
}

void tor_trace_relay_about_to_append(const cell_t *cell)
{
	//shadow_log(LD_OR, "id=%" PRIu32, cell->id);
}

void tor_trace_relay_appended(const packed_cell_t *cell)
{
	//shadow_log(LD_OR, "id=%" PRIu32, cell->id);
}
