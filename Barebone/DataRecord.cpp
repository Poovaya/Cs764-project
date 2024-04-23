#include "DataRecord.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "defs.h"

DataRecord::DataRecord() {
    // Initialize data array to empty strings
    data = "";
}

DataRecord::DataRecord(std::string data) {
    // Set the values of key and columns
    this->data = data;
}

std::string DataRecord::getKey() const { return data; }

void DataRecord::show() { std::cout << this->data << std::endl; }

std::string DataRecord::getRecord() { return this->data; }

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

void DataRecord::initRandomRecord(int recordSize) {
    std::string rand_string = random_string(recordSize);
    this->data = rand_string;
}

bool DataRecord::compareDataRecords(DataRecord& a, DataRecord& b) {
    if (a.data <= b.data)
        return true;
    else
        return false;
}

struct DataRecordComparator {
    bool operator()(DataRecord* first, DataRecord* second) const {
        // Return true if first should go before second
        DataRecord a = *first;
        DataRecord b = *second;
        // return first.data[0][0] < second.data[0][0];  // return true;
        return DataRecord::compareDataRecords(a, b);
    }
};

DataRecord::DataRecord(const DataRecord& other) {
    if (this != &other) {  // Check for self-assignment
        // Copy each member from 'other' to 'this'
        this->index = other.index;
        this->data = other.data;
    }
}