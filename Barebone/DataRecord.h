#pragma once

#include <cstring>
#include <string>
#include <vector>

class DataRecord {
   private:
    std::string random_string(size_t length);

   public:
    std::vector<std::string> data;  // 2D array to store 3 columns
    int index;
    int column_width;
    DataRecord();
    DataRecord(std::string col1, std::string col2, std::string col3, std::string col4);

    std::string getKey() const;
    std::string getColumn(int index) const;
    void setColumn(int index, std::string value);
    void initRandomRecord(int column_width);
    void show();
    std::string getRecord();
};

// DATARECORD_H
