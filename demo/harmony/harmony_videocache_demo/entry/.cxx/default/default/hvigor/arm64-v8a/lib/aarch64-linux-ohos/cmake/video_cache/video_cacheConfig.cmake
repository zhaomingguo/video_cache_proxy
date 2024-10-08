if(NOT TARGET video_cache::video_cache)
    add_library(video_cache::video_cache SHARED IMPORTED)
    set_target_properties(video_cache::video_cache PROPERTIES
        IMPORTED_LOCATION "/Users/mingo/Applications/workspace/jojo/harmony_videocache_demo/video_cache/build/default/intermediates/cmake/default/obj/arm64-v8a/libvideo_cache.so")
endif()
