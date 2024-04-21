#ifndef LABWORK_12_POCHTINEPLOHO_DATABASE_H
#define LABWORK_12_POCHTINEPLOHO_DATABASE_H

#include "ExceptionsStructs.h"

#include <string_view>
#include <string>
#include <stack>
#include <algorithm>

class DataBase {
public:
    std::map<std::string, Table*> data;
    Select selected;

    void Parse(const std::string& request_string);

    void ParseCreate(std::string_view line);

    void CheckSuffix(std::string_view& line, const std::string& table_name, const std::string& column_name);

    void ParseReferences(std::string_view& line, const std::string& table_name, const std::string& column_name);

    void ParseDrop(std::string_view line);

    void ParseInsert(std::string_view line);

    void ParseValues(std::string_view line, const std::vector<std::string>& columns, const std::string& table_name);

    bool CheckForeignKey(const std::string& table_name, const std::string& column, const std::string& element);

    void ParseSelect(std::string_view line);

    std::string_view ParseFrom(std::string_view line);

    std::string_view ParseSelectFrom(std::string_view line);

    void ParseWhere(std::string_view line);

    Where* BuildWhereTree(std::string_view line, size_t& index, bool is_not, bool join = false);

    Where* ParsePredicate(std::string_view line, bool join = false);

    Where* ParseWhereLogicPart(std::string_view line, bool join = false);

    void ParseColumns(std::string_view line, const std::string& table_name);

    void ParseOneColumn(std::string_view line, const std::string& table_name);

    std::pair<kType, size_t> ParseType(std::string_view type);

    void ParseUpdate(std::string_view bool_v);

    void ParseDelete(std::string_view line);

    bool CheckRowWhere(Where* node, size_t index, size_t join_index = 0);

    void ParseFromJoin(std::string_view line);

    void PrintSelected();
};


#endif //LABWORK_12_POCHTINEPLOHO_DATABASE_H
