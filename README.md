# video_cache_proxy

# Mobile Video Caching Library

This library is a video caching library designed for mobile platforms (iOS/Android/HarmonyOS), based on an HTTP caching proxy implemented with `libevent`. It currently supports caching for MP4 and HLS videos, enhancing playback smoothness and reducing load times.

## Features

1. **Cross-Platform Support**: Supports video caching on Android, iOS, and HarmonyOS.
2. **Video Format Support**: Supports caching of MP4 and HLS video formats.
3. **HTTP Caching Proxy**: A high-performance HTTP caching proxy built with `libevent`.
4. **HTTP 302 Redirection Support**: Handles HTTP 302 redirects to ensure proper video resource access.
5. **Domain IP Pool Support**: Configurable IP pools for domains, offering flexible resource access.
6. **Cache Management**: Supports customizable cache eviction mechanisms.
7. **LRU Eviction Strategy**: Implements an LRU (Least Recently Used) cache eviction strategy to ensure cache efficiency.