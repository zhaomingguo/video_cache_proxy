import { MediaCacheService } from "videocache/src/main/ets/native/MediaCacheService"
import { CicadaPlayer } from '@arch/cicadaplayer';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { common } from '@kit.AbilityKit';
import { media } from '@kit.MediaKit';
import { BusinessError } from '@kit.BasicServicesKit';


@Entry
@Component
struct Index {
  @State message: string = 'Hello World';
  @State poxyUrl: string = '';
  @State showCicadaPlayer: boolean = false;
  @State showSystemPlayer: boolean = false;
  mXComponentController: XComponentController = new XComponentController();
  cicadaPlayer: CicadaPlayer = new CicadaPlayer();
  avPlayer: media.AVPlayer | null = null;
  private context = getContext(this) as common.UIAbilityContext
  cacheRootDirPath = this.context.filesDir

  build() {
    RelativeContainer() {
      Column({ space: 20 }) {
        Row({ space: 20 }) {
          Button('开启缓存服务', { type: ButtonType.Normal, stateEffect: true })
            .onClick(() => {
              MediaCacheService.getInstance()
                .setCacheConfig({
                  cacheRootDir: this.cacheRootDirPath, // 缓存目录
                  policy: 1, // 缓存策略 默认1=按创建时间淘汰
                  maxCacheCapacity: 1024 * 1024 * 512, // 512MB
                  upstreamTimeoutSeconds: 3 // 请求网络资源连接超时，单位：秒
                })
              // // 2. Start Video Cache
              MediaCacheService.getInstance().startCacheProxy()
            })

          Button('清缓存', { type: ButtonType.Normal, stateEffect: true })
            .onClick(() => {
              MediaCacheService.getInstance().cleanCache()
            })
          Button('kill缓存服务', { type: ButtonType.Normal, stateEffect: true })
            .onClick(() => {
              MediaCacheService.getInstance().stopCacheProxy()
            })
        }

        Row({ space: 20 }) {
          Button('播放-Cicada播放器', { type: ButtonType.Normal, stateEffect: true })
            .onClick(() => {
              this.getProxyUrl();
              this.createCicadaPlayer();
              this.showCicadaPlayer = true;
              this.showSystemPlayer = false;
            })
          Button('播放-系统播放器', { type: ButtonType.Normal, stateEffect: true })
            .onClick(() => {
              this.getProxyUrl();
              this.showCicadaPlayer = false;
              this.showSystemPlayer = true;
            })
        }

        // Cicada播放器
        if (this.showCicadaPlayer && this.poxyUrl != "") {
          XComponent({
            id: "surfaceId",
            type: "surface",
            controller: this.mXComponentController
          })
            .onLoad(() => {
              if (this.cicadaPlayer != null) {
                this.cicadaPlayer.setSurfaceId(this.mXComponentController.getXComponentSurfaceId());
                hilog.info(0x0000, 'hm-media-arkts',
                  'XComponent surfaceId is ' + this.mXComponentController.getXComponentSurfaceId());
              }
            })
            .width("100%")
            .aspectRatio(1.77)
        }

        // 系统播放器
        if (this.showSystemPlayer && this.poxyUrl != "") {
          XComponent({
            id: "surfaceId",
            type: "surface",
            controller: this.mXComponentController
          })
            .onLoad(() => {
              let surfaceId = this.mXComponentController.getXComponentSurfaceId();
              this.createAVPlayer(surfaceId);
              hilog.info(0x0000, 'hm-media-arkts', 'XComponent surfaceId is ' + surfaceId)
            })
            .width("100%")
            .aspectRatio(1.77)
        }
      }
    }
    .height('100%')
    .width('100%')
  }

  getProxyUrl() {
    // 3. Get Video Cache URL
    // https://jojopublicfat.jojoread.com/cms/jaguar-admin/front-resource/700007225185083393.mp4
    // https://media.w3.org/2010/05/sintel/trailer.mp4
    // https://qcspublicfat.tinman.cn/vod/paired-reading/videoconcat/2673267179083314279.mp4?ossUrl=tinman-oss%3A%2F%2Fbs-media-service-vod%2Fpaired-reading%2Fvideoconcat%2F2673267179083314279.mp4
    this.poxyUrl = MediaCacheService.getInstance()
      .getProxyUrl('https://qcspublicfat.tinman.cn/vod/paired-reading/videoconcat/2673267179083314279.mp4?ossUrl=tinman-oss%3A%2F%2Fbs-media-service-vod%2Fpaired-reading%2Fvideoconcat%2F2673267179083314279.mp4');
    console.info("poxy:" + this.poxyUrl);
  }

  createCicadaPlayer() {
    this.cicadaPlayer.setPositionUpdateListener((positon: number) => {
      hilog.info(0x0000, 'hm-media-arkts', 'setPositionUpdateListener position ' + (positon / 1000.0) + ' ' +
        (this.cicadaPlayer.getDuration() / 1000.0));
    });
    this.cicadaPlayer.setLoadStartListener(() => {
      hilog.info(0x0000, 'hm-media-arkts', 'setLoadStartListener call');
    });
    this.cicadaPlayer.setLoadProgressListener((progress: number) => {
      hilog.info(0x0000, 'hm-media-arkts', 'setLoadProgressListener ' + progress);
    });
    this.cicadaPlayer.setLoadEndListener(() => {
      hilog.info(0x0000, 'hm-media', 'setLoadEndListener call');
    });
    this.cicadaPlayer.setErrorListener((errorCode: number, errorMsg: string) => {
      hilog.info(0x0000, 'hm-media', 'setErrorListener ' + errorCode + " " + errorMsg);
    });
    this.cicadaPlayer.setBufferUpdateListener((positon: number) => {
      hilog.info(0x0000, 'hm-media-arkts', 'setBufferUpdateListener position ' + (positon / 1000.0) + ' ' +
        (this.cicadaPlayer.getDuration() / 1000.0));
    });
    this.cicadaPlayer.setCompleteListener(() => {
      hilog.info(0x0000, 'hm-media', 'setCompleteListener call');
    });
    this.cicadaPlayer.setFboTextureListener((textureId: number) => {
      hilog.info(0x0000, 'hm-media', 'setFboTextureListener call textureId = ' + textureId);
    });
    this.cicadaPlayer.setLogInfoListener((level: number, msg: string, ptr: number, sourceMd5: string) => {
      hilog.info(0x0000, 'hm-media',
        'setLogInfoListener call ' + level + " " + msg + " " + ptr + " " + sourceMd5);
    });
    this.cicadaPlayer.setDataSource(this.poxyUrl);
    // this.cicadaPlayer.setDataSource("https://jojopublicfat.jojoread.com/cms/jaguar-admin/front-resource/700007225185083393.mp4")
    this.cicadaPlayer.setAutoPlay(true);
    this.cicadaPlayer.prepare();
  }

  async createAVPlayer(surfaceId?: string) {
    if (this.avPlayer == null) {
      this.avPlayer = await media.createAVPlayer();
    }
    if (this.avPlayer != null) {
      this.avPlayer.url = this.poxyUrl;
      // this.avPlayer.url = "http://10.131.67.175:8190/mp4/8401b07620398515cc7e6e6a83635da9/req.mp4?url=https://jojopublicfat.jojoread.com/cms/jaguar-admin/front-resource/700007225185083393.mp4";
      // this.avPlayer.url = "https://jojopublicfat.jojoread.com/cms/jaguar-admin/front-resource/700007225185083393.mp4";

      this.avPlayer.on("stateChange", async (state: media.AVPlayerState) => {
        if (this.avPlayer == null) {
          return;
        }
        switch (state) {
          case "idle":
            this.avPlayer.url = this.poxyUrl;
            break;
          case "initialized":
            this.avPlayer.surfaceId = surfaceId;
            this.avPlayer.prepare();
            break;
          case "prepared":
            this.avPlayer.play();
            break;
        }
      })
      this.avPlayer.on("error", (err: BusinessError) => {
        // 播放直播视频时，设置 loop 会报错，而 loop 一定会设置(video_player.dart 中初始化之后会 _applyLooping)，所以屏蔽掉该报错
        // message: Unsupport Capability: The stream is live stream, not support loop
        if (err.code == 801) {
          return;
        }
        this.avPlayer?.reset();
      })
    }

  }
}