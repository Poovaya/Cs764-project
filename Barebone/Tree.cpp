#include "Tree.h"

#include <iostream>
#include <vector>
using namespace std;
DataRecord *Tree::popRecordFromLeafList(Node *node) {
    int numRecsInNode = node->sortedRun.size();
    if (node->sortedRunIndex >= numRecsInNode) {
        return NULL;
    }
    DataRecord *dataRecord =
        new DataRecord(node->sortedRun[node->sortedRunIndex]);
    node->sortedRunIndex++;
    return dataRecord;
}

void Tree::checkforEmptyNode(Node *node) {
    int numRecsInNode = node->sortedRun.size();
    if (node->sortedRunIndex >= numRecsInNode) {
        node->isEmpty = true;
    }
}

DataRecord *Tree::getTopRecordFromLeafList(Node *node) {
    int numRecsInNode = node->sortedRun.size();
    if (node->sortedRunIndex >= numRecsInNode) {
        return NULL;
    }
    DataRecord *dataRecord =
        new DataRecord(node->sortedRun[node->sortedRunIndex]);
    return dataRecord;
}

Tree::Tree(vector<vector<DataRecord>> &recordList) {
    this->numRuns = recordList.size();
    this->numLeaves = this->numRuns;
    this->numInnerNodes = (this->numLeaves % 2 + this->numLeaves / 2) * 2 - 1;
    cout << this->numInnerNodes << " INNER NODES" << endl;

    this->numNodes = numLeaves + this->numInnerNodes;
    this->heap = new struct Node[this->numNodes];

    for (int i = 0; i < this->numNodes; i++) {
        heap[i].dataRecord = NULL;
        heap[i].isEmpty = true;
        heap[i].isLeaf = false;
        heap[i].sortedRun = {};
        heap[i].sortedRunIndex = 0;
        heap[i].dataIndex = 0;
    }

    for (int i = numInnerNodes; i < numNodes; i++) {
        heap[i].sortedRun = recordList[i - numInnerNodes];
        heap[i].isLeaf = true;
        heap[i].isEmpty = false;
        heap[i].dataIndex = i;
        heap[i].sortedRunIndex = 0;
    }
}

void Tree::run_tree() {
    for (int inner_node_index = this->numInnerNodes; inner_node_index >= 0;
         inner_node_index--) {
        this->run_tournament(inner_node_index);
    }

    this->generated_run.push_back(this->heap[0].dataRecord);
    this->heap[0].dataRecord = NULL;
}

// vector<DataRecord *> Tree::getGeneratedRun() { return this->generated_run; }

void Tree::run_tournament(int parent) {
    int left_child = parent * 2 + 1;
    int right_child = parent * 2 + 2;

    struct Node *parent_node = &this->heap[parent];

    if (left_child >= this->numNodes) {
        return;
    } else if (left_child < this->numNodes && right_child >= this->numNodes) {
        // Only left child is valid, special case only valid at leaf nodes
        struct Node *left_child_node = &this->heap[left_child];
        // If parent has a valid record, it will be the winner, so skip.
        // Iteration over the next level will empty this node
        // the iteration after that for this level would fill it up.
        if (parent_node->dataRecord) {
            return;
        } else {
            if (left_child_node->isLeaf) {
                parent_node->dataRecord =
                    popRecordFromLeafList(left_child_node);
            }
        }
    }
    { /* Both the children are valid. So add the popped record at parent (if
         empty) */
        if (parent_node->dataRecord) {
            return;
        } else {
            struct Node *left_child_node = &(this->heap[left_child]);
            struct Node *right_child_node = &(this->heap[right_child]);
            DataRecord *left_data = NULL, *right_data = NULL;
            if (left_child_node->isLeaf) {
                // Both will be a leaf node
                left_data = getTopRecordFromLeafList(left_child_node);
                right_data = getTopRecordFromLeafList(right_child_node);
                if ((right_data != NULL) & (left_data != NULL)) {
                    if (DataRecord::compareDataRecords(*left_data,
                                                       *right_data)) {
                        parent_node->dataRecord =
                            popRecordFromLeafList(left_child_node);
                        checkforEmptyNode(left_child_node);
                        delete left_data;
                        delete right_data;
                        return;
                    } else {
                        // Right is the winner
                        parent_node->dataRecord =
                            popRecordFromLeafList(right_child_node);
                        checkforEmptyNode(right_child_node);
                        delete left_data;
                        delete right_data;
                        return;
                    }
                } else if (left_data) {
                    parent_node->dataRecord =
                        popRecordFromLeafList(left_child_node);
                    checkforEmptyNode(left_child_node);
                    delete left_data;
                    return;
                } else if (right_data) {
                    parent_node->dataRecord =
                        popRecordFromLeafList(right_child_node);
                    checkforEmptyNode(right_child_node);
                    delete right_data;
                    return;
                } else {
                    parent_node->dataRecord = NULL;
                    return;
                    // At this point, both of the left and right should have
                    // been reported as empty, so no need to update.
                }
            } else {
                // It is an internal node
                left_data = left_child_node->dataRecord;
                right_data = right_child_node->dataRecord;
                if (left_data && right_data) {
                    if (DataRecord::compareDataRecords(*left_data,
                                                       *right_data)) {
                        parent_node->isEmpty = false;
                        parent_node->dataRecord = left_data;
                        left_child_node->isEmpty = true;
                        left_child_node->dataRecord = NULL;
                        return;
                    } else {
                        parent_node->isEmpty = false;
                        parent_node->dataRecord = right_data;
                        right_child_node->isEmpty = true;
                        right_child_node->dataRecord = NULL;

                        return;
                    }
                } else if (left_data) {
                    parent_node->isEmpty = false;
                    parent_node->dataRecord = left_data;
                    left_child_node->isEmpty = true;
                    left_child_node->dataRecord = NULL;
                } else if (right_data) {
                    parent_node->isEmpty = false;
                    parent_node->dataRecord = right_data;
                    right_child_node->dataRecord = NULL;
                    right_child_node->isEmpty = true;

                } else {
                    parent_node->isEmpty = true;
                    parent_node->dataRecord = NULL;
                }
            }
        }
    }
}

void Tree::printSortedRun(vector<DataRecord> dr) {
    for (DataRecord d : dr) {
        d.show();
    }
}
void Tree::printHeap() {
    for (int i = 0; i < this->numNodes; i++) {
        std::cout << "Node " << i << ":" << std::endl;
        std::cout << "  DataRecord: " << heap[i].dataRecord << std::endl;
        std::cout << "  isEmpty: " << (heap[i].isEmpty ? "true" : "false")
                  << std::endl;
        std::cout << "  isLeaf: " << (heap[i].isLeaf ? "true" : "false")
                  << std::endl;
        if (heap[i].isLeaf) {
            std::cout << "PRINTING SORTED" << endl;
            printSortedRun(heap[i].sortedRun);
        }

        std::cout << "  sortedRunIndex: " << heap[i].sortedRunIndex
                  << std::endl;
        std::cout << "  dataIndex: " << heap[i].dataIndex << std::endl;
        std::cout << std::endl;
    }
}

Tree::~Tree() { delete[] this->heap; }
