#include "disk_manager.h"
#include "table.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

std::atomic<int> DiskManager::in_io_count(0);
std::atomic<int> DiskManager::out_io_count(0);

DiskManager::DiskManager(const std::string &data_dir)
    : data_directory(data_dir) {
  ensure_data_directory();
}

std::string DiskManager::get_table_filename(const std::string &table_name) {
  return data_directory + table_name + ".dat";
}

void DiskManager::ensure_data_directory() {
  std::filesystem::create_directories(data_directory);
}

std::shared_ptr<Page> DiskManager::read_page(const std::string &table_name,
                                             int page_id) {

  std::string filename = get_table_filename(table_name);
  std::ifstream file(filename, std::ios::binary);

  if (!file.is_open()) {
    // File doesn't exist, return empty page
    return std::make_shared<Page>(page_id);
  }

  auto page = std::make_shared<Page>(page_id);

  // Calculate file position for this page
  file.seekg(0, std::ios::end);
  std::streamsize file_size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::string line;
  int current_page = 0;
  int rows_in_current_page = 0;

  while (std::getline(file, line) && current_page <= page_id) {
    if (line.empty())
      continue;

    if (current_page == page_id) {
      // Parse the line and add to page
      std::vector<std::string> columns;
      std::stringstream ss(line);
      std::string token;

      while (std::getline(ss, token, '|')) {
        columns.push_back(token);
      }

      if (!columns.empty()) {
        Row row(columns);
        page->add_row(row);
      }
    }

    rows_in_current_page++;
    if (rows_in_current_page >= Page::MAX_ROWS) {
      current_page++;
      // increments io count for each page read
      increment_in_io_count();
      rows_in_current_page = 0;
    }
  }

  file.close();
  page->dirty = false;
  return page;
}

void DiskManager::write_page(const std::string &table_name,
                             std::shared_ptr<Page> page) {
  increment_out_io_count();

  std::string filename = get_table_filename(table_name);

  std::vector<std::string> all_lines;

  std::ifstream read_file(filename);
  if (read_file.is_open()) {
    std::string line;
    while (std::getline(read_file, line)) {
      all_lines.push_back(line);
    }
    read_file.close();
  }

  int start_line = page->page_id * Page::MAX_ROWS;

  // Ensure we have enough lines
  while (all_lines.size() < start_line + Page::MAX_ROWS) {
    all_lines.push_back("");
  }

  // Replace lines for this page
  for (size_t i = 0; i < page->rows.size(); ++i) {
    std::stringstream ss;
    const Row &row = page->rows[i];

    for (size_t j = 0; j < row.size(); ++j) {
      if (j > 0)
        ss << "|";
      ss << row[j];
    }

    all_lines[start_line + i] = ss.str();
  }

  // Clear remaining lines in this page
  for (size_t i = page->rows.size(); i < Page::MAX_ROWS; ++i) {
    if (start_line + i < all_lines.size()) {
      all_lines[start_line + i] = "";
    }
  }

  // Write back to file
  std::ofstream write_file(filename);
  if (!write_file.is_open()) {
    throw std::runtime_error("Cannot write to file: " + filename);
  }

  for (const std::string &line : all_lines) {
    if (!line.empty()) {
      write_file << line << std::endl;
    }
  }

  write_file.close();
  page->dirty = false;
}

bool DiskManager::table_file_exists(const std::string &table_name) {
  std::string filename = get_table_filename(table_name);
  std::ifstream file(filename);
  return file.good();
}

void DiskManager::create_table_file(const std::string &table_name) {
  std::string filename = get_table_filename(table_name);
  std::ofstream file(filename);
  file.close();
}

int DiskManager::get_total_pages(const std::string &table_name) {
  std::string filename = get_table_filename(table_name);
  std::ifstream file(filename);

  if (!file.is_open()) {
    return 0;
  }

  int line_count = 0;
  std::string line;

  while (std::getline(file, line)) {
    if (!line.empty()) {
      line_count++;
    }
  }

  file.close();

  // Calculate number of pages needed
  return (line_count + Page::MAX_ROWS - 1) / Page::MAX_ROWS;
}
