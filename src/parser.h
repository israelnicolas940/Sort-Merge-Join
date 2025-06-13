#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <string>
#include <vector>

class Table;
class BufferManager;

class CSVParser {
private:
  static std::vector<std::string> split_csv_line(const std::string &line);
  static std::string trim(const std::string &str);

public:
  static std::shared_ptr<Table>
  parse_uva_csv(const std::string &filename,
                std::shared_ptr<BufferManager> buffer_manager);

  static std::shared_ptr<Table>
  parse_vinho_csv(const std::string &filename,
                  std::shared_ptr<BufferManager> buffer_manager);

  static std::shared_ptr<Table>
  parse_pais_csv(const std::string &filename,
                 std::shared_ptr<BufferManager> buffer_manager);

  // Generic CSV parser
  static std::shared_ptr<Table>
  parse_csv(const std::string &filename, const std::string &table_name,
            const std::vector<std::string> &expected_columns,
            std::shared_ptr<BufferManager> buffer_manager);
};

#endif // PARSER_H
