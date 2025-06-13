#ifndef JOIN_OPERATIONS_H
#define JOIN_OPERATIONS_H

#include <memory>
#include <string>
#include <vector>

class Table;
class BufferManager;
class Row;

namespace JoinOperations {

struct JoinResult {
  std::vector<Row> result_rows;
  std::vector<std::string> result_columns;
  int total_io_operations;

  JoinResult() : total_io_operations(0) {}
};

JoinResult sort_merge_join(std::shared_ptr<Table> left_table,
                           std::shared_ptr<Table> right_table,
                           const std::string &left_column,
                           const std::string &right_column,
                           std::shared_ptr<BufferManager> buffer_manager);

// Helper functions for sort-merge join
std::shared_ptr<Table>
external_sort(std::shared_ptr<Table> table, const std::string &sort_column,
              std::shared_ptr<BufferManager> buffer_manager);

void merge_sorted_runs(const std::vector<std::string> &run_files,
                       const std::string &output_file,
                       const std::string &table_name, int sort_column_index,
                       std::shared_ptr<BufferManager> buffer_manager);

std::vector<std::string>
create_sorted_runs(std::shared_ptr<Table> table, int sort_column_index,
                   std::shared_ptr<BufferManager> buffer_manager);

Row merge_rows(const Row &left_row, const Row &right_row);

// Utility functions
int compare_values(const std::string &a, const std::string &b);
std::string generate_temp_filename(const std::string &prefix);

void write_join_result_to_file(const JoinResult &result,
                               std::shared_ptr<BufferManager> buffer_manager,
                               const std::string &left_table_name,
                               const std::string &right_table_name);
} // namespace JoinOperations

#endif // JOIN_OPERATIONS_H
