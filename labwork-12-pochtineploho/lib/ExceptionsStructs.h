#ifndef LABWORK_12_POCHTINEPLOHO_EXCEPTIONSSTRUCTS_H
#define LABWORK_12_POCHTINEPLOHO_EXCEPTIONSSTRUCTS_H

#include <iostream>
#include <vector>
#include <map>
#include <variant>
#include <set>
#include <numeric>
#include <iomanip>
#include <limits>

const size_t kColumnWidth = 20;

const int kNullInt = std::numeric_limits<int>::max();
const float kNullFloat = std::numeric_limits<float>::max();
const double kNullDouble = std::numeric_limits<double>::max();
const float kMaxDiffFloat = std::numeric_limits<float>::epsilon();
const double kMaxDiffDouble = std::numeric_limits<double>::epsilon();

enum kType {
    kBool = 0, kInt = 1, kFloat = 2, kDouble = 3, kVarchar = 4
};

class BoException : std::exception {
public:
    [[nodiscard]] virtual std::string Info() const {
        return "error";
    }
};

class WrongInputException : BoException {
public:
    explicit WrongInputException(std::string information) : error{std::move(information)} {}

    [[nodiscard]] std::string Info() const {
        return "Unavailable syntax: " + error;
    }

private:
    std::string error;
};

class DoubleNameUsage : BoException {
public:
    explicit DoubleNameUsage(std::string information) : error{std::move(information)} {}

    [[nodiscard]] std::string Info() const {
        return "Double usage of " + error;
    }

private:
    std::string error;
};

class MissingException : BoException {
public:
    explicit MissingException(std::string operation) : operation_(std::move(operation)), missing_("something") {}

    MissingException(std::string operation, std::string missing) : operation_(std::move(operation)),
                                                                   missing_(std::move(missing)) {}

    [[nodiscard]] std::string Info() const {
        return "There must be " + missing_ + " after " + operation_;
    }

private:
    std::string operation_;
    std::string missing_;
};

enum kBoolVariants {
    kTrue, kFalse, kNull
};

struct Table {
    using holds_bool = std::vector<kBoolVariants>;
    using holds_int = std::vector<int>;
    using holds_float = std::vector<float>;
    using holds_double = std::vector<double>;
    using holds_varchar = std::pair<std::vector<std::string>, size_t>;

    std::map<std::string, holds_bool> bool_columns;
    std::map<std::string, holds_int> int_columns;
    std::map<std::string, holds_float> float_columns;
    std::map<std::string, holds_double> double_columns;
    std::map<std::string, holds_varchar> varchar_columns;

    std::vector<std::string> column_order;
    std::map<std::string, bool> might_be_null;
    std::set<std::string> primary_key;
    std::map<std::string, std::pair<std::string, std::string>> foreign_key;

    bool Contains(const std::string& column) {
        return bool_columns.contains(column) || int_columns.contains(column) || float_columns.contains(column) ||
               double_columns.contains(column) || varchar_columns.contains(column);
    }

    std::variant<holds_varchar, holds_int, holds_float, holds_double, holds_bool>
    at(const std::string& index) {
        if (bool_columns.contains(index)) {
            return bool_columns[index];
        }
        if (int_columns.contains(index)) {
            return int_columns[index];
        }
        if (double_columns.contains(index)) {
            return double_columns[index];
        }
        if (float_columns.contains(index)) {
            return float_columns[index];
        }
        if (varchar_columns.contains(index)) {
            return varchar_columns[index];
        }
        throw std::out_of_range("Column " + index + " does not exist");
    }

    void AddColumn(const std::string& column, kType type, size_t varchar_size = 0) {
        if (Contains(column)) {
            throw DoubleNameUsage(column);
        }
        if (type == kBool) {
            bool_columns.insert({column, std::vector<kBoolVariants>()});
            column_order.push_back(column);

            return;
        }
        if (type == kInt) {
            int_columns.insert({column, std::vector<int>()});
            column_order.push_back(column);
            return;
        }
        if (type == kDouble) {
            double_columns.insert({column, std::vector<double>()});
            column_order.push_back(column);

            return;
        }
        if (type == kFloat) {
            float_columns.insert({column, std::vector<float>()});
            column_order.push_back(column);

            return;
        }
        if (type == kVarchar) {
            varchar_columns.insert({column, {std::vector<std::string>(), varchar_size}});
            column_order.push_back(column);
        }
    }

    size_t GetSize() {
        auto column = at(column_order.front());
        if (std::holds_alternative<holds_bool>(column)) {
            return std::get<holds_bool>(column).size();
        } else if (std::holds_alternative<holds_double>(column)) {
            return std::get<holds_double>(column).size();
        } else if (std::holds_alternative<holds_float>(column)) {
            return std::get<holds_float>(column).size();
        } else if (std::holds_alternative<holds_int>(column)) {
            return std::get<holds_int>(column).size();
        } else if (std::holds_alternative<holds_varchar>(column)) {
            return std::get<holds_varchar>(column).first.size();
        }
        return 0;
    }

    void AddElement(const std::string& column, const std::string& element) {
        kType type;
        auto type_definer = at(column);
        if (std::holds_alternative<holds_bool>(type_definer)) {
            type = kBool;
        } else if (std::holds_alternative<holds_double>(type_definer)) {
            type = kDouble;
        } else if (std::holds_alternative<holds_float>(type_definer)) {
            type = kFloat;
        } else if (std::holds_alternative<holds_int>(type_definer)) {
            type = kInt;
        } else if (std::holds_alternative<holds_varchar>(type_definer)) {
            type = kVarchar;
        }
        bool is_primary_key = primary_key.contains(column);
        if (type == kBool) {
            auto tmp = bool_columns.find(column);
            if (tmp == bool_columns.end()) {
                throw WrongInputException(column);
            }
            kBoolVariants result;
            if (element == "TRUE") {
                result = kTrue;
            } else if (element == "FALSE") {
                result = kFalse;
            } else if (element == "NULL" && might_be_null[column]) {
                result = kNull;
            } else {
                throw WrongInputException(element);
            }
            if (is_primary_key) {
                for (auto row: tmp->second) {
                    if (result == row) {
                        throw WrongInputException("Can not have 2 lines with same primary keys");
                    }
                }
            }
            tmp->second.push_back(result);

            return;
        }

        if (type == kInt) {
            auto tmp = int_columns.find(column);
            if (tmp == int_columns.end()) {
                throw WrongInputException(column);
            }
            int result;
            if (element == "NULL" && might_be_null[column]) {
                result = kNullInt;
            } else {
                result = std::stoi(element);
            }
            if (is_primary_key) {
                for (auto row: tmp->second) {
                    if (result == row) {
                        throw WrongInputException("Can not have 2 lines with same primary keys");
                    }
                }
            }
            tmp->second.push_back(result);

            return;
        }
        if (type == kDouble) {
            auto tmp = double_columns.find(column);
            if (tmp == double_columns.end()) {
                throw WrongInputException(column);
            }
            double result;
            if (element == "NULL" && might_be_null[column]) {
                result = kNullDouble;
            } else {
                result = std::stod(element);
            }
            if (is_primary_key) {
                for (auto row: tmp->second) {
                    if (std::abs(result - row) <= kMaxDiffDouble) {
                        throw WrongInputException("Can not have 2 lines with same primary keys");
                    }
                }
            }
            tmp->second.push_back(result);

            return;
        }
        if (type == kFloat) {
            auto tmp = float_columns.find(column);
            if (tmp == float_columns.end()) {
                throw WrongInputException(column);
            }
            float result;
            if (element == "NULL" && might_be_null[column]) {
                result = kNullFloat;
            } else {
                result = std::stof(element);
            }
            if (is_primary_key) {
                for (auto row: tmp->second) {
                    if (std::abs(result - row) <= kMaxDiffFloat) {
                        throw WrongInputException("Can not have 2 lines with same primary keys");
                    }
                }
            }
            tmp->second.push_back(result);

            return;
        }
        if (type == kVarchar) {
            auto tmp = varchar_columns.find(column);
            if (tmp == varchar_columns.end()) {
                throw WrongInputException(column);
            }
            std::string result = element.substr(0, tmp->second.second);
            if (is_primary_key) {
                for (const auto& row: tmp->second.first) {
                    if (result == row) {
                        throw WrongInputException("Can not have 2 lines with same primary keys");
                    }
                }
            }
            tmp->second.first.push_back(result);
        }
    }

    void ChangeElement(const std::string& column, const std::string& element, size_t index) {
        kType type;
        auto type_definer = at(column);
        if (std::holds_alternative<holds_bool>(type_definer)) {
            type = kBool;
        } else if (std::holds_alternative<holds_double>(type_definer)) {
            type = kDouble;
        } else if (std::holds_alternative<holds_float>(type_definer)) {
            type = kFloat;
        } else if (std::holds_alternative<holds_int>(type_definer)) {
            type = kInt;
        } else if (std::holds_alternative<holds_varchar>(type_definer)) {
            type = kVarchar;
        }
        bool is_primary_key = primary_key.contains(column);
        if (type == kBool) {
            auto tmp = bool_columns.find(column);
            if (tmp == bool_columns.end()) {
                throw WrongInputException(column);
            }
            kBoolVariants result;
            if (element == "TRUE") {
                result = kTrue;
            } else if (element == "FALSE") {
                result = kFalse;
            } else if (element == "NULL" && might_be_null[column]) {
                result = kNull;
            } else {
                throw WrongInputException(element);
            }
            size_t counter = 0;
            if (is_primary_key) {
                for (auto row: tmp->second) {
                    if (result == row && counter != index) {
                        throw WrongInputException("Can not have 2 lines with same primary keys");
                    }
                    ++counter;
                }
            }
            tmp->second.operator[](index)= result;

            return;
        }

        if (type == kInt) {
            auto tmp = int_columns.find(column);
            if (tmp == int_columns.end()) {
                throw WrongInputException(column);
            }
            int result;
            if (element == "NULL" && might_be_null[column]) {
                result = kNullInt;
            } else {
                result = std::stoi(element);
            }
            size_t counter = 0;
            if (is_primary_key) {
                for (auto row: tmp->second) {
                    if (result == row && counter != index) {
                        throw WrongInputException("Can not have 2 lines with same primary keys");
                    }
                    ++counter;
                }
            }
            tmp->second.operator[](index) = result;

            return;
        }
        if (type == kDouble) {
            auto tmp = double_columns.find(column);
            if (tmp == double_columns.end()) {
                throw WrongInputException(column);
            }
            double result;
            if (element == "NULL" && might_be_null[column]) {
                result = kNullDouble;
            } else {
                result = std::stod(element);
            }
            size_t counter = 0;
            if (is_primary_key) {
                for (auto row: tmp->second) {
                    if (std::abs(result - row) <= kMaxDiffDouble && counter != index) {
                        throw WrongInputException("Can not have 2 lines with same primary keys");
                    }
                    ++counter;
                }
            }
            tmp->second.operator[](index) = result;

            return;
        }
        if (type == kFloat) {
            auto tmp = float_columns.find(column);
            if (tmp == float_columns.end()) {
                throw WrongInputException(column);
            }
            float result;
            if (element == "NULL" && might_be_null[column]) {
                result = kNullFloat;
            } else {
                result = std::stof(element);
            }
            size_t counter = 0;
            if (is_primary_key) {
                for (auto row: tmp->second) {
                    if (std::abs(result - row) <= kMaxDiffFloat && counter != index) {
                        throw WrongInputException("Can not have 2 lines with same primary keys");
                    }
                    ++counter;
                }
            }
            tmp->second.operator[](index) = result;

            return;
        }
        if (type == kVarchar) {
            auto tmp = varchar_columns.find(column);
            if (tmp == varchar_columns.end()) {
                throw WrongInputException(column);
            }
            std::string result = element.substr(0, tmp->second.second);
            size_t counter = 0;
            if (is_primary_key) {
                for (const auto& row: tmp->second.first) {
                    if (result == row && counter != index) {
                        throw WrongInputException("Can not have 2 lines with same primary keys");
                    }
                    ++counter;
                }
            }
            tmp->second.first.operator[](index) = result;
        }
    }

    void DeleteRow(size_t index) {
        for (auto& column: bool_columns) {
            column.second.erase(column.second.begin() + index);
        }
        for (auto& column: int_columns) {
            column.second.erase(column.second.begin() + index);
        }
        for (auto& column: float_columns) {
            column.second.erase(column.second.begin() + index);
        }
        for (auto& column: double_columns) {
            column.second.erase(column.second.begin() + index);
        }
        for (auto& column: varchar_columns) {
            column.second.first.erase(column.second.first.begin() + index);
        }
    }

    void PrintRow(size_t index, const std::set<std::string>& columns) {
        for (const auto& column: columns) {
            std::cout << std::setw(kColumnWidth);
            auto variant = at(column);
            if (std::holds_alternative<Table::holds_varchar>(variant)) {
                std::cout << std::get<Table::holds_varchar>(variant).first.at(index);
            } else if (std::holds_alternative<Table::holds_double>(variant)) {
                if (std::get<Table::holds_double>(variant).at(index) != kNullDouble) {
                    std::cout << std::get<Table::holds_double>(variant).at(index);
                } else {
                    std::cout << "NULL";
                }
            } else if (std::holds_alternative<Table::holds_float>(variant)) {
                if (std::get<Table::holds_float>(variant).at(index) != kNullFloat) {
                    std::cout << std::get<Table::holds_float>(variant).at(index);
                } else {
                    std::cout << "NULL";
                }
            } else if (std::holds_alternative<Table::holds_int>(variant)) {
                if (std::get<Table::holds_int>(variant).at(index) != kNullInt) {
                    std::cout << std::get<Table::holds_int>(variant).at(index);
                } else {
                    std::cout << "NULL";
                }
            } else {
                if (std::get<Table::holds_bool>(variant).at(index) == kTrue) {
                    std::cout << "TRUE";
                } else if (std::get<Table::holds_bool>(variant).at(index) == kFalse) {
                    std::cout << "FALSE";
                } else {
                    std::cout << "NULL";
                }
            }
        }
    }

    ~Table() = default;
};

enum kLogic {
    kAnd, kOr, kNone, kDefault
};

enum kCompare {
    kLess, kEqual, kGreater, kLEq, kGEq, kNEq, kIsNull
};

struct Where {
    Where* left = nullptr;
    Where* right = nullptr;
    bool is_not = false;
    kLogic logic_connect = kDefault;
    std::pair<std::string, std::string> column; // table + column
    std::pair<std::string, std::string> compare_column;
    kCompare comparator;
    std::variant<kBoolVariants, int, float, double, std::string> compare_with;
};

inline void ClearWhere(Where* root) {
    if (root == nullptr) {
        return;
    }
    if (root->left != nullptr) {
        ClearWhere(root->left);
    }
    if (root->right != nullptr) {
        ClearWhere(root->right);
    }
    delete root->left;
    delete root->right;
}

enum kJoinType {
    kLeft, kRight, kInner
};

struct Join {
    bool exists;
    kJoinType type;
    std::string table;
    std::set<std::string> join_columns;
    Where* on;
};

struct Select {
    std::string from;
    std::set<std::string> columns;
    Where* where = nullptr;
    Join join;

    void Clear() {
        columns.clear();
        ClearWhere(where);
        delete where;
        where = nullptr;
        from.clear();
        join.exists = false;
    }
};

inline std::string_view FindVarchar(std::string_view& line) {
    size_t word_begin = line.find_first_not_of(" \n");
    if (word_begin == std::string::npos || line[word_begin] != '\'' && line[word_begin] != '\"') {
        return "";
    }
    size_t word_end = line.find(line[word_begin], word_begin + 1);
    if (word_end == std::string::npos) {
        throw MissingException("opening quote", "closing quote");
    }
    std::string_view word = line.substr(word_begin + 1, word_end - word_begin - 1);
    line = line.substr(word_end + 1);

    return word;
}

#endif //LABWORK_12_POCHTINEPLOHO_EXCEPTIONSSTRUCTS_H
