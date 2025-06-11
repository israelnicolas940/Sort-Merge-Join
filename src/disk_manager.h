#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include "data_structures.h"
#include "schema.h"

class DiskManager {
public:
    Table load_table_from_disk(const std::string &filename);

    void save_table_to_disk(const Table &table, const std::string &filename);

    int read_file(const std::string &filename,
                std::vector<std::vector<std::string>> &rows, int &total_rows);

    std::vector<Page> load_pages_from_disk(const std::string &filename,
                                        int page_num, int num_columns,
                                        int &total_pages);

    std::vector<Page> load_pages_from_disk(const std::string &filename,
                                        int page_num,
                                        std::vector<std::string> schema,
                                        int &total_pages);

    Page load_page_from_disk(const std::string &filename, int page_num,
                            int num_columns);

    Page load_page_from_disk(const std::string &filename, int page_num,
                            std::vector<std::string> schema);

    void save_page_to_disk(const std::string &filename, int page_num,
                        const Page &page);

    void save_pages_to_disk(const std::string &filename, int page_num,
                            const std::vector<Page> &pages);

    Table create_merged_table(const Table &left_table, const Table &right_table);

private:
    void load_from_disk(const std::string &filename, Table &table);

    void save_to_disk(const std::string &filename, const Table &table);
    void initialize_table(Table &table, const std::string &filename);
    void clear_table(Table &table);
};

#endif
