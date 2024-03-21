#pragma once

#include <bits/stdc++.h>

#include <cstring>

class DataRecord {
   private:
    std::string random_string(size_t length);

   public:
    char data[4][256];  // 2D array to store 3 columns
    int index;
    int column_width;
    DataRecord();
    DataRecord(const char* col1, const char* col2, const char* col3,
               const char* col4);

    const char* getKey() const;
    const char* getColumn(int index) const;
    void setColumn(int index, const char* value);
    void initRandomRecord(int column_width);
    void show();
};

typedef struct DataRecordList {
    std::list<DataRecord> records;
    long int num_records;
};

// DATARECORD_H
