#include "DataRecord.h"
#include "DeviceConstants.h"
#include "RecordDetails.h"
#include "StorageDevice.h"
#include "defs.h"

using namespace std;
class Tree {
   private:
    /* data */
   public:
    Tree();
    Tree(vector<RecordDetails *> &, int, StorageDevice &, StorageDevice &);
    Tree(vector<DataRecord> &, bool);
    ~Tree();
    void run_tournament(int);
    int numRuns;
    struct Node *heap;
    void printHeap();
    int numLeaves;
    int numNodes;
    int numInnerNodes;
    int numRecords;
    StorageDevice ssd;
    StorageDevice hdd;
    vector<DataRecord *> generated_run;
    DataRecord *popRecordFromLeafList(Node *);
    DataRecord *getTopRecordFromLeafList(Node *);
    void pushRecordToGeneratedRun(DataRecord *);
    void checkforEmptyNode(Node *);
    //  vector<DataRecord *> getGeneratedRun();

    void printSortedRun(vector<DataRecord *>);
    void fetchNextPage(Node *);
};

struct Node {
    // Only for non leaf nodes
    DataRecord *dataRecord;
    int dataIndex;

    // For leaf nodes
    vector<DataRecord *> sortedRun;
    int sortedRunIndex;
    string runPath;
    Type deviceType;
    bool isLeaf;
    bool isEmpty;
    int getDevicePageSize() {
        if (deviceType == Type::SSD) {
            return SSD_PAGE_SIZE;
        } else {
            return HDD_PAGE_SIZE;
        }
    };
};