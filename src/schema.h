#ifndef SCHEMA_H
#define SCHEMA_H

#include "data_structures.h"
#include <unordered_map>

extern const std::unordered_map<std::string, std::vector<std::string>>
    TABLE_SCHEMAS;

extern const std::unordered_map<std::string,
                                std::unordered_map<std::string, int>>
    ATTRIBUTE_INDICES;

int get_attribute_index(const std::string &table_name,
                        const std::string &attribute_name);

#endif
