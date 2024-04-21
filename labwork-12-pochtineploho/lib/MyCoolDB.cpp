#include "MyCoolDB.h"
#include <fstream>

void MyCoolDB::Request(const std::string& request) {
    data_base.Parse(request);
}

void MyCoolDB::Save(const std::filesystem::path& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << path << " does not exist";

        return;
    }
    file << data_base.data.size() << "\n";
    for (const auto& table: data_base.data) {
        file << table.first << " " << table.second->column_order.size() << " " << table.second->GetSize() << "\n";
        for (const auto& column: table.second->bool_columns) {
            file << "BOOL " << column.first << " ";
            for (auto element: column.second) {
                if (element == kTrue) {
                    file << "TRUE ";
                } else if (element == kFalse) {
                    file << "FALSE ";
                } else {
                    file << "NULL ";
                }
            }
            file << "\n";
        }
        for (const auto& column: table.second->int_columns) {
            file << "INT " << column.first << " ";
            for (auto element: column.second) {
                if (element == kNullInt) {
                    file << "NULL ";
                } else {
                    file << element << " ";
                }
            }
            file << "\n";
        }
        for (const auto& column: table.second->float_columns) {
            file << "FLOAT " << column.first << " ";
            for (auto element: column.second) {
                if (element == kNullFloat) {
                    file << "NULL ";
                } else {
                    file << element << " ";
                }
            }
            file << "\n";
        }
        for (const auto& column: table.second->double_columns) {
            file << "DOUBLE " << column.first << " ";
            for (auto element: column.second) {
                if (element == kNullDouble) {
                    file << "NULL ";
                } else {
                    file << element << " ";
                }
            }
            file << "\n";
        }
        for (const auto& column: table.second->varchar_columns) {
            file << "VARCHAR " << column.second.second << " " << column.first << " ";
            for (const auto& element: column.second.first) {
                file << "\"" << element << "\" ";
            }
            file << "\n";
        }
        for (const auto& order: table.second->column_order) {
            file << order << " ";
        }
        file << "\n";
        file << table.second->primary_key.size() << " ";
        for (const auto& p_key: table.second->primary_key) {
            file << p_key << " ";
        }
        file << "\n";
        for (const auto& might_be_null: table.second->might_be_null) {
            file << might_be_null.first << " " << might_be_null.second << " ";
        }
        file << "\n";
        file << table.second->foreign_key.size() << "\n";
        for (const auto& f_key: table.second->foreign_key) {
            file << f_key.first << " " << f_key.second.first << " " << f_key.second.second << " ";
        }
        file << "\n";
    }
    std::cout << "Data base saved!\n";
}

void MyCoolDB::Upload(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << path << " does not exist";

        return;
    }
    size_t data_size;
    size_t columns_quantity;
    size_t rows_quantity;
    std::string table_name;
    std::string data_type;
    std::string column_name;
    std::string str_data;
    size_t varchar_size;
    file >> data_size;

    for (int i = 0; i < data_size; ++i) {
        file >> table_name >> columns_quantity >> rows_quantity;
        auto* table = new Table;
        for (int j = 0; j < columns_quantity; ++j) {
            file >> data_type;
            if (data_type == "VARCHAR") {
                file >> varchar_size;
            }
            file >> column_name;
            if (data_type == "INT") {
                std::vector<int> tmp;
                for (size_t k = 0; k < rows_quantity; ++k) {
                    file >> str_data;
                    if (str_data == "NULL") {
                        tmp.emplace_back(kNullInt);
                    } else {
                        tmp.emplace_back(std::stoi(str_data));
                    }
                }
                table->int_columns.emplace(column_name, tmp);
            } else if (data_type == "BOOL") {
                std::vector<kBoolVariants> tmp;
                for (size_t k = 0; k < rows_quantity; ++k) {
                    file >> str_data;
                    if (str_data == "NULL") {
                        tmp.emplace_back(kNull);
                    } else if (str_data == "FALSE") {
                        tmp.emplace_back(kFalse);
                    } else if (str_data == "TRUE") {
                        tmp.emplace_back(kTrue);
                    } else {
                        std::cerr << "Bool column can not contain " << str_data;
                    }
                }
                table->bool_columns.insert({column_name, tmp});
            } else if (data_type == "FLOAT") {
                std::vector<float> tmp;
                for (size_t k = 0; k < rows_quantity; ++k) {
                    file >> str_data;
                    if (str_data == "NULL") {
                        tmp.emplace_back(kNullFloat);
                    } else {
                        tmp.emplace_back(std::stof(str_data));
                    }
                }
                table->float_columns.insert({column_name, tmp});
            } else if (data_type == "DOUBLE") {
                std::vector<double> tmp;
                for (size_t k = 0; k < rows_quantity; ++k) {
                    file >> str_data;
                    if (str_data == "NULL") {
                        tmp.emplace_back(kNullDouble);
                    } else {
                        tmp.emplace_back(std::stod(str_data));
                    }
                }
                table->double_columns.insert({column_name, tmp});
            } else if (data_type == "VARCHAR") {
                std::vector<std::string> tmp;
                std::string line;
                std::getline(file, line);
                std::string_view line_view{line};
                for (size_t k = 0; k < rows_quantity; ++k) {
                    str_data = std::string(FindVarchar(line_view));
                    tmp.emplace_back(str_data.substr(0, varchar_size));
                }
                table->varchar_columns.insert({column_name, {tmp, varchar_size}});
            }
        }
        data_base.data.emplace(table_name, table);
        for (int j = 0; j < columns_quantity; ++j) {
            file >> str_data;
            data_base.data[table_name]->column_order.emplace_back(str_data);
        }
        size_t primary_key_size;
        file >> primary_key_size;
        for (size_t j = 0; j < primary_key_size; ++j) {
            file >> str_data;
            data_base.data[table_name]->primary_key.insert(str_data);
        }
        for (size_t j = 0; j < columns_quantity; ++j) {
            int mbn;
            file >> str_data >> mbn;
            data_base.data[table_name]->might_be_null.insert({str_data, bool(mbn)});
        }
        size_t f_key_quantity;
        file >> f_key_quantity;
        std::string tmp_1;
        std::string tmp_2;
        for (int j = 0; j < f_key_quantity; ++j) {
            file >> str_data >> tmp_1 >> tmp_2;
            data_base.data[table_name]->foreign_key.insert({str_data, {tmp_1, tmp_2}});
        }

    }
    std::cout << "Data base uploaded!\n";
}
