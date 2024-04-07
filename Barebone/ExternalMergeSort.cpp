#include <string.h>
#include <unistd.h>  // For getopt

#include <iostream>
#include <string>

#include <DataRecord.h>
#include <Tree.h>
#include <StorageDevice.h>

using namespace std;

int main(int argc, char* argv[]) {
    int opt;
    int numRecords = 0;  // Initialize to default values
    int recordSize = 0;
    string trace_file = "trace";

    // Process command-line arguments using getopt
    while ((opt = getopt(argc, argv, "c:s:o:")) !=
           -1) {  // Added ':' for option 'o' to indicate it expects an argument
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


    return 0;
}

void ssdRuns(StorageDevice &ssd, StorageDevice &hdd)
{
    uint ssd_bandwidth = 100 * 1024 * 1024;
    uint ssd_latency = 1 / (10 * 1000);
	uint ssd_page_num_records = (ssd_bandwidth * ssd_latency) / (3 + 2 + 4*4);

	while (ssd.getTotalRuns()) {
		int num_records;
		vector<DataRecord*> records;
		vector<vector<DataRecord*> > record_lists;

		record_lists = ssd.getRecordsFromRunsOnDisk(ssd_page_num_records);
		if (record_lists.size() > 1) {
			Tree tree = Tree(record_lists, num_records, false);

			tree.generateSortedRun();

			records = tree.generated_run;
		}
		hdd.spillRecordsToDisk(false, records);
	}

	return;
}

void hddRuns(StorageDevice &ssd, StorageDevice &hdd)
{
    uint hdd_bandwidth = 100 * 1024 * 1024;
    uint hdd_latency = 1 * 10 / 1000 ;
	uint hdd_page_num_records = (hdd_bandwidth * hdd_latency) / (3 + 2 + 4*4);

	while (hdd.getTotalRuns())
	{
		vector<vector<DataRecord*> > record_lists;

		record_lists = hdd.getRecordsFromRunsOnDisk(hdd_page_num_records);

		ssd.spillRecordListToDisk(record_lists);
		ssdRuns(ssd, hdd);
	}

	return;
}
