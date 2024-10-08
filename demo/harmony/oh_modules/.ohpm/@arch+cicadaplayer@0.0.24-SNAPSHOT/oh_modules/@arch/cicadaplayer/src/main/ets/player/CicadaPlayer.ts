import nativePlayer from 'libcicadaplayer.so';

export class CicadaPlayer {
  private playerHandel: number = 0

  constructor() {
    this.playerHandel = nativePlayer.initPlayer();
  }

  public setDataSource(source: string) {
    nativePlayer.setDataSource(this.playerHandel, source);
  }

  public prepare() {
    nativePlayer.prepare(this.playerHandel);
  }

  public setAutoPlay(on: boolean) {
    nativePlayer.setAutoPlay(this.playerHandel, on);
  }

  public start() {
    nativePlayer.start(this.playerHandel);
  }

  public setOnPreparedListener(l: () => void) {
    nativePlayer.setPreparedListener(this.playerHandel, l);
  }

  public setSurfaceId(surfaceId: string, width: number = 0, height: number = 0) {
    nativePlayer.setSurfaceId(this.playerHandel, surfaceId, width, height);
  }

  public pause() {
    nativePlayer.pause(this.playerHandel);
  }

  public stop() {
    nativePlayer.stop(this.playerHandel);
  }

  public setPositionUpdateListener(l: (position: number) => void) {
    nativePlayer.setPositionUpdateListener(this.playerHandel, l);
  }

  public setLoadStartListener(l: () => void) {
    nativePlayer.setLoadStartListener(this.playerHandel, l);

  }

  public setLoadProgressListener(l: (progress: number) => void) {
    nativePlayer.setLoadProgressListener(this.playerHandel, l);
  }

  public setLoadEndListener(l: () => void) {
    nativePlayer.setLoadEndListener(this.playerHandel, l);
  }

  public getDuration(): number {
    return nativePlayer.getDuration(this.playerHandel);
  }

  public setErrorListener(l: (errorCode: number, errorMsg: string) => void) {
    nativePlayer.setErrorListener(this.playerHandel, l);
  }

  public release() {
    nativePlayer.release(this.playerHandel);
    this.playerHandel = 0;
  }

  public setBufferUpdateListener(l: (position: number) => void) {
    nativePlayer.setBufferUpdateListener(this.playerHandel, l);
  }

  public seek(position: number, accurate: boolean) {
    nativePlayer.seek(this.playerHandel, position, accurate);
  }

  public setCompleteListener(l: () => void) {
    nativePlayer.setCompleteListener(this.playerHandel, l);
  }

  public setOffsetScreenSize(width: number, height: number) {
    nativePlayer.setOffsetScreenSize(this.playerHandel, width, height);
  }

  public setFboTextureListener(l: (textureId: number) => void) {
    nativePlayer.setFboTextureListener(this.playerHandel, l);
  }

  public getCurrentPosition(): number {
    return nativePlayer.getCurrentPosition(this.playerHandel);
  }

  public getVideoWidth(): number {
    return nativePlayer.getVideoWidth(this.playerHandel);
  }

  public getVideoHeight(): number {
    return nativePlayer.getVideoHeight(this.playerHandel);
  }

  public setAutoPlayListener(l: () => void) {
    nativePlayer.setAutoPlayListener(this.playerHandel, l);
  }

  public setSeekCompleteListener(l: () => void) {
    nativePlayer.setSeekCompleteListener(this.playerHandel, l);
  }

  public setRenderStartListener(l: () => void) {
    nativePlayer.setRenderStartListener(this.playerHandel, l);
  }

  public setLoop(loop: boolean) {
    nativePlayer.setLoop(this.playerHandel, loop);
  }

  public isLoop(): boolean {
    let l = nativePlayer.isLoop(this.playerHandel);
    return l != 0;
  }

  public isAutoPlay(): boolean {
    let l = nativePlayer.isAutoPlay(this.playerHandel);
    return l != 0;
  }

  public setMute(mute: boolean) {
    nativePlayer.setMute(this.playerHandel, mute);
  }

  public isMute(): boolean {
    let l = nativePlayer.isMute(this.playerHandel);
    return l != 0;
  }

  public setVolume(volume: number) {
    nativePlayer.setVolume(this.playerHandel, volume);
  }

  public getVolume(): number {
    return nativePlayer.getVolume(this.playerHandel);
  }

  public setSpeed(speed: number) {
    nativePlayer.setSpeed(this.playerHandel, speed);
  }

  public hasAudio(): boolean {
    let l = nativePlayer.hasAudio(this.playerHandel);
    return l != 0;
  }

  public setEnableLog(enable: boolean) {
    nativePlayer.enableLog(this.playerHandel, enable);
  }

  public setLogInfoListener(l: (level: number, msg: string, ptr: number, sourceMd5: string) => void) {
    nativePlayer.setLogInfoListener(this.playerHandel, l);
  };
}