/*
 * Copyright (C) 2019 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "vendor.lineage.livedisplay@2.1-service-lge_sdm845"

#include <android-base/logging.h>
#include <binder/ProcessState.h>
#include <hidl/HidlTransportSupport.h>
#include <livedisplay/sdm/PictureAdjustment.h>

#include "DisplayModes.h"
#include "SunlightEnhancement.h"

using android::sp;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

using ::vendor::lineage::livedisplay::V2_0::sdm::PictureAdjustment;
using ::vendor::lineage::livedisplay::V2_0::sdm::SDMController;
using ::vendor::lineage::livedisplay::V2_1::IDisplayModes;
using ::vendor::lineage::livedisplay::V2_1::ISunlightEnhancement;
using ::vendor::lineage::livedisplay::V2_1::implementation::DisplayModes;
using ::vendor::lineage::livedisplay::V2_1::implementation::SunlightEnhancement;

int main() {
    // HIDL frontend
    sp<DisplayModes> dm;
    sp<PictureAdjustment> pa;
    sp<SunlightEnhancement> se;

    std::shared_ptr<SDMController> controller = std::make_shared<SDMController>();

    android::ProcessState::initWithDriver("/dev/vndbinder");

    LOG(INFO) << "LiveDisplay HAL service is starting.";

    dm = new DisplayModes();
    if (dm == nullptr) {
        LOG(ERROR) << "Can not create an instance of LiveDisplay HAL DisplayModes Iface,"
                   << " exiting.";
        goto shutdown;
    }

    pa = new PictureAdjustment(controller);
    if (pa == nullptr) {
        LOG(ERROR) << "Can not create an instance of LiveDisplay HAL PictureAdjustment Iface,"
                   << " exiting.";
        goto shutdown;
    }

    se = new SunlightEnhancement();
    if (se == nullptr) {
        LOG(ERROR) << "Can not create an instance of LiveDisplay HAL SunlightEnhancement Iface,"
                   << " exiting.";
        goto shutdown;
    }

    configureRpcThreadpool(1, true /*callerWillJoin*/);

    if (dm->registerAsService() != android::OK) {
        LOG(ERROR) << "Cannot register DisplayModes HAL service.";
        goto shutdown;
    }

    if (pa->registerAsService() != android::OK) {
        LOG(ERROR) << "Cannot register PictureAdjustment HAL service.";
        goto shutdown;
    }

    if (se->registerAsService() != android::OK) {
        LOG(ERROR) << "Cannot register SunlightEnhancement HAL service.";
        goto shutdown;
    }

    LOG(INFO) << "LiveDisplay HAL service is ready.";
    joinRpcThreadpool();
    // Should not pass this line

shutdown:
    // In normal operation, we don't expect the thread pool to shutdown
    LOG(ERROR) << "LiveDisplay HAL service is shutting down.";
    return 1;
}
