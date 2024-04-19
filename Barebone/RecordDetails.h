#pragma once

#include <string>
#include <vector>

#include "DataRecord.h"

using namespace std;

enum Type { SSD, HDD, DRAM };

class RecordDetails {
   public:
    vector<DataRecord*> recordLists;
    string runPath;
    Type deviceType;
};