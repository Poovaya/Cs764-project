#include "DataRecord.h"

#include <random>
#include <iostream>
#include <string>

#include "defs.h"

DataRecord::DataRecord() {
    // Initialize data array to empty strings
    // for (int i = 0; i < 4; ++i) {
    //     memset(data[i], 0, sizeof(data[i]));
    // }
}

DataRecord::DataRecord(std::string col1, std::string  col2, std::string  col3, std::string  col4) {
    // Set the values of key and columns
    data.push_back(col1);
    data.push_back(col2);
    data.push_back(col3);
    data.push_back(col4);
}

std::string DataRecord::getKey() const { return data[0]; }

std::string DataRecord::getColumn(int index) const {
    if (index >= 0 && index < 4) {
        return data[index];
    } else {
        return NULL;  // Return nullptr for invalid index
    }
}

void DataRecord::setColumn(int index, std::string value) {
    if (index >= 0 && index < 4) {
        data[index] = value;
    }
}

void DataRecord::show() {
    for (int i = 0; i < 4; i++) {
        std::cout << data[i] << " ";
    }
    std::cout << '\n';
}

std::string DataRecord::getRecord() {
    std::string recordString = "";
    for (int i = 0; i < 4; i++) {
        recordString += this->getColumn(i);
        if (i != 3) {
            recordString += " ";
        }
    }
    return recordString;
}

std::string DataRecord::random_string(size_t length) {
    auto randchar = []() -> char {
        const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

void DataRecord::initRandomRecord(int column_width) {
    for (int i = 0; i < 4; i++) {
        std::string rand_string = random_string(4);
        setColumn(i, rand_string.c_str());
    }
}