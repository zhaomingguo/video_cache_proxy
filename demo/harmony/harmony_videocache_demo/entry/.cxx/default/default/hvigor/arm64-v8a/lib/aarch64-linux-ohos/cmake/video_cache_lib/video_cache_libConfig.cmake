if(NOT TARGET video_cache_lib::videocache)
    add_library(video_cache_lib::videocache SHARED IMPORTED)
    set_target_properties(video_cache_lib::videocache PROPERTIES
        IMPORTED_LOCATION "/Users/mingo/Applications/workspace/jojo/harmony_videocache_demo/video_cache/build/default/intermediates/cmake/default/obj/arm64-v8a/libvideocache.so")
endif()
