#pragma once

#include "DataRecord.h"

#define CACHE_SIZE 1 * 1024 * 1024        // 1 MB
#define DRAM_SIZE 100 * 1024 * 1024       // 100 MB
#define SSD_SIZE 10 * 1024 * 1024 * 1024  // 10 GB
#define SSD_LATENCY 1e-4                  // 0.1 ms
#define SSD_BANDWIDTH 200 * 1024 * 1024   // 200 MB/s
#define HDD_SIZE INT_MAX                  // Infinite
#define HDD_LATENCY 5 * 1e-3              // 5 ms
#define HDD_BANDWIDTH 100 * 1024 * 1024   // 100 MB/s

#define SSD_PAGE_SIZE = SSD_BANDWIDTH * SSD_LATENCY  // 20972 Approx 20kb
#define HDD_PAGE_SIZE = HDD_BANDWIDTH * HDD_LATENCY  // 524288 Approx 500kb
#define FAN_IN  5000  // DRAM_SIZE / SSD_PAGE_SIZE           // 5000
