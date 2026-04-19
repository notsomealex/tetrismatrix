#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "libs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

enum INPUT
{
    KEY_LEFT = 1 << 0,
    KEY_RIGHT = 1 << 1,
    KEY_UP = 1 << 2,
    KEY_DOWN = 1 << 3,
    KEY_A = 1 << 4,
    KEY_B = 1 << 5,
    KEY_X = 1 << 6,
    KEY_Y = 1 << 7,
    KEY_L = 1 << 8,
    KEY_R = 1 << 9,
    KEY_START = 1 << 10,
    KEY_SELECT = 1 << 11
};


namespace InputManager
{
    extern int controllers_connected;
    extern bool connecting_controller;
    extern bool pairing_enabled;
    extern bool new_controller_found;
    extern std::vector<uni_hid_device_t*> devices;
    extern std::vector<uni_controller_t*> controllers;
    extern std::unordered_map<uni_controller_t*, int16_t> keys_held;
    extern std::unordered_map<uni_controller_t*, int16_t> keys_pressed;


    bool init();
    void searchForControllers();
    void enablePairing();
    void disablePairing();
    void onDeviceConnected(uni_hid_device_t* device);
    void onDeviceDisconnected(uni_hid_device_t* device);
    int16_t getKeysHeld(uni_controller_t* controller);
    int16_t getKeysPressed(uni_controller_t* controller);
    void scanKeys();
    int16_t convertRawInput(uni_controller_t* controller);
    //void onControllerData(uni_controller_t* controller);
}

#endif