#ifndef EXTERNAL_SORT_H
#define EXTERNAL_SORT_H

#include "data_structures.h"

// Sort table using external merge sort
// Sort table using external merge sort
Table external_merge_sort_table(const Table &table, int sort_column_index);

bool compare_rows_by_column(const Row &a, const Row &b, int sort_column_index);
#endif
