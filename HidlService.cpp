#define LOG_TAG "hwservicemanager"
#include "HidlService.h"

#include <android-base/logging.h>
#include <sstream>

namespace android {
namespace hidl {
namespace manager {
namespace V1_0 {
namespace implementation {

HidlService::HidlService(
    const std::string &interfaceName,
    const std::string &instanceName,
    const sp<IBase> &service)
: mInterfaceName(interfaceName),
  mInstanceName(instanceName),
  mService(service)
{}

sp<IBase> HidlService::getService() const {
    return mService;
}
void HidlService::setService(sp<IBase> service) {
    mService = service;

    sendRegistrationNotifications();
}
const std::string &HidlService::getInterfaceName() const {
    return mInterfaceName;
}
const std::string &HidlService::getInstanceName() const {
    return mInstanceName;
}

void HidlService::addListener(const sp<IServiceNotification> &listener) {
    mListeners.push_back(listener);

    if (mService != nullptr) {
        listener->onRegistration(mInterfaceName, mInstanceName, true /* preexisting */);
    }
}

std::string HidlService::string() const {
    std::stringstream ss;
    ss << mInterfaceName << "/" << mInstanceName;
    return ss.str();
}

void HidlService::sendRegistrationNotifications() const {
    if (mListeners.size() == 0 || mService == nullptr) {
        return;
    }

    hidl_string iface = mInterfaceName;
    hidl_string name = mInstanceName;

    for (const auto &listener : mListeners) {
        auto ret = listener->onRegistration(iface, name, false /* preexisting */);
        ret.isOk(); // ignore result
    }
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace manager
}  // namespace hidl
}  // namespace android