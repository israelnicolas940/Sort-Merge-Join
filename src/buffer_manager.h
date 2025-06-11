#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H
#include <string>
#include <vector>
#include "disk_manager.h"

class BufferManager {
public:
    BufferManager(const std::string& filename);
    ~BufferManager();

    int readPage(int pageIndex, Page& page);
    int writePage(int pageIndex, const Page& page);
    int allocatePage(Page& page);
    int deallocatePage(int pageIndex);
    int getTotalPages() const;
    int getPageSize() const;
    int getOccupiedRowCount(int pageIndex) const;
    int getColumnCount(int pageIndex) const;
    std::string getTableName() const;
    void setTableName(const std::string& name);
    std::vector<Page> loadPages(int pageIndex, int numColumns, int& totalPages);
    std::vector<Page> loadPages(int pageIndex, const std::vector<std::string>& schema, int& totalPages);
    Page loadPage(int pageIndex, int numColumns);
private:
    std::string filename;
    int totalPages;
    int pageSize;
    std::string tableName;
    DiskManager disk_manager;

    void loadFromDisk();
    void saveToDisk();
    void initializeBuffer();
    void clearBuffer();
};

#endif // BUFFER_MANAGER_H
