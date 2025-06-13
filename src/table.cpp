#include "table.h"
#include "buffer_manager.h"
#include <fstream>
#include <iostream>
#include <sstream>

// Page implementation
void Page::add_row(const Row &row) {
  if (!is_full()) {
    rows.push_back(row);
    dirty = true;
  }
}

void Page::clear() {
  rows.clear();
  dirty = false;
}

// Table implementation
Table::Table(const std::string &name, const std::vector<std::string> &columns,
             std::shared_ptr<BufferManager> bm)
    : table_name(name), column_names(columns), buffer_manager(bm),
      total_pages(0) {

  // Build column index map
  for (size_t i = 0; i < column_names.size(); ++i) {
    column_index_map[column_names[i]] = i;
  }
}

// void Table::load_from_csv(const std::string &filename) {
//   std::ifstream file(filename);
//   if (!file.is_open()) {
//     throw std::runtime_error("Cannot open file: " + filename);
//   }
//
//   std::string line;
//   bool first_line = true;
//   int current_page_id = 0;
//   auto current_page = std::make_shared<Page>(current_page_id);
//
//   while (std::getline(file, line)) {
//     if (first_line) {
//       first_line = false;
//       continue; // Skip header line
//     }
//
//     // Parse CSV line
//     std::vector<std::string> tokens;
//     std::stringstream ss(line);
//     std::string token;
//
//     while (std::getline(ss, token, ',')) {
//       // Trim whitespace
//       token.erase(0, token.find_first_not_of(" \t\r\n"));
//       token.erase(token.find_last_not_of(" \t\r\n") + 1);
//       tokens.push_back(token);
//     }
//
//     if (tokens.size() != column_names.size()) {
//       std::cerr << "Warning: Row has " << tokens.size() << " columns,
//       expected "
//                 << column_names.size() << std::endl;
//       continue;
//     }
//
//     Row row(tokens);
//
//     if (current_page->is_full()) {
//       // Write current page and create new one
//       write_page(current_page);
//       current_page_id++;
//       current_page = std::make_shared<Page>(current_page_id);
//     }
//
//     current_page->add_row(row);
//   }
//
//   // Write the last page if it has data
//   if (!current_page->rows.empty()) {
//     write_page(current_page);
//     current_page_id++;
//   }
//
//   total_pages = current_page_id;
//   file.close();
// }

int Table::get_column_index(const std::string &column_name) const {
  auto it = column_index_map.find(column_name);
  if (it != column_index_map.end()) {
    return static_cast<int>(it->second);
  }
  return -1;
}

std::shared_ptr<Page> Table::get_page(int page_id) {
  return buffer_manager->get_page(table_name, page_id);
}

void Table::write_page(std::shared_ptr<Page> page) {
  buffer_manager->write_page(table_name, page);
}

// Iterator implementation
Table::Iterator::Iterator(Table *t, int page, size_t row)
    : table(t), current_page(page), current_row(row),
      current_page_ptr(nullptr) {
  if (current_page < table->get_total_pages()) {
    current_page_ptr = table->get_page(current_page);
  }
}

bool Table::Iterator::has_next() {
  if (current_page >= table->get_total_pages()) {
    return false;
  }

  if (!current_page_ptr) {
    current_page_ptr = table->get_page(current_page);
  }

  return current_row < current_page_ptr->rows.size();
}

Row Table::Iterator::next() {
  if (!has_next()) {
    throw std::runtime_error("No more rows");
  }

  Row result = current_page_ptr->rows[current_row];
  current_row++;

  // Check if we need to move to next page
  if (current_row >= current_page_ptr->rows.size()) {
    current_page++;
    current_row = 0;
    current_page_ptr = nullptr;

    if (current_page < table->get_total_pages()) {
      current_page_ptr = table->get_page(current_page);
    }
  }

  return result;
}

void Table::Iterator::reset() {
  current_page = 0;
  current_row = 0;
  current_page_ptr = nullptr;

  if (current_page < table->get_total_pages()) {
    current_page_ptr = table->get_page(current_page);
  }
}
