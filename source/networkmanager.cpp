#include "networkmanager.h"
#include "game.h"
#include "language.h"
#include "helpers.h"


NetworkManager::Network NetworkManager::current_network;
std::vector<NetworkManager::Network> NetworkManager::found_networks;
bool NetworkManager::enabled = false;


bool NetworkManager::enable()
{
    // pause bluetooth input processing
    uni_bt_enable_new_connections_safe(false);
    
    cyw43_arch_enable_sta_mode();
    
    int result = connectToCurrentNetwork();
    
    enabled = true;
    return result == 0;
}

bool NetworkManager::disable()
{
    cyw43_arch_disable_sta_mode();
    uni_bt_enable_new_connections_safe(true);

    enabled = false;
    return true;
}

int NetworkManager::connectToNetwork(const std::string& ssid, const std::string& password)
{
    if(!enabled)
        return -1;
    const char* p = (password == "") ? NULL : password.c_str();
    printf("ssid: %s\n", ssid.c_str());
    printf("password: %s\n", password.c_str());
    int result = cyw43_arch_wifi_connect_timeout_ms(
        ssid.c_str(), 
        p, 
        CYW43_AUTH_WPA2_AES_PSK, 
        10000
    );
    
    if(result != 0) 
    {
        printf("wifi connect failed: %d\n", result);
        if(result == PICO_ERROR_BADAUTH)
            printf("password was incorrect!!\n\n");
        else if(result == PICO_ERROR_TIMEOUT)
            printf("timeout reached!!\n\n");
        else
            printf("unknown cause of error!!\n\n");
        return result;
    }

    printf("connected to network!!\n\n");

    current_network = {.ssid = ssid, .password = password};
    
    return result;
}

bool NetworkManager::connectToCurrentNetwork()
{
    return connectToNetwork(current_network.ssid, current_network.password);
}

static int scan_result(void *env, const cyw43_ev_scan_result_t *result) 
{
    if(result) 
    {
        printf("new network found!!\n\n");
        printf("SSID: %-32s RSSI: %4d Channel: %3d MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
            result->ssid, result->rssi, result->channel,
            result->bssid[0], result->bssid[1], result->bssid[2],
            result->bssid[3], result->bssid[4], result->bssid[5]);
        
        NetworkManager::Network network;
        network.ssid = std::string((char*)result->ssid);
        network.rssi = result->rssi;
        network.channel = result->channel;
        memcpy(network.bssid, result->bssid, 6);
        network.auth_mode = result->auth_mode;

        NetworkManager::found_networks.push_back(network);
        return 0;
    }

    return 1;
}

bool NetworkManager::getNearNetworks()
{
    if(!enabled)
        return false;
    tetrismatrix::hub75.clear();
    found_networks.clear();
    // default scan options
    cyw43_wifi_scan_options_t scan_options = {0};

    int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_result);

    int x = tetrismatrix::vertical ? SCREEN_HEIGHT / 2 : SCREEN_WIDTH / 2;
    int y = tetrismatrix::vertical ? SCREEN_WIDTH / 2 : SCREEN_HEIGHT / 2;

    Language current_language = (Language)std::get<EnumOption>(tetrismatrix::settings.current_language).value;

    int limit = tetrismatrix::vertical ? SCREEN_HEIGHT : SCREEN_WIDTH;
    std::string searching = translate(current_language, Texts::SEARCHING);
    cropTextToFit(searching, 3, limit);
    draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, searching, x - (searching.size() * 4 / 2), y, 255, 255 ,255, tetrismatrix::vertical);
    
    std::string networks = translate(current_language, Texts::NETWORKS);
    cropTextToFit(networks, 3, limit - 12);
    draw4x3Text(tetrismatrix::hub75, tetrismatrix::graphics, networks, x - (networks.size() * 4 / 2), y + 6, 255, 255 ,255, tetrismatrix::vertical);

    sleep_ms(5000);


    return err == 0;

}
