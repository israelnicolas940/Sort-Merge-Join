#include "parser.h"
#include "buffer_manager.h"
#include "table.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

std::vector<std::string> CSVParser::split_csv_line(const std::string &line) {
  std::vector<std::string> tokens;
  std::stringstream ss(line);
  std::string token;

  while (std::getline(ss, token, ',')) {
    tokens.push_back(trim(token));
  }

  return tokens;
}

std::string CSVParser::trim(const std::string &str) {
  size_t first = str.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) {
    return "";
  }

  size_t last = str.find_last_not_of(" \t\r\n");
  return str.substr(first, (last - first + 1));
}

std::shared_ptr<Table>
CSVParser::parse_uva_csv(const std::string &filename,
                         std::shared_ptr<BufferManager> buffer_manager) {
  std::vector<std::string> columns = {"uva_id", "nome", "tipo", "ano_colheita",
                                      "pais_origem_id"};
  return parse_csv(filename, "Uva", columns, buffer_manager);
}

std::shared_ptr<Table>
CSVParser::parse_vinho_csv(const std::string &filename,
                           std::shared_ptr<BufferManager> buffer_manager) {
  std::vector<std::string> columns = {"vinho_id", "rotulo", "ano_producao",
                                      "uva_id", "pais_producao_id"};
  return parse_csv(filename, "Vinho", columns, buffer_manager);
}

std::shared_ptr<Table>
CSVParser::parse_pais_csv(const std::string &filename,
                          std::shared_ptr<BufferManager> buffer_manager) {
  std::vector<std::string> columns = {"pais_id", "nome", "sigla"};
  return parse_csv(filename, "Pais", columns, buffer_manager);
}

std::shared_ptr<Table>
CSVParser::parse_csv(const std::string &filename, const std::string &table_name,
                     const std::vector<std::string> &expected_columns,
                     std::shared_ptr<BufferManager> buffer_manager) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open CSV file: " + filename);
  }

  auto table =
      std::make_shared<Table>(table_name, expected_columns, buffer_manager);

  std::string line;
  bool first_line = true;
  int current_page_id = 0;
  auto current_page = std::make_shared<Page>(current_page_id);

  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    if (first_line) {
      // Validate header line
      auto header_tokens = split_csv_line(line);

      if (header_tokens.size() != expected_columns.size()) {
        std::cerr << "Warning: Header has " << header_tokens.size()
                  << " columns, expected " << expected_columns.size()
                  << std::endl;
      }

      // Optional: Check if header matches expected columns
      for (size_t i = 0;
           i < std::min(header_tokens.size(), expected_columns.size()); ++i) {
        if (header_tokens[i] != expected_columns[i]) {
          std::cerr << "Warning: Column " << i << " header '"
                    << header_tokens[i] << "' doesn't match expected '"
                    << expected_columns[i] << "'" << std::endl;
        }
      }

      first_line = false;
      continue;
    }

    // Parse data line
    auto tokens = split_csv_line(line);

    if (tokens.size() != expected_columns.size()) {
      std::cerr << "Warning: Row has " << tokens.size() << " columns, expected "
                << expected_columns.size() << std::endl;
      continue;
    }

    Row row(tokens);

    if (current_page->is_full()) {
      // Write current page and create new one
      table->write_page(current_page);
      current_page_id++;
      current_page = std::make_shared<Page>(current_page_id);
    }

    current_page->add_row(row);
  }

  // Write the last page if it has data
  if (!current_page->rows.empty()) {
    table->write_page(current_page);
    current_page_id++;
  }

  table->set_total_pages(current_page_id);
  file.close();

  return table;
}
