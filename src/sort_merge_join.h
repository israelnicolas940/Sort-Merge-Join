#ifndef SORT_MERGE_JOIN_H
#define SORT_MERGE_JOIN_H

#include "data_structures.h"

// Main join function
Table sort_merge_join(const Table &table1, int attr_index1, const Table &table2,
                      int attr_index2);

// Helper: Merge matching tuples
void merge_matching_tuples(Table &output, const std::vector<Row> &group1,
                           const std::vector<Row> &group2, int attr_index1,
                           int attr_index2);

#endif
