/* Copyright (c) 2015, The Tor Project, Inc. */
/* See LICENSE for licensing information */

#ifndef TOR_TRACE_EVENTS_SHADOW_H
#define TOR_TRACE_EVENTS_SHADOW_H

/*
 * Map the tracing event to a more specific one using the subsystem and
 * passing the name as argument. This is a way to void some subsystems for
 * shadow instead of declaring a function for each single event.
 */

 /*
 * For instance, to NOP the "circuit" subsystem, simply define this:
 *    #define tor_trace_shadow_circuit(name, ...)
 *
 * To support an event:
 *    #define tor_trace_shadow_circuit(name, ##__VA_ARGS__) \
 *      tor_trace_shadow_circuit_##name(__VA_ARGS__)

 * and just declare this function above to do anything with it.
#define tor_trace_shadow(subsystem, name, ...) \
  tor_trace_shadow_##subsystem##(name, ##__VA_ARGS__)
*/

/* Crypto subsystem. */
#define TOR_TRACE_HAS_CRYPTO
#define tor_trace_crypto(name, ...) tor_trace_crypto_##name(__VA_ARGS__)

#include <stdio.h>

static inline void
tor_trace_crypto_main(int argc, int num)
{
  fprintf(stderr, "argc: %d, num: %d\n", argc, num);
}

#endif /* TOR_TRACE_EVENTS_SHADOW_H */
