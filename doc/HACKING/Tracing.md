# Tracing #

This document describes how the tracing subsystem works in tor so developers
can add events to the code base but also hook them to a tracing framework.

## Basics ###

Tracing is seperated in two concepts, trace events and a tracer. The tracing
subsystem can be found in `src/trace`. The `events.h` header file is the
main file that maps the different tracers to trace events.

### Events ###

A trace event is basically a function from which we can pass any data that
we want to collect. In addition, we specify a context for the event such as
a subsystem and an event name.

A trace event in tor has the following standard format:

	tor_trace(subsystem, event\_name, args...)

The `subsystem` parameter is the name of the subsytem the trace event is in.
For example that could be "scheduler" or "vote" or "hs". The idea is to add
some context to the event so when we collect them we know where it's coming
from. The `event_name` is the name of the event which helps a lot with
adding some semantic to the event. Finally, `args` is any number of
arguments we want to collect.

Here is an example of a possible tracepoint in main():

	tor_trace(main, init_phase, argc)

The above is a tracepoint in the `main` subsystem with `init_phase` as the
event name and the int argc is passed to the event as well.

How `argc` is collected or used has nothing to do with the instrumentation
(adding trace events to the code). It is the work of the tracer this is why
the trace events and collection framework (tracer) are decoupled. You _can_
have trace events without a tracer.

### Tracer ###

In `src/trace/events.h`, we map the `tor_trace()` function to the right
tracer. A tracer support is only enabled at compile time. For instance, the
file `cell-tracing.{c|h}` contains the trace events mapped to functions used for
collecting statistics on how long a cell spends in a Tor process.

## Build System ##

This described how this is integrated into the build system of tor.

By default, every tracing events are disabled in tor that is `tor_trace()`
is a NOP.

To enable a tracer, there is a configure option on the form of:

	--enable-<tracer>-tracing

We have an option that will send every trace events to a `log_debug()` which
will print you the subsystem and name of the event but not the arguments for
technical reasons. This is useful if you want to quickly see if your trace
event is being hit or well written. To do so, use this configure option:

	--enable-log-debug-tracing

## Instrument Tor ##

This is pretty easy. Let's say you want to add a trace event in
`src/or/rendcache.c`, you only have to add this include statement:

	#include "trace/events.h"

Once done, you can add as many as you want `tor_trace()` that you want.
Please use the right subsystem (here it would be `hs`) and a unique name
that tells what the event is for. For example:

	tor_trace(hs, store_desc_as_client, desc, desc_id);

If you look in `src/trace/events.h`, you'll see that if cell tracing
is enabled, it will be mapped to a function called:

	tor_trace_hs_store_desc_as_client(desc, desc_id)

And the point of all this is for that function to be defined in
`src/trace/cell-tracing.{c|h}` and mapped to whatever the researcher wants to
use for recording data (e.g: fprintf()).


