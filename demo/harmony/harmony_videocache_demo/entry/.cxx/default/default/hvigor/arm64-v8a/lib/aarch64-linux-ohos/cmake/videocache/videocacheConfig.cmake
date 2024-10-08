if(NOT TARGET videocache::videocache)
    add_library(videocache::videocache SHARED IMPORTED)
    set_target_properties(videocache::videocache PROPERTIES
        IMPORTED_LOCATION "/Users/mingo/Applications/workspace/jojo/harmony_videocache_demo/videocache/build/default/intermediates/cmake/default/obj/arm64-v8a/libvideocache.so")
endif()
