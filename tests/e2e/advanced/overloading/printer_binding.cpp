#include "mirror_bridge.hpp"
#include "printer.hpp"

MIRROR_BRIDGE_MODULE(printer,
    mirror_bridge::bind_class<Printer>(m, "Printer");
)
