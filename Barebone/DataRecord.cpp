#include "DataRecord.h"

#include <random>

#include "defs.h"

DataRecord::DataRecord() {
    // Initialize data array to empty strings
    for (int i = 0; i < 3; ++i) {
        memset(data[i], 0, sizeof(data[i]));
    }
}

DataRecord::DataRecord(const char* col1, const char* col2, const char* col3,
                       const char* col4) {
    // Set the values of key and columns
    strncpy(data[0], col1, sizeof(data[0]) - 1);  // Copy col1 to second column
    strncpy(data[1], col2, sizeof(data[1]) - 1);  // Copy col2 to third column
    strncpy(data[2], col3, sizeof(data[2]) - 1);  // Copy col3 to fourth column
    strncpy(data[3], col4, sizeof(data[3]) - 1);  // Copy col3 to fourth column
}

const char* DataRecord::getKey() const { return data[0]; }

const char* DataRecord::getColumn(int index) const {
    if (index >= 0 && index < 4) {
        return data[index];
    } else {
        return NULL;  // Return nullptr for invalid index
    }
}

void DataRecord::setColumn(int index, const char* value) {
    if (index >= 0 && index < 4 && value != nullptr) {
        strncpy(data[index], value, sizeof(data[index]) - 1);
    }
}

void DataRecord::show() {
    for (int i = 0; i < 4; i++) {
        std::cout << data[i] << " ";
    }
    std::cout << '\n';
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