#ifndef TABLE_H
#define TABLE_H
#include <string>
#include "buffer_manager.h"

class Table {
public:
    Table(const std::string filename);

private:
    BufferManager buffer_manager;

};
#endif
