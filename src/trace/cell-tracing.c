/* Copyright (c) 2015, The Tor Project, Inc. */
/* See LICENSE for licensing information */

#include "cell-tracing.h"

#include "buffers.h"
#include "or.h"
#include "config.h"

/* In tracing mode, a cell data structure has an ID field.
 * This is a global counter indicating the next ID a cell should get. It is
 * incremented every time the inbuf trace event is hit and
 * cell_counter == trace_every_n. */
static uint32_t cell_next_id = 1;

/* This value reduces the number of cells that get traced. We count cells until
 * the counter reaches trace_every_n, at which point we increment and
 * assign the cell a non-zero id. We also attach a non-zero timestamp. This
 * cuts down on log spam. */
static int trace_every_n;
static int cell_counter = 0;

/* Whether or not tracing is enabled. First it must be enabled at compile time,
 * then the torrc option enabled.
 */
static int tracing_enabled;

static digest256map_t *connections;

struct cell_info {
  uint32_t id;
  struct timespec ts;
  ssize_t outbuf_pos;
};

void cell_tracing_init()
{
  tracing_enabled = get_options()->CellTracingEnabled;
  trace_every_n = get_options()->CellTracingEveryNCells;
  log_info(LD_OR, "[cell-tracing] (re)init cell tracing. enabled: %" PRIi32
	", n: %" PRIi32, tracing_enabled, trace_every_n);
}

void tor_trace_channel_tls_write_packed_cell(connection_t *conn,
  const packed_cell_t *cell, int wide_circ_ids)
{
  if (!tracing_enabled) return;
  if (cell->id < 1) return;

  uint8_t key[DIGEST256_LEN] = {0};
  digest256map_t *infos;
  size_t outbuf_len = buf_datalen(conn->outbuf);
  struct cell_info *c_info;

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
    log_info(LD_OR, "[cell-tracing] id=%" PRIu32 " already in our state",
        c_info->id);
    return;
  }
  c_info = tor_malloc_zero(sizeof(struct cell_info));
  c_info->id = cell->id;
  c_info->ts = cell->ts;
  c_info->outbuf_pos = outbuf_len + get_cell_network_size(wide_circ_ids);
  digest256map_set(infos, key, c_info);
  struct timespec ts;
  int result = clock_gettime(CLOCK_REALTIME, &ts);
  if (result < 0) ts.tv_sec = ts.tv_nsec = 0;
  int64_t diff = ( ((uint64_t)ts.tv_sec) *1000000000 + ts.tv_nsec ) -
          ( ((uint64_t)c_info->ts.tv_sec)*1000000000 + c_info->ts.tv_nsec );
  log_info(LD_OR, "[cell-tracing] %" PRIu32 ".%" PRIu32 " %" PRIu32 ".%" PRIu32
	" %" PRIi64 " id=%" PRIu32 " waiting in outbuf", c_info->ts.tv_sec,
	c_info->ts.tv_nsec, ts.tv_sec, ts.tv_nsec, diff, c_info->id);
}

void tor_trace_connection_cell_inbuf(cell_t *cell, connection_t *conn)
{
  if (!tracing_enabled) return;
  if (++cell_counter >= trace_every_n) {
    int result = clock_gettime(CLOCK_REALTIME, &(cell->ts));
    if (result < 0) cell->ts.tv_sec = cell->ts.tv_nsec = 0;
    cell->id = cell_next_id++;
    cell_counter -= trace_every_n;
    log_info(LD_OR, "[cell-tracing] %" PRIu32 ".%" PRIu32 " id=%" PRIu32,
	  cell->ts.tv_sec, cell->ts.tv_nsec, cell->id);
  } else {
    cell->id = 0;
    cell->ts.tv_sec = cell->ts.tv_nsec = 0;
  }
}

void tor_trace_connection_write_to_buf_flushed(connection_t *conn, int amount)
{
  if (!tracing_enabled) return;
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
    c_info->outbuf_pos -= amount;
    if (c_info->outbuf_pos <= 0) {
      struct timespec ts;
      int result = clock_gettime(CLOCK_REALTIME, &ts);
      if (result < 0)
        ts.tv_sec = ts.tv_nsec = 0;
      int64_t diff = ( ((uint64_t)ts.tv_sec)*1000000000 + ts.tv_nsec ) -
        ( ((uint64_t)c_info->ts.tv_sec)*1000000000 + c_info->ts.tv_nsec );
      log_info(LD_OR, "[cell-tracing] %" PRIu32 ".%" PRIu32 " %" PRIu32
        ".%" PRIu32 " %" PRIi64 " id=%" PRIu32 " written to kernel",
        c_info->ts.tv_sec, c_info->ts.tv_nsec, ts.tv_sec, ts.tv_nsec, diff,
        c_info->id);
      tor_free(c_info);
      MAP_DEL_CURRENT(k);
    }
  } DIGEST256MAP_FOREACH_END;
}
