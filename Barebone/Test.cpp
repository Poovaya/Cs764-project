#include <algorithm>
#include <iostream>
#include <vector>

#include "DataRecord.h"
#include "Filter.h"
#include "Iterator.h"
#include "Scan.h"
#include "Sort.h"
#include "Tree.h"
#include "defs.h"
using namespace std;
struct DataRecordComparator {
    bool operator()(DataRecord& first, DataRecord& second) const {
        // Return true if first should go before second
        // return first.data[0][0] < second.data[0][0];  // return true;
        return DataRecord::compareDataRecords(first, second);
    }
};
int main(int argc, char* argv[]) {
    // TRACE(false);

    vector<vector<DataRecord>> dr;
    for (int i = 0; i < 4; i++) {
        ScanPlan* const plan = new ScanPlan(7, 4);
        Iterator* const it = plan->init();

        vector<DataRecord> recList = plan->GetAllRecords();
        sort(recList.begin(), recList.end(), DataRecordComparator());
        dr.push_back(recList);
    }
    Tree tr = Tree(dr);

    tr.printHeap();

    for (int i = 0; i < 7 * 4; i++) {
        tr.run_tree();
    }

    vector<DataRecord*> records;

    records = tr.generated_run;
    cout << "SHOWING DR" << endl;
    for (auto dr : records) {
        dr->show();
    }
    // delete it;

    // delete plan;

    return 0;
}  // main
