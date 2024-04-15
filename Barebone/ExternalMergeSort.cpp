#include <string.h>
#include <unistd.h>  // For getopt

#include <algorithm>
#include <iostream>
#include <string>

#include "DataRecord.h"
#include "Scan.h"
#include "Tree.h"

using namespace std;

struct DataRecordComparator {
    bool operator()(DataRecord *first, DataRecord *second) const {
        // Return true if first should go before second
        DataRecord a = *first;
        DataRecord b = *second;
        // return first.data[0][0] < second.data[0][0];  // return true;
        return DataRecord::compareDataRecords(a, b);
    }
};

void ssdRuns(StorageDevice &ssd, StorageDevice &hdd) {
    uint ssd_bandwidth = 100 * 1024 * 1024;
    double ssd_latency = 0.0001;
    int ssd_page_num_records =
        524;  //(ssd_bandwidth * ssd_latency) / (3 + 1 + 4 * 4);

    //  while (ssd.getTotalRuns()) {
    int num_records = 0;
    vector<DataRecord *> records;
    vector<vector<DataRecord *>> record_lists;
    record_lists = ssd.getRecordsFromRunsOnDisk(ssd_page_num_records);
    for (auto x : record_lists) {
        num_records += x.size();
    }
    if (num_records == 0) return;
    if (record_lists.size() > 1) {
        Tree tree = Tree(record_lists, 50000, true, ssd);

        tree.generateSortedRun();

        records = tree.generated_run;
    } else if (record_lists.size() == 1) {
        records = record_lists[0];
    }
    hdd.spillRecordsToDisk(false, records);
    //   }

    return;
}

void hddRuns(StorageDevice &ssd, StorageDevice &hdd) {
    uint hdd_bandwidth = 100 * 1024 * 1024;
    double hdd_latency = 5 * 1e-3;
    uint hdd_page_num_records =
        26214;  // (hdd_bandwidth * hdd_latency) / (3 + 1 + 4 * 4);

    if (hdd.getTotalRuns() == 1) {
        return;
    }
    while (hdd.getTotalRuns()) {
        vector<vector<DataRecord *>> record_lists;

        record_lists = hdd.getRecordsFromRunsOnDisk(hdd_page_num_records);

        ssd.spillRecordListToDisk(record_lists);
        ssdRuns(ssd, hdd);
    }

    return;
}

int main(int argc, char *argv[]) {
    int opt;
    int numRecords = 0;  // Initialize to default values
    int recordSize = 16;
    string trace_file = "trace";

    // Process command-line arguments using getopt
    while ((opt = getopt(argc, argv, "c:s:o:")) !=
           -1) {  // Added ':' for option 'o' to indicate it expects an
                  // argument
        switch (opt) {
            case 'c':
                numRecords = atoi(optarg);
                break;
            case 's':
                recordSize = atoi(optarg);
                break;
            case 'o':
                trace_file = optarg;
                break;
            default: /* '?' */
                abort();
        }
    }

    // Process non-option arguments here
    cout << numRecords << " " << recordSize << " " << trace_file << endl;

    StorageDevice ssd = StorageDevice("SSD");
    StorageDevice hdd = StorageDevice("HDD");

    recordSize = 16 * sizeof(char);
    int colWidth = recordSize / 4;
    int ON_DISK_RECORD_SIZE = (4 * colWidth + 3 + 1);

    int dramSize = 100 * 1024 * 1024;
    int fanIn = 5000;
    long long int runSize = dramSize / fanIn;
    long long int numRecordsPerRun = runSize / ON_DISK_RECORD_SIZE;
    while (numRecords) {
        int remNumRecordsInDram =
            std::min((dramSize / ON_DISK_RECORD_SIZE), numRecords);
        int recordCount = 0;
        vector<vector<DataRecord *>> runs;

        int recordsToSpill = 0;
        while (remNumRecordsInDram > 0) {
            int recordsToGenerate = 0;
            if (numRecordsPerRun >= remNumRecordsInDram) {
                numRecords -= remNumRecordsInDram;
                recordsToGenerate = remNumRecordsInDram;
                remNumRecordsInDram = 0;
            } else {
                remNumRecordsInDram -= numRecordsPerRun;
                numRecords -= numRecordsPerRun;
                recordsToGenerate = numRecordsPerRun;
            }

            ScanPlan *const plan = new ScanPlan(recordsToGenerate, colWidth);
            recordsToSpill += recordsToGenerate;

            vector<DataRecord *> recList = plan->GetAllRecords();

            sort(recList.begin(), recList.end(), DataRecordComparator());

            runs.push_back(recList);
        }

        if (recordsToSpill * ON_DISK_RECORD_SIZE <= ssd.ssdSize) {
            ssd.spillRecordListToDisk(runs);
        } else {
            hdd.spillRecordListToDisk(runs);
        }
    }

    ssdRuns(ssd, hdd);
    hdd.commitRun();
    hddRuns(ssd, hdd);

    return 0;
}
