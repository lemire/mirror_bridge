#include "mirror_bridge.hpp"
#include "particle.hpp"

MIRROR_BRIDGE_MODULE(particle,
    mirror_bridge::bind_class<Particle>(m, "Particle");
)
