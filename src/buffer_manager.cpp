#include "buffer_manager.h"
#include "disk_manager.h"
#include "table.h"

BufferManager::BufferManager(std::shared_ptr<DiskManager> dm)
    : disk_manager(dm) {}

std::string BufferManager::make_key(const std::string &table_name,
                                    int page_id) {
  return table_name + "_" + std::to_string(page_id);
}

std::shared_ptr<Page> BufferManager::get_page(const std::string &table_name,
                                              int page_id) {
  std::string key = make_key(table_name, page_id);

  // Check if page is already in buffer
  auto it = page_map.find(key);
  if (it != page_map.end()) {
    // Move to front of LRU list
    auto page_it = it->second;
    auto page_pair = *page_it;
    lru_list.erase(page_it);
    lru_list.push_front(page_pair);
    page_map[key] = lru_list.begin();

    return page_pair.second;
  }

  // Page not in buffer, need to load from disk
  if (is_buffer_full()) {
    evict_page();
  }

  auto page = disk_manager->read_page(table_name, page_id);

  // Add to buffer
  lru_list.push_front(std::make_pair(key, page));
  page_map[key] = lru_list.begin();

  return page;
}

void BufferManager::write_page(const std::string &table_name,
                               std::shared_ptr<Page> page) {
  std::string key = make_key(table_name, page->page_id);

  // Update page in buffer if it exists
  auto it = page_map.find(key);
  if (it != page_map.end()) {
    auto page_it = it->second;
    page_it->second = page;
    page->dirty = true;

    // Move to front of LRU list
    auto page_pair = *page_it;
    lru_list.erase(page_it);
    lru_list.push_front(page_pair);
    page_map[key] = lru_list.begin();
  } else {
    // Page not in buffer, add if there's space
    if (is_buffer_full()) {
      evict_page();
    }

    page->dirty = true;
    lru_list.push_front(std::make_pair(key, page));
    page_map[key] = lru_list.begin();
  }

  // Write to disk immediately (write-through policy)
  disk_manager->write_page(table_name, page);
  page->dirty = false;
}

void BufferManager::evict_page() {
  if (lru_list.empty()) {
    return;
  }

  // Get least recently used page
  auto last_pair = lru_list.back();
  auto page = last_pair.second;

  // Write to disk if dirty
  if (page->dirty) {
    size_t underscore_pos = last_pair.first.find_last_of('_');
    std::string table_name = last_pair.first.substr(0, underscore_pos);
    disk_manager->write_page(table_name, page);
  }

  // Remove from buffer
  page_map.erase(last_pair.first);
  lru_list.pop_back();
}

void BufferManager::flush_all() {
  for (auto &pair : lru_list) {
    auto page = pair.second;
    if (page->dirty) {
      size_t underscore_pos = pair.first.find_last_of('_');
      std::string table_name = pair.first.substr(0, underscore_pos);
      disk_manager->write_page(table_name, page);
      page->dirty = false;
    }
  }
}
