#ifndef TABLE_H
#define TABLE_H
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class BufferManager;

struct Row {
  std::vector<std::string> columns;

  Row() = default;
  Row(const std::vector<std::string> &cols) : columns(cols) {}

  std::string &operator[](size_t index) { return columns[index]; }
  const std::string &operator[](size_t index) const { return columns[index]; }

  size_t size() const { return columns.size(); }
  void resize(size_t size) { columns.resize(size); }
};

struct Page {
  static const size_t MAX_ROWS = 10;
  std::vector<Row> rows;
  int page_id;
  bool dirty;

  Page(int id = -1) : page_id(id), dirty(false) {}

  bool is_full() const { return rows.size() >= MAX_ROWS; }
  void add_row(const Row &row);
  void clear();
};

class Table {
private:
  std::string table_name;
  std::vector<std::string> column_names;
  std::unordered_map<std::string, size_t> column_index_map;
  std::shared_ptr<BufferManager> buffer_manager;
  int total_pages;

public:
  Table(const std::string &name, const std::vector<std::string> &columns,
        std::shared_ptr<BufferManager> bm);

  // void load_from_csv(const std::string &filename);

  size_t get_column_count() const { return column_names.size(); }
  const std::vector<std::string> &get_column_names() const {
    return column_names;
  }
  int get_column_index(const std::string &column_name) const;

  std::shared_ptr<Page> get_page(int page_id);
  void write_page(std::shared_ptr<Page> page);
  int get_total_pages() const { return total_pages; }

  const std::string &get_name() const { return table_name; }
  void set_total_pages(int pages) { total_pages = pages; }

  // Iterator support for join operations
  class Iterator {
  private:
    Table *table;
    int current_page;
    size_t current_row;
    std::shared_ptr<Page> current_page_ptr;

  public:
    Iterator(Table *t, int page = 0, size_t row = 0);

    bool has_next();
    Row next();
    void reset();
  };

  Iterator get_iterator() { return Iterator(this); }
};

#endif
