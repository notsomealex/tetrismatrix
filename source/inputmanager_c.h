#pragma once

#include <uni.h>
#ifdef __cplusplus
extern "C" {
#endif

void InputManager_onDeviceConnected(uni_hid_device_t* device);
void InputManager_onDeviceDisconnected(uni_hid_device_t* device);
//void InputManager_onControllerData(uni_controller_t* controller);

#ifdef __cplusplus
}
#endif