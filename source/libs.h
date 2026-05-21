#ifndef LIBS_H
#define LIBS_H

#include <stdio.h>
#include <iostream>
#include "pico/stdlib.h"
#include <pico/cyw43_arch.h>
#include "pico/multicore.h"
#include <btstack_run_loop.h>
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/interp.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"
#include "../utilities/hub75/hub75.hpp"
#include <queue>
#include <vector>
#include <unordered_map>
#include <time.h>
#include <stdlib.h>
#include <cstdint>
#include <variant>
#include <sstream>
#include <algorithm>
#include <uni.h>
#include <stack>
#include "pico/mem_ops.h"
#include <optional>
#include <variant>
#include <memory>
#include <cctype>
#include "lwip/apps/http_client.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"
#include "lwip/dns.h"

using namespace pimoroni;

#endif