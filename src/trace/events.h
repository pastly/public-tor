/* Copyright (c) 2015, The Tor Project, Inc. */
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
 * at compile time, they are specialized accoring to the first two arguments
 * being the subsystem and the name of the event.
 *
 * By default, every trace events are NOP. See doc/tracing.txt for more
 * information on how to use tracing or add other events.
 */

#if TOR_TRACE_SHADOW

/* Enable shadow tracing events. */
#include "shadow.h"

/* Define a specific tracing call namespaced with shadow. They MUST be
 * defined in the shadow header in order to use them. */
#define tor_trace(subsystem, name, args...) \
  tor_trace_shadow_##subsystem##_##name(args)

#else /* TOR_TRACE_SHADOW */

/*
 * Reaching this point, we NOP every tracing call since it was not enabled
 * at compile time.
 */
#define tor_trace(subsystem, name, args...)

#endif /* TOR_TRACE_SHADOW */

#endif /* TOR_TRACE_EVENTS_H */
