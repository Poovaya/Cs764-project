#pragma once

#include <cstring>
#include <string>
#include <vector>

class DataRecord {
   private:
    std::string random_string(size_t length);

   public:
    std::string data;  // 1 column
    int index;
    DataRecord();
    DataRecord(std::string data);
    std::string getKey() const;
    void setColumn(int index, std::string &value);
    void initRandomRecord(int recordSize);
    void show();
    std::string getRecord();

    DataRecord(const DataRecord &);

    static bool compareDataRecords(DataRecord &, DataRecord &);

    bool operator==(const DataRecord &other) const {
        return (data == other.data);
    }
};
