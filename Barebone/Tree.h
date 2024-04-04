#include "DataRecord.h"
#include "defs.h"
using namespace std;
class Tree {
   private:
    /* data */
   public:
    Tree();
    Tree(vector<vector<DataRecord>> &);
    ~Tree();
    void run_tree();
    void run_tournament(int);
    int numRuns;
    struct Node *heap;
    void printHeap();
    int numLeaves;
    int numNodes;
    int numInnerNodes;
    vector<DataRecord *> generated_run;
    DataRecord *popRecordFromLeafList(Node *);
    DataRecord *getTopRecordFromLeafList(Node *);
    void checkforEmptyNode(Node *);
    //  vector<DataRecord *> getGeneratedRun();

    void printSortedRun(vector<DataRecord>);
};

struct Node {
    // Only for non leaf nodes
    DataRecord *dataRecord;
    int dataIndex;

    // For leaf nodes
    vector<DataRecord> sortedRun;
    int sortedRunIndex;
    bool isLeaf;
    bool isEmpty;
};