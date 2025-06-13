#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H
#include <list>
#include <memory>
#include <string>
#include <unordered_map>

class DiskManager;
class Page;

class BufferManager {
private:
  static const size_t BUFFER_SIZE = 4; // Maximum 4 pages in memory

  std::shared_ptr<DiskManager> disk_manager;

  // LRU implementation
  std::list<std::pair<std::string, std::shared_ptr<Page>>> lru_list;
  std::unordered_map<
      std::string,
      std::list<std::pair<std::string, std::shared_ptr<Page>>>::iterator>
      page_map;

  void evict_page();
  std::string make_key(const std::string &table_name, int page_id);

public:
  BufferManager(std::shared_ptr<DiskManager> dm);

  std::shared_ptr<Page> get_page(const std::string &table_name, int page_id);
  void write_page(const std::string &table_name, std::shared_ptr<Page> page);
  void flush_all();

  // Buffer statistics
  size_t get_buffer_usage() const { return page_map.size(); }
  bool is_buffer_full() const { return page_map.size() >= BUFFER_SIZE; }
};

#endif // BUFFER_MANAGER_H
