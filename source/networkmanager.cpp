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
    enabled = true;


    int result = -1;
    if(current_network.ssid.empty())
    {
        printf("no network configured\n");
    }
    else
        result = connectToCurrentNetwork();



    printf("couldn't connect to network!!\n\n"); 

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


    auto current_language = (Language)std::get<EnumOption>(tetrismatrix::settings.current_language).value;
    std::string text = translate(current_language, Texts::CONNECTING);

    showMessage(tetrismatrix::hub75, text, 0, tetrismatrix::vertical);
   
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


    uint32_t start = to_ms_since_boot(get_absolute_time());
    while(cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP)
    {
        cyw43_arch_poll();
        if(to_ms_since_boot(get_absolute_time()) - start > 5000)
            break;
    }
    printf("ip found! connected!!\n\n");

    printf("connected to network!!\n\n");

    std::string text_1;
    std::string text_2;
    switch(result)
    {
        case PICO_ERROR_BADAUTH:
            if(current_language == Language::EN)
            {
                text_1 = translate(current_language, Texts::WRONG);
                text_2 = translate(current_language, Texts::PASSWORD);
            }
            else
            {
                text_1 = translate(current_language, Texts::PASSWORD);
                text_2 = translate(current_language, Texts::WRONG);
            }
            break;
        case PICO_ERROR_TIMEOUT:
            text_1 = translate(current_language, Texts::TIMEOUT);
            text_2 = translate(current_language, Texts::REACHED);
            break;

        case PICO_ERROR_CONNECT_FAILED:

            if(current_language == Language::EN)
            {
                text_1 = translate(current_language, Texts::UNKNOWN);
                text_2 = translate(current_language, Texts::ERROR);
            }
            else
            {
                text_1 = translate(current_language, Texts::ERROR);
                text_2 = translate(current_language, Texts::UNKNOWN);
            }
            break;
        case 0:
            text_1 = translate(current_language, Texts::NETWORK);
            text_2 = translate(current_language, Texts::CONNECTED);
            break;


    }

 

    text = text_1 + " " + text_2;
    showMessage(tetrismatrix::hub75, text, 2000, tetrismatrix::vertical);

    current_network = {.ssid = ssid, .password = password};

    return result;
}

int NetworkManager::connectToCurrentNetwork()
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

    std::string text;

    text = translate(current_language, Texts::SEARCHING) + " " + translate(current_language, Texts::NETWORKS);

    showMessage(tetrismatrix::hub75, text, 5000, tetrismatrix::vertical);


    return err == 0;

}

struct HttpContext 
{
    std::string request; 
    std::string response; 
    bool done = false;  
    bool error = false;   
};

// the recv callback - called whenever data arrives from the server
static err_t http_recv(void* arg, struct tcp_pcb* pcb, struct pbuf* p, err_t err)
{
    HttpContext* ctx = (HttpContext*)arg;

    if(p == NULL) {
        // NULL pbuf means the server closed the connection - we're done
        ctx->done = true;
        tcp_close(pcb);
        return ERR_OK;
    }

    // copy received data into our response string
    char* buf = new char[p->len + 1];
    memcpy(buf, p->payload, p->len);
    buf[p->len] = 0;
    ctx->response += buf;
    delete[] buf;

    // tell lwIP we've consumed this data
    tcp_recved(pcb, p->tot_len);
    pbuf_free(p);
    return ERR_OK;
}

// the connected callback - called when TCP connection is established
static err_t http_connected(void* arg, struct tcp_pcb* pcb, err_t err)
{
    if(err != ERR_OK) return err;

    HttpContext* ctx = (HttpContext*)arg;

    // send the HTTP request
    // TCP_WRITE_FLAG_COPY means lwIP makes its own copy, safe to use stack memory
    err_t r = tcp_write(pcb, ctx->request.c_str(), ctx->request.size(), TCP_WRITE_FLAG_COPY);
    if(r != ERR_OK) {
        ctx->error = true;
        return r;
    }

    // flush the send buffer
    tcp_output(pcb);
    return ERR_OK;
}

// the error callback - called on any TCP error
static void http_err(void* arg, err_t err)
{
    printf("http_err called: %d\n", err);
    HttpContext* ctx = (HttpContext*)arg;
    ctx->error = true;
    ctx->done = true;
}

std::string NetworkManager::httpPost(const char* server_ip, uint16_t port, const char* path, const char* body)
{
    // build the full HTTP POST request string
    HttpContext ctx;
    ctx.request = "POST ";
    ctx.request += path;
    ctx.request += " HTTP/1.1\r\n";
    ctx.request += "Host: ";
    ctx.request += server_ip;
    ctx.request += "\r\n";
    ctx.request += "Content-Type: application/json\r\n";
    ctx.request += "Content-Length: ";
    ctx.request += std::to_string(strlen(body));
    ctx.request += "\r\n";
    ctx.request += "Connection: close\r\n";
    ctx.request += "\r\n";
    ctx.request += body;



    // create a new TCP control block
    struct tcp_pcb* pcb = tcp_new();
    if(!pcb) 
    {
        printf("failed to create pcb\n");
        return "";
    }

    // pass our context to all callbacks via arg
    tcp_arg(pcb, &ctx);
    tcp_recv(pcb, http_recv);
    tcp_err(pcb, http_err);

    // convert IP string to lwIP address struct
    ip_addr_t addr;
    if(ipaddr_aton(server_ip, &addr) == 0) {
        printf("invalid ip address\n");
        tcp_abort(pcb);
        return "";
    }

    printf("connecting to %s:%d%s\n", server_ip, port, path);

    // initiate the TCP connection. http_connected will be called when it succeeds
    err_t err = tcp_connect(pcb, &addr, port, http_connected);
    if(err != ERR_OK) 
    {
        printf("tcp_connect failed: %d\n", err);
        return "";
    }

    // poll until done. this drives the lwIP stack forward
    uint32_t start = to_ms_since_boot(get_absolute_time());
    while(!ctx.done && !ctx.error) 
    {
        cyw43_arch_poll();
        // timeout after 10 seconds
        if(to_ms_since_boot(get_absolute_time()) - start > 10000) 
        {
            printf("http request timed out\n");
            tcp_abort(pcb);
            return "";
        }
    }

    printf("response: %s\n", ctx.response.c_str());
    printf("error: %d\n", ctx.error);
    printf("tcp_connect result: %d\n", err);
    return ctx.response;
}

std::string NetworkManager::httpGet(const char* server_ip, uint16_t port, const char* path)
{
    HttpContext ctx;
    ctx.request = "GET ";
    ctx.request += path;
    ctx.request += " HTTP/1.1\r\n";
    ctx.request += "Host: ";
    ctx.request += server_ip;
    ctx.request += "\r\n";
    ctx.request += "Connection: close\r\n";
    ctx.request += "\r\n";

    struct tcp_pcb* pcb = tcp_new();
    if(!pcb) {
        printf("failed to create pcb\n");
        return "";
    }

    tcp_arg(pcb, &ctx);
    tcp_recv(pcb, http_recv);
    tcp_err(pcb, http_err);

    ip_addr_t addr;
    if(ipaddr_aton(server_ip, &addr) == 0) {
        printf("invalid ip address\n");
        tcp_abort(pcb);
        return "";
    }

    err_t err = tcp_connect(pcb, &addr, port, http_connected);
    if(err != ERR_OK) {
        printf("tcp_connect failed: %d\n", err);
        return "";
    }

    uint32_t start = to_ms_since_boot(get_absolute_time());
    while(!ctx.done && !ctx.error) {
        cyw43_arch_poll();
        if(to_ms_since_boot(get_absolute_time()) - start > 10000) {
            printf("http request timed out\n");
            tcp_abort(pcb);
            return "";
        }
    }

    return ctx.response;
}

std::string NetworkManager::extractBody(const std::string& response) 
{
    size_t pos = response.find("\r\n\r\n");
    if(pos == std::string::npos) return "";
    return response.substr(pos + 4);
}
