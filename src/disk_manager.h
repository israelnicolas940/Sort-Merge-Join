#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include <atomic>
#include <memory>
#include <string>

class Page;

class DiskManager {
private:
  std::string data_directory;
  static std::atomic<int> in_io_count;
  static std::atomic<int> out_io_count;

  std::string get_table_filename(const std::string &table_name);

public:
  DiskManager(const std::string &data_dir = "data/");

  std::shared_ptr<Page> read_page(const std::string &table_name, int page_id);
  void write_page(const std::string &table_name, std::shared_ptr<Page> page);

  bool table_file_exists(const std::string &table_name);
  void create_table_file(const std::string &table_name);
  int get_total_pages(const std::string &table_name);

  // I/O operation counters for monitoring purposes
  static int get_in_io_count() { return in_io_count.load(); }
  static void reset_in_io_count() { in_io_count.store(0); }
  static void increment_in_io_count() { in_io_count.fetch_add(1); }

  static int get_out_io_count() { return out_io_count.load(); }
  static void reset_out_io_count() { out_io_count.store(0); }
  static void increment_out_io_count() { out_io_count.fetch_add(1); }

  static void reset_io_count() {
    reset_in_io_count();
    reset_out_io_count();
  }
  // Utility functions
  void ensure_data_directory();
};

#endif // DISK_MANAGER_H
