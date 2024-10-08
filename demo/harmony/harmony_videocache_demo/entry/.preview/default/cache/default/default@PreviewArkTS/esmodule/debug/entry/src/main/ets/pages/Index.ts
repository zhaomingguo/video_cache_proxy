if (!("finalizeConstruction" in ViewPU.prototype)) {
    Reflect.set(ViewPU.prototype, "finalizeConstruction", () => { });
}
interface Index_Params {
    message?: string;
}
import hilog from "@ohos:hilog";
import testNapi from "@app:com.example.harmony_videocache_demo/entry/entry";
import { VideoCacheApi } from "@bundle:com.example.harmony_videocache_demo/video_cache/Index";
import type { video_cache_config } from "@bundle:com.example.harmony_videocache_demo/video_cache/cpp/types/libvideocache/index.d";
class Index extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === "function") {
            this.paramsGenerator_ = paramsLambda;
        }
        this.__message = new ObservedPropertySimplePU('Hello World', this, "message");
        this.setInitiallyProvidedValue(params);
        this.finalizeConstruction();
    }
    setInitiallyProvidedValue(params: Index_Params) {
        if (params.message !== undefined) {
            this.message = params.message;
        }
    }
    updateStateVars(params: Index_Params) {
    }
    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__message.purgeDependencyOnElmtId(rmElmtId);
    }
    aboutToBeDeleted() {
        this.__message.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }
    private __message: ObservedPropertySimplePU<string>;
    get message() {
        return this.__message.get();
    }
    set message(newValue: string) {
        this.__message.set(newValue);
    }
    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
            Row.debugLine("entry/src/main/ets/pages/Index.ets(14:5)", "entry");
            Row.height('100%');
        }, Row);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.debugLine("entry/src/main/ets/pages/Index.ets(15:7)", "entry");
            Column.width('100%');
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create(this.message);
            Text.debugLine("entry/src/main/ets/pages/Index.ets(16:9)", "entry");
            Text.fontSize(50);
            Text.fontWeight(FontWeight.Bold);
            Text.onClick(() => {
                const videoCacheApi = new VideoCacheApi();
                const r = testNapi.add(2, 3);
                const config: video_cache_config = {
                    cache_root_dir: '/path/to/cache',
                    cache_root_dir_len: 16,
                    policy: 1,
                    max_cache_capacity: BigInt(1024),
                    upstream_timeout_seconds: 3,
                };
                videoCacheApi.setCacheConfig(config);
                videoCacheApi.startCacheProxy();
                const outputString: string[] = [''];
                videoCacheApi.getProxyUrl("https://player.alicdn.com/video/aliyunmedia.mp4", outputString);
                videoCacheApi.stopCacheProxy();
                hilog.info(0x0000, 'testTag', 'Test NAPI 2 + 3 = %{public}d', testNapi.add(2, 3));
            });
        }, Text);
        Text.pop();
        Column.pop();
        Row.pop();
    }
    rerender() {
        this.updateDirtyElements();
    }
    static getEntryName(): string {
        return "Index";
    }
}
registerNamedRoute(() => new Index(undefined, {}), "", { bundleName: "com.example.harmony_videocache_demo", moduleName: "entry", pagePath: "pages/Index", pageFullPath: "entry/src/main/ets/pages/Index", integratedHsp: "false" });
