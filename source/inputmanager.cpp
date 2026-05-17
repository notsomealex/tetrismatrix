#include "inputmanager.h"
#include "game.h"
#include "helpers.h"
#include "language.h"
#include "inputmanager_c.h"

#ifdef __cplusplus
extern "C" {
#endif
// Defined in my_platform.c
struct uni_platform* get_my_platform(void);

// C api

void InputManager_onDeviceConnected(uni_hid_device_t* device)
{
    InputManager::onDeviceConnected(device);
}

void InputManager_onDeviceDisconnected(uni_hid_device_t* device)
{
    InputManager::onDeviceDisconnected(device);
}

#ifdef __cplusplus
}
#endif

// callback for core 1. basically for bluetooth to work in parallel to the rest of the game.
void core1_program()
{
    btstack_run_loop_execute();
}

bool InputManager::connecting_controller = false;
bool InputManager::pairing_enabled = false;
bool InputManager::new_controller_found = false;
std::vector<uni_hid_device_t*> InputManager::devices;
std::vector<uni_controller_t*> InputManager::controllers;
std::unordered_map<uni_controller_t*, int16_t> InputManager::keys_held;
std::unordered_map<uni_controller_t*, int16_t> InputManager::keys_pressed;

bool InputManager::init()
{
    if (cyw43_arch_init()) {
        printf("failed to initialise cyw43_arch\n");
        return false;
    }

        // Must be called before uni_init()
    uni_platform_set_custom(get_my_platform());

    // Initialize BP32
    if(uni_init(0, NULL))
    {
        printf("uni_init failed!\n\n");
        return false;
    }
    multicore_launch_core1(core1_program);

    return true;
}

// will pause the whole game to search for them.\
// TODO: OK. so for now the only way for this thing to be able to pair to controllers is to just not touch the enable/disable api, as it is enabled by default.
// if i enable it manually, it doesnt pair anymore, which sucks but eh i must move on to other stuff.
void InputManager::searchForControllers()
{
    uint32_t time = to_ms_since_boot(get_absolute_time());

    if(!pairing_enabled)
       enablePairing();

    //printf("pairing enabled: %i", pairing_enabled);

    static std::string dots = "...";
    int x = tetrismatrix::vertical ? SCREEN_HEIGHT / 2 : SCREEN_WIDTH / 2;
    int y = tetrismatrix::vertical ? SCREEN_WIDTH / 2 : SCREEN_HEIGHT / 2;

    Language current_language = (Language)std::get<EnumOption>(tetrismatrix::settings.current_language).value;

    int limit = tetrismatrix::vertical ? SCREEN_HEIGHT : SCREEN_WIDTH;
    std::string searching = translate(current_language, Texts::SEARCHING);
    cropTextToFit(searching, 3, limit);
    draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, searching, x - (searching.size() * 4 / 2), y, 255, 255 ,255, tetrismatrix::vertical);
    
    std::string controllers = translate(current_language, Texts::CONTROLLERS);
    cropTextToFit(controllers, 3, limit - 12);
    draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, controllers, x - (controllers.size() * 4 / 2), y + 6, 255, 255 ,255, tetrismatrix::vertical);

    draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, dots, x, y + 12, 255, 255 ,255, tetrismatrix::vertical);

    // update the dots animation every half a second
    if(time % 500 == 0)
    {
        tetrismatrix::hub75.clear();
        if(dots == "...")
            dots = "";
        dots += '.';
    }
    if(new_controller_found)
    {
        tetrismatrix::hub75.clear();
        std::string controller = translate(current_language, Texts::CONTROLLER);
        cropTextToFit(controller, 4, limit);
        draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, controller, x - (controller.size() * 4 / 2), y, 255, 255 ,255, tetrismatrix::vertical);

        std::string connected = translate(current_language, Texts::CONNECTED);
        cropTextToFit(connected, 4, limit);
        draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, connected, x - (connected.size() * 4 / 2), y + 5, 255, 255 ,255, tetrismatrix::vertical);

        new_controller_found = false;
        sleep_ms(3000);
        disablePairing();
        tetrismatrix::hub75.clear();
    }
}

void InputManager::enablePairing()
{
    uni_bt_enable_new_connections_safe(true);
    pairing_enabled = true;
}

void InputManager::disablePairing()
{
    uni_bt_enable_new_connections_safe(false);
    pairing_enabled = false;
}

void InputManager::onDeviceConnected(uni_hid_device_t* device)
{
    devices.push_back(device);

    if(device->controller_type != -1)
    {
        printf("controller connected!");
        controllers.push_back(&device->controller);
        new_controller_found = true;
    }
    else
    {
        printf("not a controller!\n\n");
        printf("klass: %i", device->controller.klass);
        printf("controller type: %i", device->controller_type);
    }
}

void InputManager::onDeviceDisconnected(uni_hid_device_t* device)
{
    std::erase(devices, device);

    if(device->controller.klass & UNI_CONTROLLER_CLASS_GAMEPAD)
    {
        std::erase(controllers, &device->controller);
    }
}


// THESE THREE FUNCTIONS BELOW will return a bitmask corresponding to the input values defined in inputmanager.h, NOT the ones from bluepad32.
// this so that, for example, the stick input and the dpad is the same and yeah dont judge me

// will scan keys for all controllers.
void InputManager::scanKeys()
{
    for(auto& controller : controllers)
    {
        keys_held[controller] = getKeysHeld(controller);
        keys_pressed[controller] = getKeysPressed(controller);
    }
}

// DO NOT CALL THESE TWO FUNCTIONS BELOW. just use scanKeys() once and the unordered_maps will update.
int16_t InputManager::getKeysHeld(uni_controller_t* controller)
{
    int16_t keys_held = convertRawInput(controller);

    return keys_held;
}
int16_t InputManager::getKeysPressed(uni_controller_t* controller)
{
    static std::unordered_map<uni_controller_t*, uint16_t> previous_input;
    int16_t current_input = convertRawInput(controller);
    
    int16_t keys_pressed = 0;

    keys_pressed = current_input & ~previous_input[controller];
    previous_input[controller] = current_input;

    return keys_pressed;
}

int16_t InputManager::convertRawInput(uni_controller_t* controller)
{

    int16_t input = 0;

    if(controller->gamepad.buttons & BUTTON_A)
        input |= KEY_A;

    if(controller->gamepad.buttons & BUTTON_B)
        input |= KEY_B;

    if(controller->gamepad.buttons & BUTTON_X)
        input |= KEY_X;

    if(controller->gamepad.buttons & BUTTON_Y)
        input |= KEY_Y;

    if(controller->gamepad.buttons & BUTTON_SHOULDER_L || controller->gamepad.brake >= 512)
        input |= KEY_L;

    if(controller->gamepad.buttons & BUTTON_SHOULDER_R || controller->gamepad.throttle >= 512)
        input |= KEY_R;

    if(controller->gamepad.misc_buttons & MISC_BUTTON_START)
        input |= KEY_START;

    if(controller->gamepad.misc_buttons & MISC_BUTTON_SELECT)
        input |= KEY_SELECT;

    if(controller->gamepad.dpad & DPAD_UP || controller->gamepad.axis_y <= -256)
        input |= KEY_UP;

    if(controller->gamepad.dpad & DPAD_DOWN || controller->gamepad.axis_y >= 256)
        input |= KEY_DOWN;
    
    if(controller->gamepad.dpad & DPAD_LEFT || controller->gamepad.axis_x <= -256)
        input |= KEY_LEFT;
    
    if(controller->gamepad.dpad & DPAD_RIGHT || controller->gamepad.axis_x >= 256)
        input |= KEY_RIGHT;

    return input;
}

// will clear the input for the current frame. useful for menus for example, when one doesnt want to triger an action on the match when exiting the menu.
void InputManager::clearInput(uni_controller_t* controller)
{
    keys_held[controller] = 0;
    keys_pressed[controller] = 0;
}

void InputManager::clearAllInputs()
{
    for(auto& controller : controllers)
    {
        clearInput(controller);
    }
}



