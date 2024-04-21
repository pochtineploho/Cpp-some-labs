#ifndef LABWORK_12_POCHTINEPLOHO_MYCOOLDB_H
#define LABWORK_12_POCHTINEPLOHO_MYCOOLDB_H
#include "DataBase.h"
#include <filesystem>

class MyCoolDB {
private:
    DataBase data_base;
public:
    void Request(const std::string& request);

    void Save(const std::filesystem::path& path);

    void Upload(const std::filesystem::path& path);
};


#endif
