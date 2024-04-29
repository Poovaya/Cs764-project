#include <string.h>
#include <unistd.h>  // For getopt

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>

#include "DataRecord.h"
#include "DeviceConstants.h"
#include "RecordDetails.h"
#include "Scan.h"
#include "SortTrace.h"
#include "Tree.h"
using namespace std;
string trace_file = "trace";

SortTrace trace(trace_file);

namespace fs = filesystem;

extern int recordSize = 0;
extern long long int numRecords = 0;
extern int ON_DISK_RECORD_SIZE = 0;
extern long long int totalLatency = 0;
struct DataRecordComparator {
    bool operator()(DataRecord *first, DataRecord *second) const {
        // Return true if first should go before second
        DataRecord a = *first;
        DataRecord b = *second;
        return DataRecord::compareDataRecords(a, b);
    }
};

void dramRuns(vector<RecordDetails *> &runsInMemory, StorageDevice &device,
              bool isFinal, int fileIndex) {
    int cacheSize = 1024 * 1024;
    int n = 0;
    for (auto x : runsInMemory) {
        n += x->recordLists.size();
    }
    if (runsInMemory.size() > 1) {
        Tree tree = Tree(runsInMemory, n, device, device);

        for (int i = 0; i < tree.numRecords; i++) {
            for (int inner_node_index = tree.numInnerNodes;
                 inner_node_index >= 0; inner_node_index--) {
                tree.run_tournament(inner_node_index);
            }

            // tree.generated_run.push_back(tree.heap[0].dataRecord);
            //  if generated_run.size = ssd page size, append
            tree.pushRecordToGeneratedRun(tree.heap[0].dataRecord);

            tree.heap[0].dataRecord = NULL;
            if (tree.generated_run.size() * ON_DISK_RECORD_SIZE >=
                device.pageSize) {
                vector<DataRecord *> records = tree.generated_run;
                tree.generated_run.clear();

                begin_time = clock();
                device.spillRecordsToDisk(!isFinal, records, fileIndex);
                time_spent_us = float(clock() - begin_time) * 1000 * 1000 / CLOCKS_PER_SEC;
                totalLatency += time_spent_us;
                records.clear();
            }
        }
        vector<DataRecord *> records = tree.generated_run;
        tree.generated_run.clear();

        begin_time = clock();
        device.spillRecordsToDisk(!isFinal, records, fileIndex);
        time_spent_us = float(clock() - begin_time) * 1000 * 1000 / CLOCKS_PER_SEC;
        totalLatency += time_spent_us;
    } else if (runsInMemory.size() == 1) {
        vector<DataRecord *> records;
        for (auto rec : runsInMemory[0]->recordLists) {
            records.push_back(rec);
            if (records.size() * ON_DISK_RECORD_SIZE >= device.pageSize) {
                begin_time = clock();
                device.spillRecordsToDisk(!isFinal, records, fileIndex);
                time_spent_us = float(clock() - begin_time) * 1000 * 1000 / CLOCKS_PER_SEC;
                totalLatency += time_spent_us;
                records.clear();
            }
        }
        begin_time = clock();
        device.spillRecordsToDisk(!isFinal, records, fileIndex);
        time_spent_us = float(clock() - begin_time) * 1000 * 1000 / CLOCKS_PER_SEC;
        totalLatency += time_spent_us;
    }
}

void ssdRuns(vector<RecordDetails *> runsLeftInMemory, StorageDevice &ssd,
             StorageDevice &hdd, bool isFinal, int hddRunIndex,
             long long int recordsToPutInTree) {
    uint ssd_bandwidth = 200 * 1024 * 1024;
    double ssd_latency = 0.0001;
    // int ssd_page_num_records =
    //     525;  //(ssd_bandwidth * ssd_latency) / (3 + 1 + 4 * 4);

    int ssd_page_num_records = ssd.pageSize / ON_DISK_RECORD_SIZE;

    //  while (ssd.getTotalRuns()) {
    int num_records = 0;
    vector<RecordDetails *> recordDetailsLists;

    long long int time_spent_us;
	clock_t begin_time;
    begin_time = clock();
    recordDetailsLists = ssd.getRecordsFromRunsOnDisk(ssd_page_num_records);
    time_spent_us = float(clock() - begin_time) * 1000 * 1000 / CLOCKS_PER_SEC;
    totalLatency += time_spent_us;

    recordDetailsLists.insert(recordDetailsLists.end(),
                              runsLeftInMemory.begin(), runsLeftInMemory.end());

    for (auto x : recordDetailsLists) {
        num_records += x->recordLists.size();
    }
    if (num_records == 0) return;
    if (recordDetailsLists.size() > 1) {
        Tree tree = Tree(recordDetailsLists, recordsToPutInTree, ssd, hdd);

        for (int i = 0; i < tree.numRecords; i++) {
            for (int inner_node_index = tree.numInnerNodes;
                 inner_node_index >= 0; inner_node_index--) {
                tree.run_tournament(inner_node_index);
            }

            // tree.generated_run.push_back(tree.heap[0].dataRecord);
            tree.pushRecordToGeneratedRun(tree.heap[0].dataRecord);

            tree.heap[0].dataRecord = NULL;
            if (tree.generated_run.size() * ON_DISK_RECORD_SIZE >=
                hdd.pageSize) {
                vector<DataRecord *> records = tree.generated_run;

                begin_time = clock();
                hdd.spillRecordsToDisk(!isFinal, records, hddRunIndex);
                time_spent_us = float(clock() - begin_time) * 1000 * 1000 / CLOCKS_PER_SEC;
                totalLatency += time_spent_us;
                tree.generated_run.clear();
                records.clear();
            }
        }

        vector<DataRecord *> records = tree.generated_run;
        begin_time = clock();
        hdd.spillRecordsToDisk(!isFinal, records, hddRunIndex);
        time_spent_us = float(clock() - begin_time) * 1000 * 1000 / CLOCKS_PER_SEC;
        totalLatency += time_spent_us;
        tree.generated_run.clear();
    } else if (recordDetailsLists.size() == 1) {
        // Only one run in SSD;
        // records = recordDetailsLists[0];
        // hdd.spillRecordsToDisk(false, records);
        std::filesystem::path currentDir = std::filesystem::current_path();
        string ssdRunPath = currentDir.string() + "/" +
                            ssd.device_path + "/sorted/sorted_run_1";
        string hddRunPath = currentDir.string() + "/" +
                            hdd.device_path + "/output";

        if (access(ssdRunPath.c_str(), F_OK) == 0) {
            rename(ssdRunPath.c_str(), hddRunPath.c_str());
        }
    }

    return;
}

void hddRuns(vector<RecordDetails *> runsLeftInMemoryFinal, StorageDevice &ssd,
             StorageDevice &hdd) {
    uint hdd_bandwidth = 100 * 1024 * 1024;
    double hdd_latency = 5 * 1e-3;
    uint hdd_page_num_records = hdd.pageSize / ON_DISK_RECORD_SIZE;
    int ssd_page_num_records = ssd.pageSize / ON_DISK_RECORD_SIZE;

    int num_records = 0;
    vector<RecordDetails *> recordDetailsLists;

    long long int time_spent_us;
	clock_t begin_time;

    begin_time = clock();
    recordDetailsLists = ssd.getRecordsFromRunsOnDisk(hdd_page_num_records);
    time_spent_us = float(clock() - begin_time) * 1000 * 1000 / CLOCKS_PER_SEC;
    totalLatency += time_spent_us;

    begin_time = clock();
    auto recordDetailsHDD = hdd.getRecordsFromRunsOnDisk(ssd_page_num_records);
    time_spent_us = float(clock() - begin_time) * 1000 * 1000 / CLOCKS_PER_SEC;
    totalLatency += time_spent_us;

    recordDetailsLists.insert(recordDetailsLists.end(),
                              recordDetailsHDD.begin(), recordDetailsHDD.end());
    recordDetailsLists.insert(recordDetailsLists.end(),
                              runsLeftInMemoryFinal.begin(),
                              runsLeftInMemoryFinal.end());
    for (auto x : recordDetailsLists) {
        num_records += x->recordLists.size();
    }
    if (num_records == 0) return;
    if (recordDetailsLists.size() > 1) {
        Tree tree = Tree(recordDetailsLists, numRecords, ssd, hdd);

        for (int i = 0; i < tree.numRecords; i++) {
            for (int inner_node_index = tree.numInnerNodes;
                 inner_node_index >= 0; inner_node_index--) {
                tree.run_tournament(inner_node_index);
            }

            // tree.generated_run.push_back(tree.heap[0].dataRecord);
            tree.pushRecordToGeneratedRun(tree.heap[0].dataRecord);

            tree.heap[0].dataRecord = NULL;
            if (tree.generated_run.size() * recordSize >= hdd.pageSize) {
                vector<DataRecord *> records = tree.generated_run;
                tree.generated_run.clear();
                begin_time = clock();
                hdd.spillRecordsToDisk(false, records, -1);
                time_spent_us = float(clock() - begin_time) * 1000 * 1000 / CLOCKS_PER_SEC;
                totalLatency += time_spent_us;
            }
        }

        vector<DataRecord *> records = tree.generated_run;
        tree.generated_run.clear();
        begin_time = clock();
        hdd.spillRecordsToDisk(false, records, -1);
        time_spent_us = float(clock() - begin_time) * 1000 * 1000 / CLOCKS_PER_SEC;
        totalLatency += time_spent_us;
    } else if (recordDetailsLists.size() == 1) {
        // Only one run in SSD;
        // records = recordDetailsLists[0];
        // hdd.spillRecordsToDisk(false, records);
        std::filesystem::path currentDir = std::filesystem::current_path();
        string ssdRunPath = currentDir.string() + "/" +
                            hdd.device_path + "/sorted/sorted_run_1";
        string hddRunPath = currentDir.string() + "/" +
                            hdd.device_path + "/output";

        if (access(ssdRunPath.c_str(), F_OK) == 0) {
            rename(ssdRunPath.c_str(), hddRunPath.c_str());
        }
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
    trace = SortTrace(trace_file);
    recordSize = recordSize * sizeof(char);
    ON_DISK_RECORD_SIZE = recordSize + 1;

    int dramSize = 100 * 1024 * 1024;
    long long int ssdSize = 10LL * 1024LL * 1024LL * 1024LL;
    long long int totalDataSize = ON_DISK_RECORD_SIZE * numRecords;

    // int availableDramSize = dramSize * 0.9;
    long long int numRecsThatCanFitInRam = dramSize / ON_DISK_RECORD_SIZE + 1;
    long long int numRecsThatCanFitInSSD = ssdSize / ON_DISK_RECORD_SIZE;
    int cacheSize = 1024 * 1024;
    int fanIn = 5000;
    long long int initialNumRecords = numRecords;
    long long int recordsGeneratedSoFar = 0;
    // long long int runSize = dramSize / fanIn;
    // long long int numRecordsPerRun = runSize / ON_DISK_RECORD_SIZE;
    long long int cacheMiniRunSize = cacheSize / ON_DISK_RECORD_SIZE + 1;

    // < 1MB
    if (totalDataSize <= cacheSize) {
        ScanPlan *const plan = new ScanPlan(numRecords, recordSize);
        vector<DataRecord *> recList = plan->GetAllRecords();

        sort(recList.begin(), recList.end(), DataRecordComparator());
        string trace_str = "STATE -> SORT_MINI_RUNS: Sort cache-size mini runs";
        trace.append_trace(trace_str);
        // WE ARE DONE
        std::filesystem::path currentDir = std::filesystem::current_path();

        string runPath =
            currentDir.string() + "/HDD/output";

        fstream runfile;
        string str_records = "";

        runfile.open(runPath, fstream::out | fstream::app);
        if (!runfile.is_open()) return 0;

        for (uint ii = 0; ii < recList.size(); ii++) {
            DataRecord *record = recList[ii];
            string str_record = record->getRecord();
            str_record[str_record.size() - 1] = '\n';
            str_records += str_record;
        }
        runfile << str_records;
        runfile.close();
    } else if (totalDataSize <= dramSize) {
        vector<RecordDetails *> runsInMemory;
        while (initialNumRecords > 0) {
            int recordsToGenerate =
                std::min(initialNumRecords, cacheMiniRunSize);
            initialNumRecords -= recordsToGenerate;
            recordsGeneratedSoFar += recordsToGenerate;
            ScanPlan *const plan = new ScanPlan(recordsToGenerate, recordSize);
            vector<DataRecord *> recList = plan->GetAllRecords();

            sort(recList.begin(), recList.end(), DataRecordComparator());
            string trace_str =
                "STATE -> SORT_MINI_RUNS: Sort cache-size mini runs";

            trace.append_trace(trace_str);
            RecordDetails *recordDetails = new RecordDetails;
            recordDetails->recordLists = recList;
            recordDetails->runPath = "";
            recordDetails->deviceType = Type::DRAM;
            runsInMemory.push_back(recordDetails);
        }
        dramRuns(runsInMemory, hdd, true, -1);
    } else if (totalDataSize <= ssdSize) {
        long long int numSSDRuns = (totalDataSize / dramSize) + 1;
        long long int numSSDIterations = numSSDRuns;
        int ssdRunIndex = 1;
        long long int dataToSpillFirst = totalDataSize % dramSize;
        long long int initialRecordsToSpill =
            dataToSpillFirst / ON_DISK_RECORD_SIZE;
        vector<RecordDetails *> runsLeftInMemory;
        vector<ScanPlan *> toDelete;
        while (initialNumRecords > 0) {
            vector<RecordDetails *> runsInMemory;
            long long int dramInitialNumRecords = 0;
            if (initialRecordsToSpill == 0) {
                dramInitialNumRecords =
                    std::min(numRecsThatCanFitInRam, initialNumRecords);
            } else {
                dramInitialNumRecords = initialRecordsToSpill;
                initialRecordsToSpill = 0;
            }
            initialNumRecords -= dramInitialNumRecords;
            while (dramInitialNumRecords > 0) {
                int recordsToGenerate =
                    std::min(dramInitialNumRecords, cacheMiniRunSize);
                dramInitialNumRecords -= recordsToGenerate;
                recordsGeneratedSoFar += recordsToGenerate;
                ScanPlan *const plan =
                    new ScanPlan(recordsToGenerate, recordSize);
                toDelete.push_back(plan);
                vector<DataRecord *> recList = plan->GetAllRecords();

                sort(recList.begin(), recList.end(), DataRecordComparator());
                string trace_str =
                    "STATE -> SORT_MINI_RUNS: Sort cache-size mini runs";

                trace.append_trace(trace_str);
                RecordDetails *recordDetails = new RecordDetails;
                recordDetails->recordLists = recList;
                recordDetails->runPath = "";
                recordDetails->deviceType = Type::DRAM;
                runsInMemory.push_back(recordDetails);
                recList.clear();
            }
            if (numSSDIterations > 1) {
                dramRuns(runsInMemory, ssd, false, ssdRunIndex);
                for (int i = 0; i < runsInMemory.size(); i++) {
                    delete runsInMemory[i];
                }
                runsInMemory.clear();
                for (auto p : toDelete) {
                    delete p;
                }
                toDelete.clear();
                ssdRunIndex++;
                numSSDIterations--;
            } else {
                runsLeftInMemory = runsInMemory;
            }
        }
        ssdRuns(runsLeftInMemory, ssd, hdd, true, -1, numRecords);
    } else if (totalDataSize > ssdSize) {
        long long int numHDDRuns = (totalDataSize / ssdSize) + 1;
        long long int numHDDIterations = numHDDRuns;
        long long int dataToSpillFirstHDD = totalDataSize % ssdSize;
        long long int initialRecordsToSpillHDD =
            dataToSpillFirstHDD / ON_DISK_RECORD_SIZE;
        int hddRunIndex = 1;
        vector<RecordDetails *> runsLeftInMemoryFinal;
        vector<RecordDetails *> runsLeftInMemorySSD;
        while (initialNumRecords > 1) {
            int ssdRunIndex = 1;
            vector<ScanPlan *> toDelete;
            long long int ssdInitialNumRecords = 0;
            if (initialRecordsToSpillHDD == 0) {
                ssdInitialNumRecords =
                    std::min(numRecsThatCanFitInSSD, initialNumRecords);
            } else {
                ssdInitialNumRecords = initialRecordsToSpillHDD;
                initialRecordsToSpillHDD = 0;
            }
            long long int recordsToPutInTree = ssdInitialNumRecords;
            initialNumRecords -= ssdInitialNumRecords;
            long long int ssdDataSize =
                ssdInitialNumRecords * ON_DISK_RECORD_SIZE;
            long long int numSSDIterations = (ssdDataSize) / dramSize + 1;
            long long int dataToSpillFirstSSD = ssdDataSize % dramSize;
            long long int initialRecordsToSpillSSD =
                dataToSpillFirstSSD / ON_DISK_RECORD_SIZE;
            while (ssdInitialNumRecords > 0) {
                vector<RecordDetails *> runsInMemory;
                long long int dramInitialNumRecords = 0;
                if (initialRecordsToSpillSSD == 0) {
                    dramInitialNumRecords =
                        std::min(numRecsThatCanFitInRam, ssdInitialNumRecords);
                } else {
                    dramInitialNumRecords = initialRecordsToSpillSSD;
                    initialRecordsToSpillSSD = 0;
                }

                ssdInitialNumRecords -= dramInitialNumRecords;
                while (dramInitialNumRecords > 0) {
                    int recordsToGenerate =
                        std::min(dramInitialNumRecords, cacheMiniRunSize);
                    dramInitialNumRecords -= recordsToGenerate;
                    recordsGeneratedSoFar += recordsToGenerate;
                    ScanPlan *const plan =
                        new ScanPlan(recordsToGenerate, recordSize);
                    toDelete.push_back(plan);
                    vector<DataRecord *> recList = plan->GetAllRecords();

                    sort(recList.begin(), recList.end(),
                         DataRecordComparator());
                    string trace_str =
                        "STATE -> SORT_MINI_RUNS: Sort cache-size mini runs";

                    trace.append_trace(trace_str);
                    RecordDetails *recordDetails = new RecordDetails;
                    recordDetails->recordLists = recList;
                    recordDetails->runPath = "";
                    recordDetails->deviceType = Type::DRAM;
                    runsInMemory.push_back(recordDetails);
                    recList.clear();
                }
                if (numSSDIterations > 1) {
                    dramRuns(runsInMemory, ssd, false, ssdRunIndex);
                    for (int i = 0; i < runsInMemory.size(); i++) {
                        delete runsInMemory[i];
                    }
                    runsInMemory.clear();
                    for (auto p : toDelete) {
                        delete p;
                    }
                    toDelete.clear();
                    ssdRunIndex++;
                    numSSDIterations--;
                } else {
                    runsLeftInMemorySSD = runsInMemory;
                }
            }
            if (numHDDIterations > 1) {
                ssdRuns(runsLeftInMemorySSD, ssd, hdd, false, hddRunIndex,
                        recordsToPutInTree);
                hddRunIndex++;
                numHDDIterations--;
            } else {
                runsLeftInMemoryFinal = runsLeftInMemorySSD;
            }
        }
        hddRuns(runsLeftInMemoryFinal, ssd, hdd);
    }

    cout<<"Total latency while making accesses: " + to_string(totalLatency);
    cout << "Stats for SSD Device:" << endl;
    ssd.get_device_access_stats();
    cout << endl;
    cout << "Stats for HDD Device:" << endl;
    hdd.get_device_access_stats();

    return 0;
}
