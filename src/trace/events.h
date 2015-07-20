/* Copyright (c) 2015, The Tor Project, Inc. */
/* See LICENSE for licensing information */

/**
 * \file events.h
 * \brief Header file for Tor event tracing.
 **/

#ifndef TOR_TRACE_EVENTS_H
#define TOR_TRACE_EVENTS_H

/* Shadow collection is enabled. */
#ifdef TOR_TRACE_SHADOW

/* This is the wrapper macro for declaring a tracepoint that is mapped to a
 * shadow tracepoint helper function. */
#define DECLARE_TP(subsystem, name, data, args...) \
  shadow_tracepoint(subsystem, name, data, ## args)

#else /* TOR_TRACE_SHADOW */

/* NOP the tracepoint is nothing is enabled. */
#define DECLARE_TP(subsystem, name, data, args...)

#endif /* TOR_TRACE_SHADOW */

/*
 * Trace events should be in a per subsystem header file and that header should
 * be included here so it can use the DECLARE_TP() macro above.
 */


#endif /* TOR_TRACE_EVENTS_H */
