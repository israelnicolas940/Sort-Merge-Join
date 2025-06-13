#include "join_operation.h"
#include "buffer_manager.h"
#include "disk_manager.h"
#include "table.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <random>
#include <sstream>

namespace JoinOperations {

JoinResult sort_merge_join(std::shared_ptr<Table> left_table,
                           std::shared_ptr<Table> right_table,
                           const std::string &left_column,
                           const std::string &right_column,
                           std::shared_ptr<BufferManager> buffer_manager) {

  JoinResult result;
  int initial_in_io = DiskManager::get_in_io_count();
  int initial_out_io = DiskManager::get_out_io_count();

  // Get column indices
  int left_col_idx = left_table->get_column_index(left_column);
  int right_col_idx = right_table->get_column_index(right_column);

  if (left_col_idx == -1 || right_col_idx == -1) {
    throw std::runtime_error("Join column not found in one of the tables");
  }

  // Phase 1: Sort both tables
  std::cout << "Phase 1: Sorting tables..." << std::endl;
  auto sorted_left = external_sort(left_table, left_column, buffer_manager);
  auto sorted_right = external_sort(right_table, right_column, buffer_manager);

  // Phase 2: Merge join
  std::cout << "Phase 2: Performing merge join..." << std::endl;

  // Create result column names
  for (const std::string &col : left_table->get_column_names()) {
    result.result_columns.push_back("left_" + col);
  }
  for (const std::string &col : right_table->get_column_names()) {
    result.result_columns.push_back("right_" + col);
  }

  // Perform merge join
  auto left_iter = sorted_left->get_iterator();
  auto right_iter = sorted_right->get_iterator();

  Row left_row, right_row;
  bool left_valid = false, right_valid = false;

  // Get first rows
  if (left_iter.has_next()) {
    left_row = left_iter.next();
    left_valid = true;
  }
  if (right_iter.has_next()) {
    right_row = right_iter.next();
    right_valid = true;
  }

  while (left_valid && right_valid) {
    int cmp = compare_values(left_row[left_col_idx], right_row[right_col_idx]);

    if (cmp == 0) {
      // Match found - handle potential duplicates
      std::vector<Row> left_matches, right_matches;
      std::string join_value = left_row[left_col_idx];

      // Collect all left rows with same join value
      left_matches.push_back(left_row);
      while (left_iter.has_next()) {
        Row next_left = left_iter.next();
        if (compare_values(next_left[left_col_idx], join_value) == 0) {
          left_matches.push_back(next_left);
        } else {
          left_row = next_left;
          break;
        }
      }
      if (!left_iter.has_next() &&
          compare_values(left_matches.back()[left_col_idx], join_value) == 0) {
        left_valid = false;
      }

      // Collect all right rows with same join value
      right_matches.push_back(right_row);
      while (right_iter.has_next()) {
        Row next_right = right_iter.next();
        if (compare_values(next_right[right_col_idx], join_value) == 0) {
          right_matches.push_back(next_right);
        } else {
          right_row = next_right;
          break;
        }
      }
      if (!right_iter.has_next() &&
          compare_values(right_matches.back()[right_col_idx], join_value) ==
              0) {
        right_valid = false;
      }

      // Create cartesian product of matches
      for (const Row &l_row : left_matches) {
        for (const Row &r_row : right_matches) {
          result.result_rows.push_back(merge_rows(l_row, r_row));
        }
      }

    } else if (cmp < 0) {
      // Left value is smaller, advance left
      if (left_iter.has_next()) {
        left_row = left_iter.next();
      } else {
        left_valid = false;
      }
    } else {
      // Right value is smaller, advance right
      if (right_iter.has_next()) {
        right_row = right_iter.next();
      } else {
        right_valid = false;
      }
    }
  }

  result.total_io_operations = DiskManager::get_in_io_count() - initial_in_io +
                               DiskManager::get_out_io_count() - initial_out_io;

  std::cout << "Join completed. Result has " << result.result_rows.size()
            << " rows with " << result.total_io_operations << " I/O operations."
            << "In I/O: " << DiskManager::get_in_io_count()
            << ", Out I/O: " << DiskManager::get_out_io_count() << "."
            << std::endl;

  return result;
}

std::shared_ptr<Table>
external_sort(std::shared_ptr<Table> table, const std::string &sort_column,
              std::shared_ptr<BufferManager> buffer_manager) {

  int sort_column_index = table->get_column_index(sort_column);
  if (sort_column_index == -1) {
    throw std::runtime_error("Sort column not found: " + sort_column);
  }

  // Phase 1: Create sorted runs
  std::vector<std::string> run_files =
      create_sorted_runs(table, sort_column_index, buffer_manager);

  if (run_files.empty()) {
    return table; // Empty table
  }

  if (run_files.size() == 1) {
    // Only one run, create table from it
    auto sorted_table =
        std::make_shared<Table>(table->get_name() + "_sorted",
                                table->get_column_names(), buffer_manager);

    // Load data from run file
    std::ifstream file(run_files[0]);
    std::string line;
    int page_id = 0;
    auto current_page = std::make_shared<Page>(page_id);

    while (std::getline(file, line)) {
      if (line.empty())
        continue;

      std::vector<std::string> tokens;
      std::stringstream ss(line);
      std::string token;

      while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
      }

      if (!tokens.empty()) {
        Row row(tokens);

        if (current_page->is_full()) {
          sorted_table->write_page(current_page);
          page_id++;
          current_page = std::make_shared<Page>(page_id);
        }

        current_page->add_row(row);
      }
    }

    if (!current_page->rows.empty()) {
      sorted_table->write_page(current_page);
      page_id++;
    }

    sorted_table->set_total_pages(page_id);
    file.close();

    // Clean up temporary file
    std::remove(run_files[0].c_str());

    return sorted_table;
  }

  // Phase 2: Merge runs
  std::string output_file = generate_temp_filename("final_sorted");
  merge_sorted_runs(run_files, output_file, table->get_name() + "_sorted",
                    sort_column_index, buffer_manager);

  // Create table from merged result
  auto sorted_table = std::make_shared<Table>(
      table->get_name() + "_sorted", table->get_column_names(), buffer_manager);

  // Load data from output file
  std::ifstream file(output_file);
  std::string line;
  int page_id = 0;
  auto current_page = std::make_shared<Page>(page_id);

  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;

    while (std::getline(ss, token, '|')) {
      tokens.push_back(token);
    }

    if (!tokens.empty()) {
      Row row(tokens);

      if (current_page->is_full()) {
        sorted_table->write_page(current_page);
        page_id++;
        current_page = std::make_shared<Page>(page_id);
      }

      current_page->add_row(row);
    }
  }

  if (!current_page->rows.empty()) {
    sorted_table->write_page(current_page);
    page_id++;
  }

  sorted_table->set_total_pages(page_id);
  file.close();

  // Clean up temporary files
  std::remove(output_file.c_str());
  for (const std::string &run_file : run_files) {
    std::remove(run_file.c_str());
  }

  return sorted_table;
}

std::vector<std::string>
create_sorted_runs(std::shared_ptr<Table> table, int sort_column_index,
                   std::shared_ptr<BufferManager> buffer_manager) {

  std::vector<std::string> run_files;
  std::vector<Row> buffer;
  int run_number = 0;

  // Use 3 pages for sorting (reserve 1 page for buffer management)
  const int SORT_BUFFER_PAGES = 3;
  const int SORT_BUFFER_SIZE = SORT_BUFFER_PAGES * Page::MAX_ROWS;

  auto table_iter = table->get_iterator();

  while (table_iter.has_next()) {
    // Fill buffer
    buffer.clear();

    for (int i = 0; i < SORT_BUFFER_SIZE && table_iter.has_next(); ++i) {
      buffer.push_back(table_iter.next());
    }

    if (buffer.empty())
      break;

    // Sort buffer
    std::sort(buffer.begin(), buffer.end(),
              [sort_column_index](const Row &a, const Row &b) {
                return compare_values(a[sort_column_index],
                                      b[sort_column_index]) < 0;
              });

    // Write sorted run to file
    std::string run_filename =
        generate_temp_filename("run_" + std::to_string(run_number));
    std::ofstream run_file(run_filename);

    for (const Row &row : buffer) {
      for (size_t j = 0; j < row.size(); ++j) {
        if (j > 0)
          run_file << "|";
        run_file << row[j];
      }
      run_file << std::endl;
    }

    run_file.close();
    run_files.push_back(run_filename);
    run_number++;
  }

  return run_files;
}

void merge_sorted_runs(const std::vector<std::string> &run_files,
                       const std::string &output_file,
                       const std::string &table_name, int sort_column_index,
                       std::shared_ptr<BufferManager> buffer_manager) {

  // Priority queue for k-way merge
  struct RunEntry {
    Row row;
    int run_id;
  };

  // Lambda comparator for priority queue
  auto cmp = [sort_column_index](const RunEntry &a, const RunEntry &b) {
    return compare_values(a.row[sort_column_index], b.row[sort_column_index]) >
           0;
  };

  std::priority_queue<RunEntry, std::vector<RunEntry>, decltype(cmp)> pq(cmp);
  std::vector<std::ifstream> run_streams(run_files.size());

  // Open all run files and initialize priority queue
  for (size_t i = 0; i < run_files.size(); ++i) {
    run_streams[i].open(run_files[i]);

    std::string line;
    if (std::getline(run_streams[i], line) && !line.empty()) {
      std::vector<std::string> tokens;
      std::stringstream ss(line);
      std::string token;

      while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
      }

      if (!tokens.empty()) {
        RunEntry entry;
        entry.row = Row(tokens);
        entry.run_id = i;
        pq.push(entry);
      }
    }
  }

  // Merge runs
  std::ofstream output(output_file);

  while (!pq.empty()) {
    RunEntry min_entry = pq.top();
    pq.pop();

    // Write to output
    for (size_t j = 0; j < min_entry.row.size(); ++j) {
      if (j > 0)
        output << "|";
      output << min_entry.row[j];
    }
    output << std::endl;

    // Read next row from the same run
    std::string line;
    if (std::getline(run_streams[min_entry.run_id], line) && !line.empty()) {
      std::vector<std::string> tokens;
      std::stringstream ss(line);
      std::string token;

      while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
      }

      if (!tokens.empty()) {
        RunEntry entry;
        entry.row = Row(tokens);
        entry.run_id = min_entry.run_id;
        pq.push(entry);
      }
    }
  }

  // Close all files
  output.close();
  for (auto &stream : run_streams) {
    stream.close();
  }
}

Row merge_rows(const Row &left_row, const Row &right_row) {
  Row result;
  result.resize(left_row.size() + right_row.size());

  size_t idx = 0;
  for (size_t i = 0; i < left_row.size(); ++i) {
    result[idx++] = left_row[i];
  }
  for (size_t i = 0; i < right_row.size(); ++i) {
    result[idx++] = right_row[i];
  }

  return result;
}

int compare_values(const std::string &a, const std::string &b) {
  // Try numeric comparison first
  try {
    double num_a = std::stod(a);
    double num_b = std::stod(b);

    if (num_a < num_b)
      return -1;
    if (num_a > num_b)
      return 1;
    return 0;
  } catch (...) {
    // Fall back to string comparison
    if (a < b)
      return -1;
    if (a > b)
      return 1;
    return 0;
  }
}

std::string generate_temp_filename(const std::string &prefix) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> dis(1000, 9999);

  return "temp_" + prefix + "_" + std::to_string(dis(gen)) + ".tmp";
}

void write_join_result_to_file(const JoinResult &result,
                               std::shared_ptr<BufferManager> buffer_manager,
                               const std::string &left_table_name,
                               const std::string &right_table_name) {
  // Compose output table name
  std::string output_table_name =
      left_table_name + "_" + right_table_name + "_join";
  auto output_table = std::make_shared<Table>(
      output_table_name, result.result_columns, buffer_manager);

  int page_id = 0;
  auto current_page = std::make_shared<Page>(page_id);

  for (const Row &row : result.result_rows) {
    if (current_page->is_full()) {
      output_table->write_page(current_page);
      page_id++;
      current_page = std::make_shared<Page>(page_id);
    }
    current_page->add_row(row);
  }

  if (!current_page->rows.empty()) {
    output_table->write_page(current_page);
    page_id++;
  }

  output_table->set_total_pages(page_id);
}

} // namespace JoinOperations
