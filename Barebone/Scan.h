#include "defs.h"
#include "DataRecord.h"
#include "Iterator.h"

class ScanPlan : public Plan {
    friend class ScanIterator;

   public:
    ScanPlan(RowCount const count, int column_width);
    ~ScanPlan();
    Iterator *init() const;
    DataRecord GetRecord(RowCount const rowid) const;
    std::vector<DataRecord> GetAllRecords() const;

   private:
    RowCount const _count;
    DataRecord *_rows;

};  // class ScanPlan

class ScanIterator : public Iterator {
   public:
    ScanIterator(ScanPlan const *const plan);
    ~ScanIterator();
    bool next();

   private:
    ScanPlan const *const _plan;
    RowCount _count;
};  // class ScanIterator
