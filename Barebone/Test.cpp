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
bool shouldRemoveDuplicates = false;

int main(int argc, char* argv[]) {
    // TRACE(false);

    // vector<vector<DataRecord*>> dr;
    // int numRecords = 28 * 2;
    // for (int i = 0; i < 4; i++) {
    //     ScanPlan* const plan = new ScanPlan(7, 4);
    //     Iterator* const it = plan->init();

    //     vector<DataRecord> recList = plan->GetAllRecords();
    //     Tree miniTree = Tree(recList, shouldRemoveDuplicates);
    //     miniTree.generateSortedRun();
    //     vector<DataRecord*> sortedRecList = miniTree.generated_run;
    //     dr.push_back(sortedRecList);
    // }
    // Tree tr = Tree(dr, numRecords, shouldRemoveDuplicates);

    // tr.printHeap();

    // tr.generateSortedRun();

    // vector<DataRecord*> records;

    // records = tr.generated_run;
    // cout << "SHOWING DR" << endl;
    // for (auto dr : records) {
    //     dr->show();
    // }
    // delete it;

    // delete plan;
    // Test duplicate removal
    vector<vector<DataRecord*>> dr;
    int numRecords = 28 * 2;
    for (int i = 0; i < 4; i++) {
        ScanPlan* const plan = new ScanPlan(7, 4);
        Iterator* const it = plan->init();

        vector<DataRecord> recList = plan->GetAllRecords();
        int old_count = recList.size();
        recList.resize(2 * old_count);
        std::copy_n(recList.begin(), old_count, recList.begin() + old_count);
        Tree miniTree = Tree(recList, shouldRemoveDuplicates);
        miniTree.generateSortedRun();
        vector<DataRecord*> sortedRecList = miniTree.generated_run;
        dr.push_back(sortedRecList);
    }
    Tree tr = Tree(dr, numRecords, shouldRemoveDuplicates);

    tr.printHeap();

    tr.generateSortedRun();

    vector<DataRecord*> records;

    records = tr.generated_run;
    cout << "SHOWING DR" << endl;
    for (auto dr : records) {
        dr->show();
    }

    return 0;
}  // main
