#include "Scan.h"

ScanPlan::ScanPlan(RowCount const count, int column_width) : _count(count) {
    this->_rows = new DataRecord[count];
    for (RowCount i = 0; i < count; i++) {
        this->_rows[i].initRandomRecord(column_width);
    }
    TRACE(true);
}  // ScanPlan::ScanPlan

ScanPlan::~ScanPlan() { TRACE(true); }  // ScanPlan::~ScanPlan

Iterator* ScanPlan::init() const {
    TRACE(true);
    return new ScanIterator(this);
}  // ScanPlan::init

ScanIterator::ScanIterator(ScanPlan const* const plan)
    : _plan(plan), _count(0) {
    TRACE(true);
}  // ScanIterator::ScanIterator

ScanIterator::~ScanIterator() {
    TRACE(true);
    traceprintf("produced %lu of %lu rows\n", (unsigned long)(_count),
                (unsigned long)(_plan->_count));
}  // ScanIterator::~ScanIterator

bool ScanIterator::next() {
    TRACE(false);

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

std::vector<DataRecord> ScanPlan::GetAllRecords() const {
    std::vector<DataRecord> recList;
    for (RowCount i = 0; i < _count; i++) {
        recList.push_back(this->_rows[i]);
    }
    return recList;
}