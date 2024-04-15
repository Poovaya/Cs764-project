#include "DataRecord.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "defs.h"

DataRecord::DataRecord() {
    // Initialize data array to empty strings
    data = std::vector<std::string>(4, "");
}

DataRecord::DataRecord(std::string col1, std::string col2, std::string col3,
                       std::string col4) {
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

void DataRecord::setColumn(int index, std::string& value) {
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
        setColumn(i, rand_string);
    }
}

bool DataRecord::compareDataRecords(DataRecord& a, DataRecord& b) {
    for (int i = 0; i < 4; i++) {
        if (a.data[i] == b.data[i])
            continue;
        else if (a.data[i] < b.data[i])
            return true;
        else
            return false;
    }
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
        this->column_width = other.column_width;
        this->data = std::vector<std::string>(4, "");
        for (int i = 0; i < 4; ++i) {
            this->data[i] = other.data[i];
        }
    }
}