// Callback binding example
// Shows how to bind classes that use std::function callbacks

#include "python/mirror_bridge_python.hpp"
#include "event_emitter.hpp"

MIRROR_BRIDGE_MODULE(callbacks,
    mirror_bridge::bind_class<EventEmitter>(m, "EventEmitter");
    mirror_bridge::bind_class<DataProcessor>(m, "DataProcessor");
)
