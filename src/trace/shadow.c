/* Copyright (c) 2015, The Tor Project, Inc. */
/* See LICENSE for licensing information */

#include "shadow.h"

#include "buffers.h"
#include "or.h"
#include "config.h"

/* In tracing mode, a cell and var cell data structure have an ID field.
 * This is a global counter indicating the next ID a cell should get. It is
 * incremented every time the inbuf trace event is hit for either types. */
static uint32_t var_cell_next_id = 1;
static uint32_t cell_next_id = 1;

/* This value determines what cell IDs get traced. Essentially, if
 * cellid % ShadowTraceEveryNCells == 0, then it will be traced. This cuts down
 * on log spam. Every cell gets an id, but only some get logged. */
int ShadowTraceEveryNCells;

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

void shadow_tracing_init() {ShadowTraceEveryNCells = get_options()->ShadowTraceEveryNCells;}

/* Trace event hit when a cell_t is read from inbuf. */
void
tor_trace_connection_cell_inbuf(cell_t *cell, connection_t *conn)
{
  cell->id = cell_next_id++;
  if (cell->id % ShadowTraceEveryNCells != 0) return;
  shadow_log(LD_OR, "id=%" PRIu32 " read from "
                    "connection %" PRIu64 " inbuf",
             cell->id, conn->global_identifier);
}

void tor_trace_channel_tls_write_packed_cell(const packed_cell_t *cell)
{
	if (cell->id % ShadowTraceEveryNCells != 0) return;
	shadow_log(LD_OR, "id=%" PRIu32, cell->id);
}
