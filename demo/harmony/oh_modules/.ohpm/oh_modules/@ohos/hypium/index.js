/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import Core from './src/main/core';
import { DEFAULT, TestType, Size, Level, TAG } from './src/main/Constant';
import DataDriver from './src/main/module/config/DataDriver';
import ExpectExtend from './src/main/module/assert/ExpectExtend';
import OhReport from './src/main/module/report/OhReport';
import SysTestKit from './src/main/module/kit/SysTestKit';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, beforeItSpecified, afterItSpecified, xdescribe, xit } from './src/main/interface';
import { MockKit, when } from './src/main/module/mock/MockKit';
import ArgumentMatchers from './src/main/module/mock/ArgumentMatchers';

class Hypium {
    static context = new Map();
    static setData(data) {
        const core = Core.getInstance();
        const dataDriver = new DataDriver({ data });
        core.addService('dataDriver', dataDriver);
    }

    static setTimeConfig(systemTime) {
        SysTestKit.systemTime = systemTime;
    }
	
    static set(key, value) {
        Hypium.context.set(key, value);
    }

    static get(key) {
        return Hypium.context.get(key);
    }

    static hypiumTest(abilityDelegator, abilityDelegatorArguments, testsuite) {
        const core = Core.getInstance();
        const expectExtend = new ExpectExtend({
            'id': 'extend'
        });
        core.addService('expect', expectExtend);
        const ohReport = new OhReport({
            'delegator': abilityDelegator,
            'abilityDelegatorArguments': abilityDelegatorArguments
        });
        SysTestKit.delegator = abilityDelegator;
        core.addService('report', ohReport);
        core.init();
        core.subscribeEvent('spec', ohReport);
        core.subscribeEvent('suite', ohReport);
        core.subscribeEvent('task', ohReport);
        const configService = core.getDefaultService('config');
        if (abilityDelegatorArguments !== null) {
            let testParameters = configService.translateParams(abilityDelegatorArguments.parameters);
            console.info(`${TAG}parameters:${JSON.stringify(testParameters)}`);
            configService.setConfig(testParameters);
        }
        testsuite();
        core.execute(abilityDelegator);
    }

    static registerAssert(customAssertion) {
        const core = Core.getInstance();
        const expectService = core.getDefaultService('expect');
        let matchers = {};
        matchers[customAssertion.name] = customAssertion;
        expectService.addMatchers(matchers);
        expectService.customMatchers.push(customAssertion.name);
        console.log(`${TAG}success to register the ${customAssertion.name}`);
    }

    static unregisterAssert(customAssertion) {
        const core = Core.getInstance();
        const expectService = core.getDefaultService('expect');
        let customAssertionName = typeof customAssertion === 'function' ? customAssertion.name : customAssertion;
        expectService.removeMatchers(customAssertionName);
        console.log(`${TAG}success to unregister the ${customAssertionName}`);
    }

}

export {
    Hypium,
    Core,
    DEFAULT,
    TestType,
    Size,
    Level,
    DataDriver,
    ExpectExtend,
    OhReport,
    SysTestKit,
    describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, beforeItSpecified, afterItSpecified, xdescribe, xit,
    MockKit, when,
    ArgumentMatchers
};