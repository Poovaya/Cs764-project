#pragma once

#include <cstring>
#include <string>
#include <vector>

class DataRecord {
   private:
    std::string random_string(size_t length);

   public:
    std::vector<std::string> data;  // 2D array to store 4 columns
    int index;
    int column_width;
    DataRecord();
    DataRecord(std::string col1, std::string col2, std::string col3,
               std::string col4);

    std::string getKey() const;
    std::string getColumn(int index) const;
    void setColumn(int index, std::string &value);
    void initRandomRecord(int column_width);
    void show();
    std::string getRecord();

    DataRecord(const DataRecord &);

    static bool compareDataRecords(DataRecord &, DataRecord &);

    bool operator==(const DataRecord &other) const {
        return (data[0] == other.data[0] && data[1] == other.data[1] &&
                data[2] == other.data[2] && data[3] == other.data[3]);
    }
};
