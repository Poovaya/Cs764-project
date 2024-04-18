#include <string.h>
#include <unistd.h>  // For getopt

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>

#include "DataRecord.h"
#include "RecordDetails.h"
#include "DeviceConstants.h"
#include "Scan.h"
#include "Tree.h"
using namespace std;

namespace fs = filesystem;

extern int recordSize = 0;
extern long long int numRecords = 0;
extern int ON_DISK_RECORD_SIZE = 0;
struct DataRecordComparator {
    bool operator()(DataRecord *first, DataRecord *second) const {
        // Return true if first should go before second
        DataRecord a = *first;
        DataRecord b = *second;
        // return first.data[0][0] < second.data[0][0];  // return true;
        return DataRecord::compareDataRecords(a, b);
    }
};

void dramRuns(vector<RecordDetails*> &runsInMemory, StorageDevice &device,
              bool isFinal, int fileIndex) {
    int cacheSize = 1024 * 1024;
    int n = 0;
    for (auto x : runsInMemory) {
        n += x->recordLists.size();
    }
    if (runsInMemory.size() > 1) {
        Tree tree = Tree(runsInMemory, n, true, device, true);

        for (int i = 0; i < tree.numRecords; i++) {
            for (int inner_node_index = tree.numInnerNodes;
                 inner_node_index >= 0; inner_node_index--) {
                tree.run_tournament(inner_node_index);
            }

            tree.generated_run.push_back(tree.heap[0].dataRecord);
            // if generated_run.size = ssd page size, append

            tree.heap[0].dataRecord = NULL;
            if (tree.generated_run.size() * recordSize >= device.pageSize) {
                vector<DataRecord *> records = tree.generated_run;
                tree.generated_run.clear();
                device.spillRecordsToDisk(!isFinal, records, fileIndex);
                records.clear();
            }
        }
        vector<DataRecord *> records = tree.generated_run;
        tree.generated_run.clear();
        device.spillRecordsToDisk(!isFinal, records, fileIndex);
    } else if (runsInMemory.size() == 1) {
        vector<DataRecord *> records;
        for (auto rec : runsInMemory[0]->recordLists) {
            records.push_back(rec);
            if (records.size() * recordSize >= device.pageSize) {
                device.spillRecordsToDisk(!isFinal, records, fileIndex);
                records.clear();
            }
        }
        device.spillRecordsToDisk(!isFinal, records, fileIndex);
    }
}

void ssdRuns(StorageDevice &ssd, StorageDevice &hdd) {
    uint ssd_bandwidth = 100 * 1024 * 1024;
    double ssd_latency = 0.0001;
    int ssd_page_num_records =
        525;  //(ssd_bandwidth * ssd_latency) / (3 + 1 + 4 * 4);

    //  while (ssd.getTotalRuns()) {
    int num_records = 0;
    vector<RecordDetails*> recordDetailsLists;
    recordDetailsLists = ssd.getRecordsFromRunsOnDisk(ssd_page_num_records);
    for (auto x : recordDetailsLists) {
        num_records += x->recordLists.size();
    }
    if (num_records == 0) return;
    if (recordDetailsLists.size() > 1) {
        Tree tree = Tree(recordDetailsLists, numRecords, true, ssd, false);

        // tree.generateSortedRun();

        for (int i = 0; i < tree.numRecords; i++) {
            for (int inner_node_index = tree.numInnerNodes;
                 inner_node_index >= 0; inner_node_index--) {
                tree.run_tournament(inner_node_index);
            }

            tree.generated_run.push_back(tree.heap[0].dataRecord);

            tree.heap[0].dataRecord = NULL;
            if (tree.generated_run.size() * recordSize >= hdd.pageSize) {
                vector<DataRecord *> records = tree.generated_run;
                tree.generated_run.clear();
                hdd.spillRecordsToDisk(false, records, -1);
            }
        }

        vector<DataRecord *> records = tree.generated_run;
        tree.generated_run.clear();
        hdd.spillRecordsToDisk(false, records, -1);
    } else if (recordDetailsLists.size() == 1) {
        // Only one run in SSD;
        // records = recordDetailsLists[0];
        // hdd.spillRecordsToDisk(false, records);
        string ssdRunPath = "/home/kjain38/Cs764-project/Barebone/" +
                            ssd.device_path + "/sorted/sorted_run_1";
        string hddRunPath = "/home/kjain38/Cs764-project/Barebone/" +
                            hdd.device_path + "/merged_runs";

        if (access(ssdRunPath.c_str(), F_OK) == 0) {
            rename(ssdRunPath.c_str(), hddRunPath.c_str());
        }
    }

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
        vector<RecordDetails*> recordDetailsLists;

        recordDetailsLists = hdd.getRecordsFromRunsOnDisk(hdd_page_num_records);

        // ssd.spillRecordListToDisk(recordDetailsLists);
        ssdRuns(ssd, hdd);
    }

    return;
}

int main(int argc, char *argv[]) {
    int opt;
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

    // cout << fs::current_path() << endl;

    StorageDevice ssd = StorageDevice("SSD");
    StorageDevice hdd = StorageDevice("HDD");

    recordSize = recordSize * sizeof(char);
    int colWidth = recordSize / 4;
    ON_DISK_RECORD_SIZE = (4 * colWidth + 3 + 1);

    int dramSize = 100 * 1024 * 1024;
    long long int ssdSize = 10LL * 1024LL * 1024LL * 1024LL;
    long long int totalDataSize = recordSize * numRecords;

    int availableDramSize = dramSize * 0.9;
    long long int numRecsThatCanFitInRam = availableDramSize / recordSize + 1;
    int cacheSize = 1024 * 1024;
    int fanIn = 5000;
    long long int initialNumRecords = numRecords;
    long long int recordsGeneratedSoFar = 0;
    // long long int runSize = dramSize / fanIn;
    // long long int numRecordsPerRun = runSize / ON_DISK_RECORD_SIZE;
    long long int cacheMiniRunSize = cacheSize / recordSize + 1;

    // < 1MB
    if (totalDataSize <= cacheSize) {
        ScanPlan *const plan = new ScanPlan(numRecords, colWidth);
        vector<DataRecord *> recList = plan->GetAllRecords();

        sort(recList.begin(), recList.end(), DataRecordComparator());
        // WE ARE DONE
        string runPath =
            "/home/kjain38/Cs764-project/Barebone/HDD/merged_runs";

        fstream runfile;
        string str_records = "";

        runfile.open(runPath, fstream::out | fstream::app);
        if (!runfile.is_open()) return 0;

        for (uint ii = 0; ii < recList.size(); ii++) {
            DataRecord *record = recList[ii];
            string str_record = record->getRecord();
            str_records += str_record + "|";
        }
        runfile << str_records;
        runfile.close();
        return 0;
    }

    // > 1MB < 100MB
    if (totalDataSize <= availableDramSize) {
        vector<RecordDetails*> runsInMemory;
        while (initialNumRecords > 0) {
            int recordsToGenerate =
                std::min(initialNumRecords, cacheMiniRunSize);
            initialNumRecords -= recordsToGenerate;
            recordsGeneratedSoFar += recordsToGenerate;
            ScanPlan *const plan = new ScanPlan(recordsToGenerate, colWidth);
            vector<DataRecord *> recList = plan->GetAllRecords();

            sort(recList.begin(), recList.end(), DataRecordComparator());

            RecordDetails* recordDetails = new RecordDetails;
            recordDetails->recordLists = recList;
            recordDetails->runPath = "";
            recordDetails->deviceType = Type::DRAM;
            runsInMemory.push_back(recordDetails);
        }
        dramRuns(runsInMemory, hdd, true, -1);

        return 0;
    }

    // > 100MB  < 10GB
    if (totalDataSize <= ssdSize) {
        long long int numSSDRuns = (totalDataSize / dramSize) + 1;
        int ssdRunIndex = 1;
        vector<ScanPlan *> toDelete;
        while (initialNumRecords > 0) {
            vector<RecordDetails*> runsInMemory;
            long long int dramInitialNumRecords =
                std::min(numRecsThatCanFitInRam, initialNumRecords);
            initialNumRecords -= dramInitialNumRecords;
            while (dramInitialNumRecords > 0) {
                int recordsToGenerate =
                    std::min(dramInitialNumRecords, cacheMiniRunSize);
                dramInitialNumRecords -= recordsToGenerate;
                recordsGeneratedSoFar += recordsToGenerate;
                ScanPlan *const plan =
                    new ScanPlan(recordsToGenerate, colWidth);
                toDelete.push_back(plan);
                vector<DataRecord *> recList = plan->GetAllRecords();

                sort(recList.begin(), recList.end(), DataRecordComparator());

                RecordDetails* recordDetails = new RecordDetails;
                recordDetails->recordLists = recList;
                recordDetails->runPath = "";
                recordDetails->deviceType = Type::DRAM;
                runsInMemory.push_back(recordDetails);
                recList.clear();
            }
            dramRuns(runsInMemory, ssd, false, ssdRunIndex);
            for (int i=0;i<runsInMemory.size();i++) {
                delete runsInMemory[0];
            }
            runsInMemory.clear();
            for (auto p : toDelete) {
                delete p;
            }
            toDelete.clear();
            ssdRunIndex++;
        }
        ssdRuns(ssd, hdd);
        return 0;
    }

    // while (numRecords) {
    //  int remNumRecordsInDram =
    //      std::min((dramSize / ON_DISK_RECORD_SIZE), numRecords);
    //  int recordCount = 0;
    //  vector<vector<DataRecord *>> runs;

    // int recordsToSpill = 0;
    // while (remNumRecordsInDram > 0) {
    //     int recordsToGenerate = 0;
    //     if (numRecordsPerRun >= remNumRecordsInDram) {
    //         numRecords -= remNumRecordsInDram;
    //         recordsToGenerate = remNumRecordsInDram;
    //         remNumRecordsInDram = 0;
    //     } else {
    //         remNumRecordsInDram -= numRecordsPerRun;
    //         numRecords -= numRecordsPerRun;
    //         recordsToGenerate = numRecordsPerRun;
    //     }

    //     ScanPlan *const plan = new ScanPlan(recordsToGenerate, colWidth);
    //     recordsToSpill += recordsToGenerate;

    //     vector<DataRecord *> recList = plan->GetAllRecords();

    //     sort(recList.begin(), recList.end(), DataRecordComparator());

    //     runs.push_back(recList);
    // }
    //   int recordsToGenerate = std::min(numRecords, cacheMiniRunSize);
    //   numRecords -= recordsToGenerate;
    //   recordsGeneratedSoFar += recordsToGenerate;
    //   ScanPlan *const plan = new ScanPlan(recordsToGenerate, colWidth);
    //   vector<DataRecord *> recList = plan->GetAllRecords();
    // sort(recList.begin(), recList.end(), DataRecordComparator());
    // if (initialNumRecords <= cacheMiniRunSize) {
    //     // WE ARE DONE
    //     hdd.spillRecordsToDisk(false, recList);
    //     return 0;
    // }
    // if (availableDramSize >= recList.size() * recordSize) {
    //     runsInMemory.push_back(recList);
    //     availableDramSize -= recList.size() * recordSize;
    // } else if (ssd.ssdSize >= 0) {
    //     ssd.spillRecordsToDisk(true, recList);
    // } else {
    //     hdd.spillRecordsToDisk(true, recList);
    // }

    // if (recordsToSpill * ON_DISK_RECORD_SIZE <= ssd.ssdSize) {
    //     ssd.spillRecordListToDisk(runs);
    // } else {
    //     hdd.spillRecordListToDisk(runs);
    //    // }
    // }
    // dramRuns(runsInMemory, ssd);
    // ssdRuns(ssd, hdd);
    // hdd.commitRun();
    // hddRuns(ssd, hdd);c

    return 0;
}
