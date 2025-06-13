#include "buffer_manager.h"
#include "disk_manager.h"
#include "join_operation.h"
#include "parser.h"
#include "table.h"
#include <iomanip>
#include <iostream>
#include <memory>

void print_join_result(const JoinOperations::JoinResult &result) {
  std::cout << "\n=== JOIN RESULT ===" << std::endl;
  std::cout << "Total I/O Operations: " << result.total_io_operations
            << std::endl;
  std::cout << "Result Rows: " << result.result_rows.size() << std::endl;
  std::cout << "\nColumns: ";
  for (size_t i = 0; i < result.result_columns.size(); ++i) {
    if (i > 0)
      std::cout << " | ";
    std::cout << std::setw(15) << result.result_columns[i];
  }
  std::cout << std::endl;

  std::cout << std::string(result.result_columns.size() * 18, '-') << std::endl;

  // Print first 10 rows
  size_t rows_to_print = std::min(result.result_rows.size(), size_t(10));
  for (size_t i = 0; i < rows_to_print; ++i) {
    const Row &row = result.result_rows[i];
    for (size_t j = 0; j < row.size() && j < result.result_columns.size();
         ++j) {
      if (j > 0)
        std::cout << " | ";
      std::cout << std::setw(15) << row[j];
    }
    std::cout << std::endl;
  }

  if (result.result_rows.size() > rows_to_print) {
    std::cout << "... (" << (result.result_rows.size() - rows_to_print)
              << " more rows)" << std::endl;
  }
  std::cout << std::endl;
}

int main() {
  try {
    std::cout << "=== SIMULATED DBMS SORT-MERGE JOIN ===" << std::endl;
    std::cout << "Buffer Size: 4 pages, Page Size: 10 rows" << std::endl;

    auto disk_manager = std::make_shared<DiskManager>("data/");
    auto buffer_manager = std::make_shared<BufferManager>(disk_manager);

    DiskManager::reset_io_count();

    std::cout << "\n1. Loading tables from CSV files..." << std::endl;

    // Load tables
    auto uva_table = CSVParser::parse_uva_csv("./data/uva.csv", buffer_manager);
    std::cout << "Loaded Uva table: " << uva_table->get_total_pages()
              << " pages" << std::endl;

    auto vinho_table =
        CSVParser::parse_vinho_csv("./data/vinho.csv", buffer_manager);
    std::cout << "Loaded Vinho table: " << vinho_table->get_total_pages()
              << " pages" << std::endl;

    auto pais_table =
        CSVParser::parse_pais_csv("./data/pais.csv", buffer_manager);
    std::cout << "Loaded Pais table: " << pais_table->get_total_pages()
              << " pages" << std::endl;

    std::cout << "Total In I/O operations for loading: "
              << DiskManager::get_in_io_count() << std::endl;
    std::cout << "Total Out I/O operations: " << DiskManager::get_out_io_count()
              << std::endl;

    // Reset I/O counter for joins
    DiskManager::reset_io_count();

    std::cout << "\n2. Performing joins..." << std::endl;

    std::cout << "\nJoin 1: Vinho ⋈ Uva (vinho.uva_id = uva.id)" << std::endl;
    auto join_result1 = JoinOperations::sort_merge_join(
        vinho_table, uva_table, "uva_id", "uva_id", buffer_manager);
    write_join_result_to_file(join_result1, buffer_manager, "vinho", "uva");
    std::cout << "Total I/O operations for Join 1: "
              << join_result1.total_io_operations << std::endl;
    std::cout << "In IO Count: " << DiskManager::get_in_io_count()
              << ", Out IO Count: " << DiskManager::get_out_io_count()
              << std::endl;
    DiskManager::reset_io_count();

    std::cout << "\nJoin : Vinho ⋈ Pais (vinho.pais_producao_id = pais.pais_id)"
              << std::endl;
    auto join_result2 = JoinOperations::sort_merge_join(
        vinho_table, pais_table, "pais_producao_id", "pais_id", buffer_manager);
    write_join_result_to_file(join_result2, buffer_manager, "vinho", "pais");
    std::cout << "Total I/O operations for Join 2: "
              << join_result2.total_io_operations << std::endl;
    std::cout << "In IO Count: " << DiskManager::get_in_io_count()
              << ", Out IO Count: " << DiskManager::get_out_io_count()
              << std::endl;
    DiskManager::reset_io_count();

    std::cout << "\nJoin 3: Uva ⋈ Pais (uva.pais_origem_id = pais.pais_id)"
              << std::endl;
    DiskManager::reset_io_count();
    auto join_result3 = JoinOperations::sort_merge_join(
        uva_table, pais_table, "pais_origem_id", "pais_id", buffer_manager);
    write_join_result_to_file(join_result3, buffer_manager, "uva", "pais");
    std::cout << "Total I/O operations for Join 3: "
              << join_result3.total_io_operations << std::endl;
    std::cout << "In IO Count: " << DiskManager::get_in_io_count()
              << ", Out IO Count: " << DiskManager::get_out_io_count()
              << std::endl;
    DiskManager::reset_io_count();

    std::cout << "=== COMPLETED ===" << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
