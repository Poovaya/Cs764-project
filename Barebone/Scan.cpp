#include "Scan.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "defs.h"

ScanPlan::ScanPlan(RowCount const count, int recordSize) : _count(count) {
    this->_rows = std::vector<DataRecord>(count);
    for (RowCount i = 0; i < count; i++) {
        this->_rows[i].initRandomRecord(recordSize);
    }

    std::string runPath =
        "/home/poovaya/project764/Cs764-project/Barebone/HDD/input";

    std::string str_records = "";
    std::fstream runfile;
    for (uint ii = 0; ii < this->_rows.size(); ii++) {
        DataRecord record = (this->_rows[ii]);
        std::string str_record = record.getRecord();
        str_record[str_record.size() - 1] = '\n';
        str_records += str_record;
    }

    runfile.open(runPath, std::fstream::out | std::fstream::app);
    if (!runfile.is_open()) return;

    runfile << str_records;

    runfile.close();

    // TRACE(true);
}  // ScanPlan::ScanPlan

ScanPlan::~ScanPlan() {
    // TRACE(true);
}  // ScanPlan::~ScanPlan

Iterator* ScanPlan::init() const {
    // TRACE(true);
    return new ScanIterator(this);
}  // ScanPlan::init

ScanIterator::ScanIterator(ScanPlan const* const plan)
    : _plan(plan), _count(0) {
    // TRACE(true);
}  // ScanIterator::ScanIterator

ScanIterator::~ScanIterator() {
    //  TRACE(true);
    traceprintf("produced %lu of %lu rows\n", (unsigned long)(_count),
                (unsigned long)(_plan->_count));
}  // ScanIterator::~ScanIterator

bool ScanIterator::next() {
    //  TRACE(false);

    if (_count >= _plan->_count) return false;
    this->_plan->GetRecord(_count).show();
    std::string rec = this->_plan->GetRecord(_count).getRecord();
    std::cout << "GET RECORD " << rec << std::endl;
    ++_count;
    return true;
}  // ScanIterator::next

DataRecord ScanPlan::GetRecord(RowCount const rowid) const {
    return this->_rows[rowid];
}  // ScanPlan::GetRecord

std::vector<DataRecord*> ScanPlan::GetAllRecords() {
    std::vector<DataRecord*> recList(_count);
    for (RowCount i = 0; i < _count; i++) {
        recList[i] = &this->_rows[i];
    }
    return recList;
}