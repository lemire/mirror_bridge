// Manual binding for Lua (without ResourceHolder due to smart pointer issue)
#include "lua/mirror_bridge_lua.hpp"
#include "features.hpp"

MIRROR_BRIDGE_LUA_MODULE(features_lua,
    mirror_bridge::lua::bind_class<VectorTest>(L, "VectorTest");
    mirror_bridge::lua::bind_class<Resource>(L, "Resource");
    // mirror_bridge::lua::bind_class<ResourceHolder>(L, "ResourceHolder");  // TODO: Fix smart pointer support
    mirror_bridge::lua::bind_class<Address>(L, "Address");
    mirror_bridge::lua::bind_class<Person>(L, "Person");
    mirror_bridge::lua::bind_class<ColoredShape>(L, "ColoredShape");
)
