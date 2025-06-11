#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <string>
#include <vector>

#define NO_ERR 0
#define ERR 1

struct Row {
  std::vector<std::string> columns;
};

struct Page {
  std::vector<Row> rows;
  int occupiedRowCount = 0;
};

struct Table {
  std::vector<Page> pages;
  int pageCount = 0;
  int columnCount = 0;
  std::string tableName;
};

struct JoinCondition {
  std::string firstTable;
  std::string firstColumn;
  std::string secondTable;
  std::string secondColumn;
};

#endif
