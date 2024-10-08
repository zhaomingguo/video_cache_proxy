/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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

import SysTestKit from "../kit/SysTestKit";
import { collectCoverageData } from '../coverage/coverageCollect';
import { TAG } from '../../Constant';

class OhReport {
  constructor(attr) {
    this.delegator = attr.delegator;
    this.abilityDelegatorArguments = attr.abilityDelegatorArguments;
    this.id = 'report';
    this.index = 0;
    this.duration = 0;
  }

  init(coreContext) {
    this.coreContext = coreContext;
    this.suiteService = this.coreContext.getDefaultService('suite');
    this.specService = this.coreContext.getDefaultService('spec');
  }

  taskStart() {
  }

  async taskDone() {
    if (this.abilityDelegatorArguments !== null) {
      this.taskDoneTime = new Date().getTime();
      let summary = this.suiteService.getSummary();
      const configService = this.coreContext.getDefaultService('config');
      const suiteService = this.coreContext.getDefaultService('suite');
      const specService = this.coreContext.getDefaultService('spec');
      if (configService['coverage'] === 'true') {
        await collectCoverageData();
      }
      let message = '\n' + 'OHOS_REPORT_RESULT: stream=Tests run: ' + summary.total + ', Failure: ' + summary.failure;
      message += ', Error: ' + summary.error;
      message += ', Pass: ' + summary.pass;
      message += ', Ignore: ' + summary.ignore;
      if (specService.skipSpecNum > 0) {
        message += ', SkipSpec: ' + specService.skipSpecNum;
      }
      message += '\n' + 'OHOS_REPORT_CODE: ' + (summary.failure > 0 ? -1 : 0) + '\n';
      let isHasError = summary.failure > 0 || summary.error > 0;
      let config = this.coreContext.getDefaultService('config');
      if (config.isBreakOnError() && isHasError) {
        // 未执行全部说明
        message += '\n' + 'OHOS_REPORT_RESULT: breakOnError model, Stopping whole test suite if one specific test case failed or error' + '\n';
      }
      message += 'OHOS_REPORT_STATUS: taskconsuming=' + summary.duration + '\n';
      console.info(`${message}`);
      await SysTestKit.print(message);
    }
    console.info(`${TAG}report print success`);
    this.delegator.finishTest('your test finished!!!', 0, () => { });
  }

  incorrectFormat() {
    if (this.coreContext.getDefaultService('config').filterValid.length !== 0) {
      var value = this.coreContext.getDefaultService('config').filterValid;
      var message = 'this param ' + value.join(',') + ' is invalid' + '\n';
      this.delegator.finishTest(message, 0, () => {
      });
    }
  }

  incorrectTestSuiteFormat() {
    if (this.coreContext.getDefaultService('config').filterXdescribe.length !== 0) {
      let value = this.coreContext.getDefaultService('config').filterXdescribe;
      let message = 'xdescribe ' + value.join(',') + ' should not contain it' + '\n';
      this.delegator.finishTest(message, 0, () => {
      });
    }
  }
  async suiteStart() {
    if (this.abilityDelegatorArguments !== null) {
      let specArr = [];
      this.suiteService.getAllChildSuiteNum(this.suiteService.getCurrentRunningSuite(), specArr);
      let message = '\n' + 'OHOS_REPORT_SUM: ' + specArr.length;
      this.suiteService.setCurrentRunningSuiteDesc(this.suiteService.getRootSuite(), this.suiteService.getCurrentRunningSuite(), '');
      message += '\n' + 'OHOS_REPORT_STATUS: class=' + this.suiteService.getCurrentRunningSuiteDesc() + '\n';
      if (this.suiteService.currentRunningSuite.isSkip) {
        message += 'OHOS_REPORT_STATUS: skipReason=' + this.suiteService.currentRunningSuite.skipReason + '\n';
      }
      console.info(`${message}`);
      await SysTestKit.print(message);
      console.info(`${TAG}${this.suiteService.getCurrentRunningSuite().description} suiteStart print success`);
    }
  }

  async suiteDone() {
    if (this.abilityDelegatorArguments !== null) {
      const currentRunningSuite = this.suiteService.getCurrentRunningSuite();
      this.suiteService.setCurrentRunningSuiteDesc(this.suiteService.getRootSuite(), this.suiteService.getCurrentRunningSuite(), '');
      let message = '\n' + 'OHOS_REPORT_STATUS: class=' + this.suiteService.getCurrentRunningSuiteDesc();
      if (this.suiteService.currentRunningSuite.isSkip && this.suiteService.currentRunningSuite.skipReason !== '') {
        message += '\n' + 'OHOS_REPORT_STATUS: skipReason=' + this.suiteService.currentRunningSuite.skipReason;
      }
      message += '\n' + 'OHOS_REPORT_STATUS: suiteconsuming=' + this.suiteService.getCurrentRunningSuite().duration;
      if (currentRunningSuite.hookError) {
        message += '\n' + `OHOS_REPORT_STATUS: ${currentRunningSuite.hookError.message}`;
      }
      message += '\n';
      console.info(`${message}`);
      await SysTestKit.print(message);
      console.info(`${TAG}${this.suiteService.getCurrentRunningSuite().description} suiteDone print success`);
    }
  }

  async specStart() {
    if (this.abilityDelegatorArguments !== null) {
      let message = '\n' + 'OHOS_REPORT_STATUS: class=' + this.suiteService.getCurrentRunningSuiteDesc();
      message += '\n' + 'OHOS_REPORT_STATUS: current=' + (++this.index);
      message += '\n' + 'OHOS_REPORT_STATUS: id=JS';
      message += '\n' + 'OHOS_REPORT_STATUS: numtests=' + this.specService.getTestTotal();
      message += '\n' + 'OHOS_REPORT_STATUS: stream=';
      message += '\n' + 'OHOS_REPORT_STATUS: test=' + this.specService.currentRunningSpec.description;
      message += '\n' + 'OHOS_REPORT_STATUS_CODE: 1' + '\n';
      if (this.specService.currentRunningSpec.isSkip) {
        message += 'OHOS_REPORT_STATUS: skipReason=' + this.specService.currentRunningSpec.skipReason + '\n';
      }
      console.info(`${message}`);
      await SysTestKit.print(message);
      console.info(`${TAG}${this.specService.currentRunningSpec.description} specStart start print success`);
    }
  }

  async specDone() {
    if (this.abilityDelegatorArguments !== null) {
      let message = '\n' + 'OHOS_REPORT_STATUS: class=' + this.suiteService.getCurrentRunningSuiteDesc();
      message += '\n' + 'OHOS_REPORT_STATUS: current=' + (this.index);
      message += '\n' + 'OHOS_REPORT_STATUS: id=JS';
      message += '\n' + 'OHOS_REPORT_STATUS: numtests=' + this.specService.getTestTotal();
      let messageStack = '';
      let messageCode = '';
      if (this.specService.currentRunningSpec.error) {
        messageStack = 'OHOS_REPORT_STATUS: stack=' + this.specService.currentRunningSpec.error?.stack?.slice(0, -1);
        messageCode += 'OHOS_REPORT_STATUS: stream=';
        messageCode += this.specService.currentRunningSpec.expectMsg !== '' ?
          `message: ${this.specService.currentRunningSpec.expectMsg}, Error in ${this.specService.currentRunningSpec.description}, ${this.specService.currentRunningSpec.error?.message}` :
          `Error in ${this.specService.currentRunningSpec.description}, ${this.specService.currentRunningSpec.error?.message}`;
        messageCode += '\n' + 'OHOS_REPORT_STATUS: test=' + this.specService.currentRunningSpec.description;
        messageCode += '\n' + 'OHOS_REPORT_STATUS_CODE: -1' + '\n';
      } else if (this.specService.currentRunningSpec) {
        if (this.specService.currentRunningSpec.fail) {
          messageStack += 'OHOS_REPORT_STATUS: stack=' + this.specService.currentRunningSpec.fail?.stack?.slice(0, -1);
          messageCode += 'OHOS_REPORT_STATUS: stream=';
          messageCode += this.specService.currentRunningSpec.expectMsg !== '' ?
            `message: ${this.specService.currentRunningSpec.expectMsg}, Error in ${this.specService.currentRunningSpec.description}, ${this.specService.currentRunningSpec.fail?.message}` :
            `Error in ${this.specService.currentRunningSpec.description}, ${this.specService.currentRunningSpec.fail?.message}`;
          messageCode += '\n' + 'OHOS_REPORT_STATUS: test=' + this.specService.currentRunningSpec.description;
          messageCode += '\n' + 'OHOS_REPORT_STATUS_CODE: -2' + '\n';
        } else {
          messageStack += 'OHOS_REPORT_STATUS: stream=';
          messageCode += 'OHOS_REPORT_STATUS: test=' + this.specService.currentRunningSpec.description;
          messageCode += '\n' + 'OHOS_REPORT_STATUS_CODE: 0' + '\n';
          messageCode += this.specService.currentRunningSpec.isSkip ?  ('OHOS_REPORT_STATUS: skipReason=' + this.specService.currentRunningSpec.skipReason + '\n') : '';
        }
      } else {
        messageCode += '\n';
      }
      messageCode += 'OHOS_REPORT_STATUS: consuming=' + this.specService.currentRunningSpec.duration + '\n';
      console.info(`${message}`);
      console.info(`\n${messageStack}`);
      console.info(`\n${messageCode}`);
      await SysTestKit.print(message);
      await SysTestKit.print(messageStack);
      await SysTestKit.print(messageCode);
      console.info(`${TAG}${this.specService.currentRunningSpec.description} specDone end print success`);
    }
  }
}

export default OhReport;
