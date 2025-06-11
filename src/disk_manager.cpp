#include "disk_manager.h"
#include <fstream>
#include <sstream>

Table load_table_from_disk(const std::string &filename) {
  Table table;
  return table;
}

void save_table_to_disk(const Table &table, const std::string &filename) {}

int read_file(const std::string &filename,
              std::vector<std::vector<std::string>> &rows, int &total_rows) {
  std::ifstream infile(filename);
  if (!infile.is_open()) {
    return ERR;
  }
  std::string line;
  total_rows = 0;

  std::getline(infile, line);

  while (std::getline(infile, line)) {
    std::stringstream ss(line);
    std::string cell;
    std::vector<std::string> row;
    while (std::getline(ss, cell, ',')) {
      row.push_back(cell);
    }
    rows.push_back(row);
    ++total_rows;
    if (total_rows >= 10)
      break;
  }
  return NO_ERR;
}

std::vector<Page> load_pages_from_disk(const std::string &filename,
                                       int page_num, int num_columns,
                                       int &total_pages) {
  std::vector<Page> pages;

  return pages;
}

std::vector<Page> load_pages_from_disk(const std::string &filename,
                                       int page_num,
                                       std::vector<std::string> schema,
                                       int &total_pages) {
  std::vector<Page> pages;
  return pages;
}

Page load_page_from_disk(const std::string &filename, int page_num,
                         int num_columns) {
  Page page;
  return page;
}

Page load_page_from_disk(const std::string &filename, int page_num,
                         std::vector<std::string> schema) {
  Page page;
  return page;
}

void save_page_to_disk(const std::string &filename, int page_num,
                       const Page &page) {}

void save_pages_to_disk(const std::string &filename, int page_num,
                        const std::vector<Page> &pages) {}

Table create_merged_table(const Table &left_table, const Table &right_table) {
  Table merged_table;
  // This function should merge the two tables based on some criteria
  // For now, we will return an empty table
  return merged_table;
}
