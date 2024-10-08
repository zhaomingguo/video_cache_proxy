import { constant } from "@kit.ConnectivityKit";

export const initPlayer: () => number;

export const setDataSource: (handel: number, source: string) => void;

export const prepare: (handel: number) => void;

export const setAutoPlay: (handel: number, autoPlay: boolean) => void;

export const isAutoPlay: (handel: number) => number;

export const start: (handel: number) => void;

export const setPreparedListener: (handel: number, listener: () => void) => void;

export const setSurfaceId: (handel: number, surfaceId: string, width: number, height: number) => void;

export const pause: (handel: number) => void;

export const stop: (handel: number) => void;

export const setPositionUpdateListener: (handel: number, listener: (position: number) => void) => void;

export const setLoadStartListener: (handel: number, listener: () => void) => void;

export const setLoadProgressListener: (handel: number, listener: (progress: number) => void) => void;

export const setLoadEndListener: (handel: number, listener: () => void) => void;

export const getDuration: (handel: number) => number;

export const setErrorListener: (handel: number, listener: (errorCode: number, errorMsg: string) => void) => void;

export const release: (handel: number) => void;

export const setBufferUpdateListener: (handel: number, listener: (position: number) => void) => void;

export const seek: (handel: number, position: number, accurate: boolean) => void;

export const setCompleteListener: (handel: number, listener: () => void) => void;

export const setOffsetScreenSize: (handel: number, width: number, height: number) => void;

export const setFboTextureListener: (handel: number, listener: (textureId: number) => void) => void;

export const getCurrentPosition: (handel: number) => number;

export const getVideoWidth: (handel: number) => number;

export const getVideoHeight: (handel: number) => number;

export const setAutoPlayListener: (handel: number, l: () => void) => void;

export const setSeekCompleteListener: (handel: number, l: () => void) => void;

export const setRenderStartListener: (handel: number, l: () => void) => void;

export const setLoop: (handel: number, loop: boolean) => void;

export const isLoop: (handel: number) => number;

export const setMute: (handel: number, mute: Boolean) => void;

export const isMute: (handel: number) => number;

export const setVolume: (handel: number, volume: number) => void;

export const getVolume: (handel: number) => number;

export const setSpeed: (handel: number, speed: number) => void;

export const hasAudio: (handel: number) => number;

export const enableLog: (handel: number, enable: boolean) => void;

export const setLogInfoListener: (handel: number,
  l: (level: number, msg: string, ptr: number, sourceMd5: string) => void) => void;

// complete
// seek

