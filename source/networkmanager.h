#include "libs.h"

namespace NetworkManager
{

    struct Network
    {
        // maybe could add more info but meh
        std::string ssid = "";
        std::string password = "";
        int rssi = 0;
        int channel = 0;
        uint8_t bssid[6];
        int auth_mode;
    };
        

    extern bool enabled;
    extern Network current_network;

    extern std::vector<Network> found_networks;

    bool enable();
    bool disable();

    int connectToNetwork(const std::string& ssid, const std::string& password);
    int connectToCurrentNetwork();

    std::string extractBody(const std::string& response);
    std::string httpPost(const char* server_ip, uint16_t port, const char* path, const char* body);
    std::string httpGet(const char* server_ip, uint16_t port, const char* path);


    bool getNearNetworks();
};