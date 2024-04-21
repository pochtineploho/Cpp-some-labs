#include "DataBase.h"

std::string_view FindNextWordCut(std::string_view& line) {
    size_t word_begin = line.find_first_not_of(" \n");
    if (word_begin == std::string::npos) {
        return "";
    }
    size_t word_end = std::min(line.find_first_of(" \n", word_begin), line.size());
    std::string_view word = line.substr(word_begin, word_end - word_begin);
    line = line.substr(word_end);

    return word;
}

std::string_view FindNextWord(std::string_view line) {
    size_t word_begin = line.find_first_not_of(" \n");
    if (word_begin == std::string::npos) {
        return "";
    }
    size_t word_end = std::min(line.find_first_of(" \n", word_begin), line.size());
    std::string_view word = line.substr(word_begin, word_end - word_begin);

    return word;
}

std::string_view FindWordOrBracket(std::string_view& line) {
    size_t word_begin = line.find_first_not_of(" \n");
    if (word_begin == std::string::npos) {
        return "";
    } else if (line[word_begin] == '(') {
        line = line.substr(word_begin + 1);
        return "(";
    }
    size_t word_end = std::min(line.find_first_of(" \n", word_begin), line.size());
    std::string_view word = line.substr(word_begin, word_end - word_begin);
    line = line.substr(word_end);

    return word;
}

std::string_view FindName(std::string_view& line) {
    size_t word_begin = line.find_first_not_of(" \n");
    if (word_begin == std::string::npos) {
        return "";
    }
    if (line[word_begin] != '(') {
        line = line.substr(word_begin);
        return FindNextWordCut(line);
    }
    size_t right_bracket = line.find_first_of(')');
    if (right_bracket == std::string::npos) {
        throw MissingException("(", ")");
    }
    word_begin = line.find_first_not_of(" \n", word_begin + 1);
    size_t word_end = std::min(line.find_first_of(" \n", word_begin + 1), line.size());
    std::string_view word = line.substr(word_begin, word_end - word_begin);
    line = line.substr(right_bracket + 1);

    return word;
}

void DataBase::Parse(const std::string& request_string) {
    std::vector<std::string_view> commands;
    size_t split_position_begin = 0;
    size_t split_position_end;
    std::string_view request{request_string};
    while ((split_position_end = request.find(';', split_position_begin)) != std::string::npos) {
        commands.push_back(request.substr(split_position_begin, split_position_end - split_position_begin));
        split_position_begin = split_position_end + 1;
    }
    for (auto& line: commands) {
        std::string_view first_operation = FindNextWordCut(line);
        if (first_operation.empty()) {
            continue;
        }
        try {
            if (first_operation == "SELECT") {
                ParseSelect(line);
            } else if (first_operation == "CREATE") {
                ParseCreate(line);
            } else if (first_operation == "DROP") {
                ParseDrop(line);
            } else if (first_operation == "INSERT") {
                ParseInsert(line);
            } else if (first_operation == "UPDATE") {
                ParseUpdate(line);
            } else if (first_operation == "DELETE") {
                ParseDelete(line);
            } else {
                throw WrongInputException(std::string(first_operation));
            }
        }
        catch (const WrongInputException& exception) {
            std::cerr << exception.Info() << "\n";
        }
        catch (const MissingException& exception) {
            std::cerr << exception.Info() << "\n";
        }
    }
}

void DataBase::ParseCreate(std::string_view line) {
    std::string_view word = FindNextWordCut(line);
    if (word != "TABLE") {
        throw MissingException("CREATE", "TABLE");
    }
    std::string table_name;
    table_name = std::string(FindName(line));
    if (table_name.empty()) {
        throw MissingException("CREATE TABLE ", "table name");
    }
    if (std::isdigit(table_name.front())) {
        throw WrongInputException("table names van not begin with digit");
    }
    for (auto& table: data) {
        if (table.first == table_name) {
            throw DoubleNameUsage("data base name \"" + table_name + "\"");
        }
    }
    word = FindWordOrBracket(line);
    if (word == "(") {
        size_t right_bracket = line.find_last_of(')');
        if (right_bracket == std::string::npos) {
            throw MissingException("(", ")");
        }
        line.substr(0, right_bracket);
        data[table_name] = new Table;
        ParseColumns(line, table_name);
    } else {
        throw MissingException("CREATE TABLE " + table_name, "columns");
    }
}

void DataBase::ParseColumns(std::string_view line, const std::string& table_name) {
    size_t coma_index = line.find_first_of(',');
    while (coma_index != std::string::npos) {
        ParseOneColumn(line.substr(0, coma_index), table_name);
        line = line.substr(coma_index + 1);
        coma_index = line.find_first_of(',');
    }
    ParseOneColumn(line.substr(0, line.find_last_of(')')), table_name);
}

void DataBase::ParseOneColumn(std::string_view line, const std::string& table_name) {
    size_t index = line.find_first_not_of(" \n");
    if (index == std::string::npos) {
        throw MissingException("\",\"", "column name");
    }
    line.substr(index);
    std::string_view column_name;
    std::string_view word = FindNextWordCut(line);
    if (word == "PRIMARY") {
        word = FindNextWordCut(line);
        if (word == "KEY") {
            column_name = FindName(line);
            if (column_name.empty()) {
                throw MissingException("PRIMARY KEY");
            }
            data[table_name]->primary_key.insert(std::string(column_name));
            return;
        } else {
            throw MissingException("PRIMARY", "KEY");
        }
    } else if (word == "FOREIGN") {
        word = FindNextWordCut(line);
        if (word == "KEY") {
            column_name = FindName(line);
            if (column_name.empty()) {
                throw MissingException("FOREIGN KEY");
            }
            ParseReferences(line, table_name, std::string(column_name));
            return;
        } else {
            throw MissingException("FOREIGN", "KEY");
        }
    } else {
        column_name = word;
    }
    if (column_name.empty()) {
        column_name = FindNextWordCut(line);
    }
    std::string_view type = FindNextWordCut(line);
    std::pair<kType, size_t> parsed_type = ParseType(type);
    std::string cn_tmp = std::string(column_name);
    data[table_name]->AddColumn(cn_tmp, parsed_type.first, parsed_type.second);
    CheckSuffix(line, table_name, cn_tmp);
    CheckSuffix(line, table_name, cn_tmp);
}

void DataBase::CheckSuffix(std::string_view& line, const std::string& table_name, const std::string& column_name) {
    std::string_view word = FindNextWordCut(line);
    if (word.empty() || word == "NULL") {
        data[table_name]->might_be_null[column_name] = true;
    } else if (word == "NOT") {
        word = FindNextWordCut(line);
        if (word == "NULL") {
            data[table_name]->might_be_null[column_name] = false;
        } else {
            throw MissingException("NOT", "NULL");
        }
    } else if (word == "PRIMARY") {
        word = FindNextWordCut(line);
        if (word == "KEY") {
            data[table_name]->primary_key.insert(column_name);
        } else {
            throw MissingException("PRIMARY", "KEY");
        }
    } else if (word == "FOREIGN") {
        word = FindNextWordCut(line);
        if (word == "KEY") {
            ParseReferences(line, table_name, column_name);
        } else {
            throw MissingException("FOREIGN", "KEY");
        }
    } else {
        throw WrongInputException(std::string(word));
    }
}

void DataBase::ParseReferences(std::string_view& line, const std::string& table_name, const std::string& column_name) {
    std::string_view word = FindNextWordCut(line);
    if (word != "REFERENCES") {
        throw MissingException("FOREIGN KEY", "REFERENCES");
    }
    size_t index_1 = line.find('(');
    size_t index_2 = line.find(')');
    if (index_1 == std::string::npos || index_2 == std::string::npos) {
        throw MissingException("REFERENCES", "brackets");
    }
    std::string table = std::string(FindNextWord(line.substr(0, index_1)));
    std::string column = std::string(FindNextWord(line.substr(index_1 + 1, index_2 - index_1 - 1)));
    if (!data.contains(table)) {
        throw WrongInputException(table + " table does not exist");
    }
    if (!data[table]->Contains(column)) {
        throw WrongInputException(column + " column does not exist in " + table);
    }
    if (data[table_name]->at(column).index() != data[table_name]->at(column_name).index()) {
        throw WrongInputException(column_name + " and its FOREIGN KEY must have similar types");
    }
    data[table_name]->foreign_key.insert({column_name, {table, column}});
}

std::pair<kType, size_t> DataBase::ParseType(std::string_view type) {
    if (type == "BOOL") {
        return {kBool, 0};
    } else if (type == "INT") {
        return {kInt, 0};
    } else if (type == "FLOAT") {
        return {kFloat, 0};
    } else if (type == "DOUBLE") {
        return {kDouble, 0};
    }
    std::string_view varchar = type.substr(0, 7);
    size_t left_bracket = type.find_first_of('(');
    size_t right_bracket = type.find_first_of(')');
    if (left_bracket != std::string::npos && right_bracket != std::string::npos && varchar == "VARCHAR") {
        type = type.substr(left_bracket + 1, right_bracket - left_bracket);
        size_t size = std::stoll(std::string(type));
        if (size <= 0) {
            throw WrongInputException("size of VARCHAR must be positive");
        }
        return {kVarchar, size};
    }

    throw WrongInputException(std::string(type));
}

void DataBase::ParseDrop(std::string_view line) {
    std::string_view word = FindNextWordCut(line);
    if (word != "TABLE") {
        throw MissingException("DROP", "TABLE");
    }
    std::string table_name;
    table_name = std::string(FindName(line));
    if (table_name.empty()) {
        throw MissingException("DROP TABLE ", "table name");
    }
    bool deleted = false;
    for (auto& table: data) {
        if (table.first == table_name) {
            delete data[table_name];
            data.erase(table_name);
            deleted = true;
            break;
        }
    }
    if (!deleted) {
        throw WrongInputException("table " + table_name + " does not exist");
    }
}

std::string_view DataBase::ParseSelectFrom(std::string_view line) {
    size_t index_from = line.find("FROM");
    if (index_from == std::string::npos) {
        throw MissingException("SELECT", "FROM");
    }
    std::string table_name = std::string(ParseFrom(line.substr(index_from + 4)));
    selected.from = table_name;
    size_t index = line.find(',');
    while (index < index_from) {
        auto column = std::string(FindNextWord(line.substr(0, index)));
        if (column == "*") {
            for (const auto& column_name: data[table_name]->column_order) {
                selected.columns.insert(column_name);
            }
            break;
        }
        if (!data[table_name]->Contains(column)) {
            throw WrongInputException(column);
        }
        selected.columns.insert(column);
        line = line.substr(index + 1);
        index_from -= index + 1;
        index = line.find(',');
    }
    auto column = std::string(FindNextWordCut(line));
    if (column == "*") {
        for (const auto& column_name: data[table_name]->column_order) {
            selected.columns.insert(column_name);
        }
    } else if (!data[table_name]->Contains(column)) {
        throw WrongInputException(column);
    } else {
        selected.columns.insert(column);
    }
    line = line.substr(line.find(table_name) + table_name.size());

    return line;
}

void DataBase::ParseSelect(std::string_view line) {
    selected.Clear();
    if (line.find("JOIN") == std::string::npos) {
        line = ParseSelectFrom(line);
        ParseWhere(line);
        PrintSelected();

        return;
    }
    selected.join.exists = true;
    ParseFromJoin(line);
    line = line.substr(0, line.find("FROM"));
    size_t index = line.find(',');
    std::string_view word;
    std::string table;
    std::string column;
    size_t dot_index;
    while (index != std::string::npos) {
        word = FindNextWord(line.substr(0, index));
        dot_index = word.find('.');
        if (dot_index == std::string::npos) {
            throw WrongInputException("expected format table.column");
        }
        table = std::string(word.substr(0, dot_index));
        column = std::string(word.substr(dot_index + 1));
        if (table == selected.from) {
            selected.columns.insert(column);
        } else if (table == selected.join.table) {
            selected.join.join_columns.insert(column);
        } else {
            throw WrongInputException("SELECTED contains unpredictable columns");
        }
        line = line.substr(index + 1);
        index = line.find(',');
    }
    word = FindNextWordCut(line);
    dot_index = word.find('.');
    if (dot_index == std::string::npos) {
        throw WrongInputException("expected format table.column");
    }
    table = std::string(word.substr(0, dot_index));
    column = std::string(word.substr(dot_index + 1));
    if (table == selected.from) {
        selected.columns.insert(column);
    } else if (table == selected.join.table) {
        selected.join.join_columns.insert(column);
    } else {
        throw WrongInputException("SELECTED contains unpredictable columns");
    }
    PrintSelected();
}

std::string_view DataBase::ParseFrom(std::string_view line) {
    std::string_view table_name = FindName(line);
    if (data.find(std::string(table_name)) != data.end()) {
        return table_name;
    }
    throw WrongInputException("FROM " + std::string(table_name));
}

void DataBase::ParseFromJoin(std::string_view line) {
    size_t index = line.find("FROM");
    if (index == std::string::npos) {
        throw MissingException("SELECT", "FROM");
    }
    line = line.substr(index);
    FindNextWordCut(line);
    std::string table_name = std::string(FindName(line));
    if (data.find(table_name) == data.end()) {
        throw WrongInputException(table_name + " table does not exist");
    }
    selected.from = table_name;
    std::string_view word = FindNextWordCut(line);
    if (word == "INNER" || word == "JOIN") {
        selected.join.type = kInner;
    } else if (word == "LEFT") {
        selected.join.type = kLeft;
    } else if (word == "RIGHT") {
        selected.join.type = kRight;
    } else {
        throw WrongInputException("JOIN expected");
    }
    if (word != "JOIN") {
        if (FindNextWordCut(line) != "JOIN") {
            throw MissingException("JOIN type", "JOIN");
        }
    }
    table_name = std::string(FindName(line));
    if (data.find(table_name) == data.end()) {
        throw WrongInputException(table_name + " table does not exist");
    }
    selected.join.table = table_name;
    if (FindNextWordCut(line) != "ON") {
        throw MissingException("JOIN", "ON");
    }
    index = 0;
    selected.join.on = BuildWhereTree(line, index, false, true);
    ParseWhere(line);
}

void DataBase::ParseWhere(std::string_view line) {
    if (FindNextWord(line) != "WHERE") {
        return;
    }
    FindNextWordCut(line);
    size_t index = 0;

    selected.where = BuildWhereTree(line, index, false);
}

Where*
DataBase::BuildWhereTree(std::string_view line, size_t& index, bool is_not, bool join) {
    auto* new_where = new Where;
    if (is_not) {
        new_where->is_not = true;
    }
    std::vector<std::pair<size_t, size_t>> last_word_index;
    std::vector<std::string_view> words;
    bool quote_1 = false;
    bool quote_2 = false;
    bool part = false;
    bool contains_brackets = false;
    bool reading_word = false;
    size_t index_begin = index;
    for (size_t i = 0; i < line.size(); ++i) {
        index++;
        if (line[i] == ' ' || line[i] == '\n') {
            if (reading_word) {
                last_word_index.back().second = i;
                words.push_back(
                        line.substr(last_word_index.back().first,
                                    last_word_index.back().second - last_word_index.back().first));
                reading_word = false;
            }
            continue;
        }
        if (line[i] == '\'') {
            quote_1 = !quote_1;
        } else if (line[i] == '\"') {
            quote_2 = !quote_2;
        } else if (line[i] == ')' && !quote_2 && !quote_1) {
            new_where = ParseWhereLogicPart(line.substr(0, i), join);
            return new_where;
        } else if (line[i] == '(' && !quote_1 && !quote_2) {
            if (reading_word) {
                last_word_index.back().second = i;
                words.push_back(
                        line.substr(last_word_index.back().first,
                                    last_word_index.back().second - last_word_index.back().first));
                reading_word = false;
            }
            if (words.empty()) {
                contains_brackets = true;
                new_where->left = BuildWhereTree(line.substr(i + 1), index, false, join);
                part = true;
                break;
            } else if (words.size() == 1) {
                if (words[0] != "NOT") {
                    throw WrongInputException(std::string(words[0]));
                }
                contains_brackets = true;
                new_where->left = BuildWhereTree(line.substr(i + 1), index, true, join);
                part = true;
                break;
            } else {
                if (words.back() == "NOT") {
                    if (words[words.size() - 2] == "AND") {
                        new_where->logic_connect = kAnd;
                    } else if (words[words.size() - 2] == "OR") {
                        new_where->logic_connect = kOr;
                    } else {
                        throw WrongInputException(std::string(words[words.size() - 2]));
                    }
                    new_where->left = BuildWhereTree(
                            line.substr(0, last_word_index[last_word_index.size() - 2].first - 1),
                            index, false, join);
                    new_where->right = BuildWhereTree(line.substr(i + 1), index, true, join);
                } else {
                    if (words.back() == "AND") {
                        new_where->logic_connect = kAnd;
                    } else if (words.back() == "OR") {
                        new_where->logic_connect = kOr;
                    } else {
                        throw WrongInputException(std::string(words.back()));
                    }
                    new_where->left = BuildWhereTree(line.substr(0, last_word_index.back().first - 1), index, false,
                                                     join);
                    new_where->right = BuildWhereTree(line.substr(i + 1), index, false, join);

                }

                return new_where;
            }
        } else if (!quote_2 && !quote_1) {
            if (!reading_word) {
                last_word_index.emplace_back();
                last_word_index.back().first = i;
                reading_word = true;
            }
        }
    }
    if (quote_1) {
        throw MissingException("\'", "\'");
    }
    if (quote_2) {
        throw MissingException("\"", "\"");
    }
    reading_word = false;
    if (part) {
        for (size_t i = index - index_begin; i < line.size(); ++i) {
            ++index;
            if (line[i] == ' ' || line[i] == '\n') {
                if (reading_word) {
                    last_word_index.back().second = i;
                    words.push_back(
                            line.substr(last_word_index.back().first,
                                        last_word_index.back().second - last_word_index.back().first));
                    reading_word = false;
                    break;
                }
                continue;
            } else {
                if (!reading_word) {
                    last_word_index.emplace_back();
                    last_word_index.back().first = i;
                    reading_word = true;
                }
            }
        }
        if (words.empty()) {
            new_where->logic_connect = kNone;
            if (!contains_brackets) {
                new_where = ParseWhereLogicPart(line, join);
            }
            return new_where;
        } else if (words.back() == "AND") {
            new_where->logic_connect = kAnd;
        } else if (words.back() == "OR") {
            new_where->logic_connect = kOr;
        } else {
            throw WrongInputException(std::string(words.back()));
        }

        size_t tmp_index = index;
        bool tmp_not = false;
        for (size_t i = index - index_begin; i < line.size(); ++i) {
            if (line[i] == ' ' || line[i] == '\n') {
                index++;
                continue;
            }
            if (line[i] == '(') {
                tmp_index = i + 1;
                break;
            } else if (line[i] == 'N') {
                if (i < line.size() - 3) {
                    if (line[i + 1] == 'O' && line[i + 2] == 'T') {
                        tmp_not = true;
                        i += 3;
                        for (size_t j = i; j < line.size(); ++j) {
                            if (line[j] == ' ' || line[j] == '\n') {
                                index++;
                                continue;
                            }
                            if (line[j] == '(') {
                                tmp_index = j + 1;
                                break;
                            } else {
                                tmp_index = j;
                                break;
                            }
                        }
                        break;
                    } else {
                        tmp_index = i;
                        break;
                    }
                } else {
                    tmp_index = i;
                    break;
                }
            }
        }
        new_where->right = BuildWhereTree(line.substr(tmp_index), index, tmp_not, join);
    }
    if (!contains_brackets) {
        new_where = ParseWhereLogicPart(line, join);
    }
    return new_where;
}

Where* DataBase::ParseWhereLogicPart(std::string_view line, bool join) {
    auto* new_where = new Where;
    size_t index_operator = line.find("AND");
    if (index_operator != std::string::npos) {
        new_where->logic_connect = kAnd;
        new_where->left = ParseWhereLogicPart(line.substr(0, index_operator), join);
        new_where->right = ParseWhereLogicPart(line.substr(index_operator + 3), join);
        return new_where;
    }
    index_operator = line.find("OR");
    if (index_operator != std::string::npos) {
        new_where->logic_connect = kOr;
        new_where->left = ParseWhereLogicPart(line.substr(0, index_operator), join);
        new_where->right = ParseWhereLogicPart(line.substr(index_operator + 2), join);
        return new_where;
    }
    new_where = ParsePredicate(line, join);

    return new_where;
}

Where* DataBase::ParsePredicate(std::string_view line, bool join) {
    auto* new_where = new Where;
    std::string_view first_word = FindNextWord(line);
    if (first_word == "NOT") {
        new_where->is_not = true;
        FindNextWordCut(line);
    }
    std::string_view word;
    if (!join) {
        std::string column_name = std::string(FindNextWordCut(line));
        if (!data[selected.from]->Contains(column_name)) {
            throw WrongInputException(column_name);
        }
        new_where->column.first = selected.from;
        new_where->column.second = column_name;
    } else {
        word = FindNextWordCut(line);
        size_t dot_index = word.find('.');
        if (dot_index == std::string::npos) {
            throw MissingException("ON", "dots in columns");
        }
        std::string table_name = std::string(word.substr(0, dot_index));
        std::string column_name = std::string(word.substr(dot_index + 1));
        if (table_name != selected.from && table_name != selected.join.table) {
            throw WrongInputException(table_name + " table does not exist");
            if (!data[table_name]->Contains(column_name)) {
                throw WrongInputException(column_name + " is not in " + table_name);
            }
        }
        new_where->column.first = std::string(table_name);
        new_where->column.second = std::string(column_name);
    }
    std::string_view compare = FindNextWordCut(line);
    if (compare.size() > 2 || compare.empty()) {
        throw WrongInputException("wrong comparator \"" + std::string(compare) + "\"");
    }
    if (compare == "=") {
        new_where->comparator = kEqual;
    } else if (compare == "<") {
        new_where->comparator = kLess;
    } else if (compare == ">") {
        new_where->comparator = kGreater;
    } else if (compare == ">=") {
        new_where->comparator = kGEq;
    } else if (compare == "<=") {
        new_where->comparator = kLEq;
    } else if (compare == "<>" || compare == "!=") {
        new_where->comparator = kNEq;
    } else if (compare == "IS") {
        std::string_view null = FindNextWordCut(line);
        if (null == "NULL" && data[selected.from]->might_be_null[new_where->column.second]) {
            new_where->comparator = kIsNull;
        } else {
            throw MissingException("IS", "NULL");
        }
        return new_where;
    } else {
        throw WrongInputException("wrong comparator \"" + std::string(compare) + "\"");
    }
    if (new_where->comparator != kIsNull) {
        if (join) {
            word = FindNextWord(line);
            if (!isdigit(word.front()) && word.front() != '\'' && word.front() != '\"' && word.contains('.')) {
                FindNextWordCut(line);
                size_t dot_index = word.find('.');
                if (dot_index == std::string::npos) {
                    throw MissingException("ON", "dots in columns");
                }
                std::string table_name = std::string(word.substr(0, dot_index));
                std::string column_name = std::string(word.substr(dot_index + 1));
                if (table_name != selected.from && table_name != selected.join.table) {
                    throw WrongInputException(table_name + " table does not exist");
                    if (!data[table_name]->Contains(column_name)) {
                        throw WrongInputException(column_name + " is not in " + table_name);
                    }
                    if (data[new_where->column.first]->at(new_where->column.second).index() !=
                        data[table_name]->at(column_name).index()) {
                        throw WrongInputException("Can not compare different data types");
                    }
                }
                new_where->compare_column.first = std::string(table_name);
                new_where->compare_column.second = std::string(column_name);

                return new_where;
            }
        }
        if (std::holds_alternative<Table::holds_varchar>(data[selected.from]->at(new_where->column.second))) {
            size_t left_quote_index = line.find_first_of("\"\'");
            size_t right_quote_index = line.find(line[left_quote_index], left_quote_index + 1);
            if (left_quote_index == std::string::npos || right_quote_index == std::string::npos) {
                throw MissingException(std::string(compare), "quotes");
            }
            std::string_view comparing_with = line.substr(left_quote_index + 1,
                                                          right_quote_index - left_quote_index - 1);
            new_where->compare_with = std::string(comparing_with.substr(0, std::get<Table::holds_varchar>(
                    data[selected.from]->at(new_where->column.second)).second));
        } else if (std::holds_alternative<Table::holds_bool>(data[selected.from]->at(new_where->column.second))) {
            std::string_view comparing_with = FindNextWordCut(line);
            if (comparing_with == "FALSE") {
                new_where->compare_with = kFalse;
            } else if (comparing_with == "TRUE") {
                new_where->compare_with = kTrue;
            } else {
                throw WrongInputException(std::string(comparing_with));
            }
        } else if (std::holds_alternative<Table::holds_int>(data[selected.from]->at(new_where->column.second))) {
            std::string_view comparing_with = FindNextWordCut(line);
            new_where->compare_with = std::stoi(std::string(comparing_with));
        } else if (std::holds_alternative<Table::holds_double>(data[selected.from]->at(new_where->column.second))) {
            std::string_view comparing_with = FindNextWordCut(line);
            new_where->compare_with = std::stod(std::string(comparing_with));
        } else {
            std::string_view comparing_with = FindNextWordCut(line);
            new_where->compare_with = std::stof(std::string(comparing_with));
        }
    }
    return new_where;
}

void DataBase::ParseInsert(std::string_view line) {
    std::string_view word = FindNextWordCut(line);
    if (word != "INTO") {
        throw MissingException("INSERT", "INTO");
    }
    word = FindNextWordCut(line);
    std::string table_name = std::string(word);
    if (!data.contains(table_name)) {
        throw WrongInputException(table_name + " does not exist");
    }
    size_t index = line.find_first_not_of(" \n");
    if (index == std::string::npos) {
        throw MissingException("INSERT INTO");
    }
    std::vector<std::string> columns;
    if (line[index] == '(') {
        line = line.substr(index + 1);
        index = line.find(',');
        while (index != std::string::npos) {
            columns.emplace_back(FindNextWord(line.substr(0, index)));
            line = line.substr(index + 1);
            index = line.find(',');
        }
        word = FindNextWordCut(line);
        if (word.back() == ')') {
            word = word.substr(0, word.size() - 1);
        } else {
            if (FindNextWordCut(line) != ")") {
                throw MissingException("(", ")");
            }
        }
        columns.emplace_back(word);
    }
    ParseValues(line, columns, table_name);
}

void
DataBase::ParseValues(std::string_view line, const std::vector<std::string>& columns, const std::string& table_name) {
    size_t index = line.find('(');
    if (index == std::string::npos) {
        throw MissingException("VALUES");
    }
    std::string_view word = FindNextWord(line.substr(0, index));
    line = line.substr(index);
    if (word != "VALUES") {
        throw WrongInputException(std::string(word));
    }
    index = line.find_first_not_of(" \n");
    if (index == std::string::npos || line[index] != '(') {
        throw MissingException("VALUES");
    }
    line = line.substr(index + 1);
    size_t quantity = 0;
    if (columns.empty()) {
        index = line.find(',');
        while (index != std::string::npos) {
            if (std::holds_alternative<Table::holds_varchar>(
                    data[table_name]->at(data[table_name]->column_order[quantity]))) {
                word = FindVarchar(line);
            } else {
                word = FindNextWord(line.substr(0, index));
            }
            std::string str_tmp = std::string(word);
            if (!CheckForeignKey(table_name, data[table_name]->column_order[quantity], str_tmp)) {
                throw WrongInputException("Can not insert " + str_tmp + " because it confronts with FOREIGN KEY");
            }
            data[table_name]->AddElement(data[table_name]->column_order[quantity], str_tmp);
            index = line.find(',');
            line = line.substr(index + 1);
            index = line.find(',');
            ++quantity;
        }
        if (std::holds_alternative<Table::holds_varchar>(
                data[table_name]->at(data[table_name]->column_order[quantity]))) {
            word = FindVarchar(line);
        } else {
            word = FindNextWord(line.substr(0, index));
        }
        if (word.back() == ')') {
            word = word.substr(0, word.size() - 1);
        } else {
            if (FindNextWordCut(line) != ")") {
                throw MissingException("(", ")");
            }
        }
        std::string str_tmp = std::string(word);
        if (!CheckForeignKey(table_name, data[table_name]->column_order[quantity], str_tmp)) {
            throw WrongInputException("Can not insert " + str_tmp + " because it confronts with FOREIGN KEY");
        }
        data[table_name]->AddElement(data[table_name]->column_order[quantity], str_tmp);
        ++quantity;
        if (word.empty() || quantity != data[table_name]->column_order.size()) {
            throw MissingException("VALUES", "more values");
        }
        return;
    }
    index = line.find(',');
    while (index != std::string::npos) {
        if (std::holds_alternative<Table::holds_varchar>(
                data[table_name]->at(columns[quantity]))) {
            word = FindVarchar(line);
        } else {
            word = FindNextWord(line.substr(0, index));
        }
        std::string str_tmp = std::string(word);
        if (!CheckForeignKey(table_name, columns[quantity], str_tmp)) {
            throw WrongInputException("Can not insert " + str_tmp + " because it confronts with FOREIGN KEY");
        }
        data[table_name]->AddElement(columns[quantity], str_tmp);
        line = line.substr(index + 1);
        index = line.find(',');
        ++quantity;
    }
    if (std::holds_alternative<Table::holds_varchar>(
            data[table_name]->at(columns[quantity]))) {
        word = FindVarchar(line);
    } else {
        word = FindNextWord(line.substr(0, index));
    }
    if (word.back() == ')') {
        word = word.substr(0, word.size() - 1);
    } else {
        if (FindNextWordCut(line) != ")") {
            throw MissingException("(", ")");
        }
    }
    std::string str_tmp = std::string(word);
    if (!CheckForeignKey(table_name, columns[quantity], str_tmp)) {
        throw WrongInputException("Can not insert " + str_tmp + " because it confronts with FOREIGN KEY");
    }
    data[table_name]->AddElement(columns[quantity], str_tmp);
    ++quantity;
    if (word.empty() || quantity != columns.size()) {
        throw MissingException("VALUES", "more values");
    }
}

bool DataBase::CheckForeignKey(const std::string& table_name, const std::string& column, const std::string& element) {
    if (!data[table_name]->foreign_key.contains(column)) {
        return true;
    }
    auto variant = data[data[table_name]->foreign_key[column].first]->at(data[table_name]->foreign_key[column].second);
    if (std::holds_alternative<Table::holds_varchar>(variant)) {
        auto tmp_vector = std::get<Table::holds_varchar>(variant);
        auto varchar = element.substr(0, tmp_vector.second);
        return std::ranges::any_of(tmp_vector.first, [varchar](const std::string& num) { return num == varchar; });
    } else if (std::holds_alternative<Table::holds_int>(variant)) {
        int integer_;
        element == "NULL" ? integer_ = kNullInt : integer_ = std::stoi(element);
        auto tmp_vector = std::get<Table::holds_int>(variant);
        return std::ranges::any_of(tmp_vector, [integer_](int num) { return num == integer_; });
    } else if (std::holds_alternative<Table::holds_float>(variant)) {
        float float_;
        element == "NULL" ? float_ = kNullFloat : float_ = std::stof(element);
        auto tmp_vector = std::get<Table::holds_float>(variant);
        return std::ranges::any_of(tmp_vector,
                                   [float_](float num) { return std::abs(num - float_) <= kMaxDiffFloat; });
    } else if (std::holds_alternative<Table::holds_double>(variant)) {
        double double_;
        element == "NULL" ? double_ = kNullDouble : double_ = std::stod(element);
        auto tmp_vector = std::get<Table::holds_double>(variant);
        return std::ranges::any_of(tmp_vector,
                                   [double_](double num) { return std::abs(num - double_) <= kMaxDiffDouble; });
    } else {
        kBoolVariants type = kNull;
        if (element == "FALSE") {
            type = kFalse;
        } else if (element == "TRUE") {
            type = kTrue;
        }
        auto tmp_vector = std::get<Table::holds_bool>(variant);
        return std::ranges::any_of(tmp_vector, [type](kBoolVariants num) { return type == num; });
    }
    return false;
}

void DataBase::ParseUpdate(std::string_view line) {
    selected.Clear();
    selected.from = std::string(FindNextWordCut(line));
    if (!data.contains(selected.from)) {
        throw WrongInputException("Table " + selected.from + " does not exist");
    }
    std::string_view word = FindNextWordCut(line);
    if (word != "SET") {
        throw MissingException("UPDATE", "SET");
    }
    std::vector<std::pair<std::string, std::string>> set_parameters;
    size_t coma_index = line.find(',');
    while (coma_index != std::string::npos) {
        size_t equal_index = line.find('=');
        if (equal_index == std::string::npos) {
            throw MissingException("SET", "=");
        }
        std::string column = std::string(FindNextWord(line.substr(0, equal_index - 1)));
        std::string element;
        if (std::holds_alternative<Table::holds_varchar>(data[selected.from]->at(column))) {
            std::string_view varchar_tmp = line.substr(equal_index + 1, coma_index - 1);
            element = std::string(FindVarchar(varchar_tmp));
        } else {
            element = std::string(FindNextWord(line.substr(equal_index + 1, coma_index - 1)));
        }
        line = line.substr(coma_index + 1);
        set_parameters.emplace_back(column, element);
    }
    size_t equal_index = line.find('=');
    if (equal_index == std::string::npos) {
        throw MissingException("SET", "=");
    }
    std::string column = std::string(FindNextWord(line.substr(0, equal_index - 1)));
    std::string element;
    if (std::holds_alternative<Table::holds_varchar>(data[selected.from]->at(column))) {
        std::string_view varchar_tmp = line.substr(equal_index + 1);
        element = std::string(FindVarchar(varchar_tmp));
    } else {
        element = std::string(FindNextWord(line.substr(equal_index + 1)));
    }
    line = line.substr(coma_index + 1);
    set_parameters.emplace_back(column, element);
    size_t index_where = line.find("WHERE");
    bool where_exists = false;
    if (index_where != std::string::npos) {
        size_t index = 0;
        where_exists = true;
        selected.where = BuildWhereTree(line.substr(index_where + 5), index, false);
    }
    size_t table_size = data[selected.from]->GetSize();
    for (size_t i = 0; i < table_size; ++i) {
        if (!CheckRowWhere(selected.where, i)) {
            continue;
        }
        for (const auto& parameter: set_parameters) {
            if (data[selected.from]->foreign_key.contains(parameter.first)) {
                auto variant = data[data[selected.from]->foreign_key[parameter.first].first]->at(
                        data[selected.from]->foreign_key[parameter.first].second);
                if (std::holds_alternative<Table::holds_bool>(variant)) {
                    kBoolVariants cmp;
                    if (parameter.second == "FALSE") {
                        cmp = kFalse;
                    } else if (parameter.second == "TRUE") {
                        cmp = kTrue;
                    } else {
                        cmp = kNull;
                    }
                    if (std::none_of(std::get<Table::holds_bool>(variant).begin(),
                                     std::get<Table::holds_bool>(variant).end(),
                                     [cmp](kBoolVariants bool_v) { return bool_v == cmp; })) {
                        throw WrongInputException("FOREIGN KEY interferes UPDATE");
                    }
                }
                if (std::holds_alternative<Table::holds_int>(variant)) {
                    int cmp = std::stoi(parameter.second);
                    if (std::none_of(std::get<Table::holds_int>(variant).begin(),
                                     std::get<Table::holds_int>(variant).end(),
                                     [cmp](int num) { return num == cmp; })) {
                        throw WrongInputException("FOREIGN KEY interferes UPDATE");
                    }
                }
                if (std::holds_alternative<Table::holds_float>(variant)) {
                    float cmp = std::stof(parameter.second);
                    if (std::none_of(std::get<Table::holds_float>(variant).begin(),
                                     std::get<Table::holds_float>(variant).end(),
                                     [cmp](float num) { return std::abs(num - cmp) < kMaxDiffFloat; })) {
                        throw WrongInputException("FOREIGN KEY interferes UPDATE");
                    }
                }
                if (std::holds_alternative<Table::holds_double>(variant)) {
                    double cmp = std::stod(parameter.second);
                    if (std::none_of(std::get<Table::holds_double>(variant).begin(),
                                     std::get<Table::holds_double>(variant).end(),
                                     [cmp](double num) { return std::abs(num - cmp) < kMaxDiffDouble; })) {
                        throw WrongInputException("FOREIGN KEY interferes UPDATE");
                    }
                }
                if (std::holds_alternative<Table::holds_varchar>(variant)) {
                    if (std::none_of(std::get<Table::holds_varchar>(variant).first.begin(),
                                     std::get<Table::holds_varchar>(variant).first.end(),
                                     [parameter](const std::string& line) { return line == parameter.second; })) {
                        throw WrongInputException("FOREIGN KEY interferes UPDATE");
                    }
                }
            }
            data[selected.from]->ChangeElement(parameter.first, parameter.second, i);
        }
    }
}

void DataBase::ParseDelete(std::string_view line) {
    if (FindNextWordCut(line) != "FROM") {
        throw MissingException("DELETE", "FROM");
    }
    selected.Clear();
    selected.from = std::string(ParseFrom(line));
    size_t index = 0;
    size_t index_where = line.find("WHERE");
    if (index_where == std::string::npos) {
        throw MissingException("DELETE", "WHERE");
    }
    selected.where = BuildWhereTree(line.substr(index_where + 5), index, false);
    size_t table_size = data[selected.from]->GetSize();

    for (size_t i = 0; i < table_size; ++i) {
        if (CheckRowWhere(selected.where, i)) {
            for (const auto& table: data) {
                if (table.first == selected.from) {
                    continue;
                }
                for (const auto& f_key: table.second->foreign_key) {
                    if (f_key.second.first == selected.from) {
                        auto variant = data[selected.from]->at(f_key.second.second);
                        if (holds_alternative<Table::holds_bool>(variant)) {
                            kBoolVariants tmp = std::get<Table::holds_bool>(variant).at(i);
                            for (size_t j = 0;
                                 j < std::get<Table::holds_bool>(data[table.first]->at(f_key.first)).size(); ++j) {
                                if (tmp == std::get<Table::holds_bool>(data[table.first]->at(f_key.first)).at(j)) {
                                    throw WrongInputException("FOREIGN KEY interferes DELETE");
                                }
                            }
                        }
                        if (holds_alternative<Table::holds_int>(variant)) {
                            int tmp = std::get<Table::holds_int>(variant).at(i);
                            for (size_t j = 0;
                                 j < std::get<Table::holds_int>(data[table.first]->at(f_key.first)).size(); ++j) {
                                if (tmp == std::get<Table::holds_int>(data[table.first]->at(f_key.first)).at(j)) {
                                    throw WrongInputException("FOREIGN KEY interferes DELETE");
                                }
                            }
                        }
                        if (holds_alternative<Table::holds_float>(variant)) {
                            float tmp = std::get<Table::holds_float>(variant).at(i);
                            for (size_t j = 0;
                                 j < std::get<Table::holds_float>(data[table.first]->at(f_key.first)).size(); ++j) {
                                if (std::abs(
                                        tmp - std::get<Table::holds_float>(data[table.first]->at(f_key.first)).at(j)) <
                                    kMaxDiffFloat) {
                                    throw WrongInputException("FOREIGN KEY interferes DELETE");
                                }
                            }
                        }
                        if (holds_alternative<Table::holds_double>(variant)) {
                            double tmp = std::get<Table::holds_double>(variant).at(i);
                            for (size_t j = 0;
                                 j < std::get<Table::holds_double>(data[table.first]->at(f_key.first)).size(); ++j) {
                                if (std::abs(tmp -
                                             std::get<Table::holds_double>(data[table.first]->at(f_key.first)).at(j)) <
                                    kMaxDiffDouble) {
                                    throw WrongInputException("FOREIGN KEY interferes DELETE");
                                }
                            }
                        }
                        if (holds_alternative<Table::holds_varchar>(variant)) {
                            std::string tmp = std::get<Table::holds_varchar>(variant).first.at(i);
                            for (size_t j = 0;
                                 j < std::get<Table::holds_varchar>(
                                         data[table.first]->at(f_key.first)).first.size(); ++j) {
                                if (std::get<Table::holds_varchar>(data[table.first]->at(f_key.first)).first.at(j) ==
                                    tmp) {
                                    throw WrongInputException("FOREIGN KEY interferes DELETE");
                                }
                            }
                        }
                    }
                }
            }
            data[selected.from]->DeleteRow(i);
            table_size--;
            i--;
        }
    }
}

bool DataBase::CheckRowWhere(Where* node, size_t index, size_t join_index) {
    if (node == nullptr) {
        return true;
    }
    if (node->logic_connect == kOr) {
        return (CheckRowWhere(node->left, index, join_index) ||
                CheckRowWhere(node->right, index, join_index)) ^
               node->is_not;
    }
    if (node->logic_connect == kAnd) {
        return (CheckRowWhere(node->left, index, join_index) &&
                CheckRowWhere(node->right, index, join_index)) ^
               node->is_not;
    }
    if (node->logic_connect == kNone) {
        return (CheckRowWhere(node->left, index, join_index)) ^ node->is_not;
    }
    auto variant = data[node->column.first]->at(node->column.second);
    if (holds_alternative<Table::holds_bool>(variant)) {
        kBoolVariants cmp;
        if (!node->compare_column.first.empty()) {
            cmp = std::get<Table::holds_bool>(data[node->compare_column.first]->at(node->compare_column.second)).at(
                    join_index);
        } else {
            cmp = std::get<kBoolVariants>(node->compare_with);
        }
        if (node->comparator == kIsNull) {
            return std::get<Table::holds_bool>(variant).at(index) == kNull;
        }
        if (node->comparator == kNEq) {
            return std::get<Table::holds_bool>(variant).at(index) != cmp;
        }
        if (node->comparator == kEqual) {
            return std::get<Table::holds_bool>(variant).at(index) == cmp;
        }
        if (node->comparator == kLEq) {
            return std::get<Table::holds_bool>(variant).at(index) <= cmp;
        }
        if (node->comparator == kGEq) {
            return std::get<Table::holds_bool>(variant).at(index) >= cmp;
        }
        if (node->comparator == kGreater) {
            return std::get<Table::holds_bool>(variant).at(index) > cmp;
        }
        if (node->comparator == kLess) {
            return std::get<Table::holds_bool>(variant).at(index) < cmp;
        }
    }
    if (holds_alternative<Table::holds_int>(variant)) {
        int cmp;
        if (!node->compare_column.first.empty()) {
            cmp = std::get<Table::holds_int>(data[node->compare_column.first]->at(node->compare_column.second)).at(
                    join_index);
        } else {
            cmp = std::get<int>(node->compare_with);
        }
        if (node->comparator == kIsNull) {
            return std::get<Table::holds_int>(variant).at(index) == kNullInt;
        }
        if (node->comparator == kNEq) {
            return std::get<Table::holds_int>(variant).at(index) != cmp;
        }
        if (node->comparator == kEqual) {
            return std::get<Table::holds_int>(variant).at(index) == cmp;
        }
        if (node->comparator == kLEq) {
            return std::get<Table::holds_int>(variant).at(index) <= cmp;
        }
        if (node->comparator == kGEq) {
            return std::get<Table::holds_int>(variant).at(index) >= cmp;
        }
        if (node->comparator == kGreater) {
            return std::get<Table::holds_int>(variant).at(index) > cmp;
        }
        if (node->comparator == kLess) {
            return std::get<Table::holds_int>(variant).at(index) < cmp;
        }
    }
    if (holds_alternative<Table::holds_float>(variant)) {
        float cmp;
        if (!node->compare_column.first.empty()) {
            cmp = std::get<Table::holds_float>(data[node->compare_column.first]->at(node->compare_column.second)).at(
                    join_index);
        } else {
            cmp = std::get<float>(node->compare_with);
        }
        if (node->comparator == kIsNull) {
            return std::abs(std::get<Table::holds_float>(variant).at(index) - kNullFloat) < kMaxDiffFloat;
        }
        if (node->comparator == kNEq) {
            return std::abs(std::get<Table::holds_float>(variant).at(index) - cmp) > kMaxDiffFloat;
        }
        if (node->comparator == kEqual) {
            return std::abs(std::get<Table::holds_float>(variant).at(index) - cmp) <= kMaxDiffFloat;
        }
        if (node->comparator == kLEq) {
            return std::get<Table::holds_float>(variant).at(index) < cmp ||
                   std::abs(std::get<Table::holds_float>(variant).at(index) - cmp) <= kMaxDiffFloat;
        }
        if (node->comparator == kGEq) {
            return std::get<Table::holds_float>(variant).at(index) > cmp ||
                   std::abs(std::get<Table::holds_float>(variant).at(index) - cmp) <= kMaxDiffFloat;
        }
        if (node->comparator == kGreater) {
            return std::get<Table::holds_float>(variant).at(index) > cmp &&
                   std::abs(std::get<Table::holds_float>(variant).at(index) - cmp) > kMaxDiffFloat;
        }
        if (node->comparator == kLess) {
            return std::get<Table::holds_float>(variant).at(index) < cmp &&
                   std::abs(std::get<Table::holds_float>(variant).at(index) - cmp) > kMaxDiffFloat;
        }
    }
    if (holds_alternative<Table::holds_double>(variant)) {
        double cmp;
        if (!node->compare_column.first.empty()) {
            cmp = std::get<Table::holds_double>(data[node->compare_column.first]->at(node->compare_column.second)).at(
                    join_index);
        } else {
            cmp = std::get<double>(node->compare_with);
        }
        if (node->comparator == kIsNull) {
            return std::abs(std::get<Table::holds_double>(variant).at(index) - kNullDouble) < kMaxDiffDouble;
        }
        if (node->comparator == kNEq) {
            return std::abs(std::get<Table::holds_double>(variant).at(index) - cmp) > kMaxDiffDouble;
        }
        if (node->comparator == kEqual) {
            return std::abs(std::get<Table::holds_double>(variant).at(index) - cmp) <= kMaxDiffDouble;
        }
        if (node->comparator == kLEq) {
            return std::get<Table::holds_double>(variant).at(index) < cmp ||
                   std::abs(std::get<Table::holds_double>(variant).at(index) - cmp) <= kMaxDiffDouble;
        }
        if (node->comparator == kGEq) {
            return std::get<Table::holds_double>(variant).at(index) > cmp ||
                   std::abs(std::get<Table::holds_double>(variant).at(index) - cmp) <= kMaxDiffDouble;
        }
        if (node->comparator == kGreater) {
            return std::get<Table::holds_double>(variant).at(index) > cmp &&
                   std::abs(std::get<Table::holds_double>(variant).at(index) - cmp) > kMaxDiffDouble;
        }
        if (node->comparator == kLess) {
            return std::get<Table::holds_double>(variant).at(index) < cmp &&
                   std::abs(std::get<Table::holds_double>(variant).at(index) - cmp) > kMaxDiffDouble;
        }
    }
    std::string cmp;
    if (!node->compare_column.first.empty()) {
        cmp = std::get<Table::holds_varchar>(
                data[node->compare_column.first]->at(node->compare_column.second)).first.at(
                join_index);
    } else {
        cmp = std::get<std::string>(node->compare_with);
    }
    if (node->comparator == kIsNull) {
        return std::get<Table::holds_varchar>(variant).first.at(index) == "NULL";
    }
    if (node->comparator == kNEq) {
        return std::get<Table::holds_varchar>(variant).first.at(index) != cmp;
    }
    if (node->comparator == kEqual) {
        return std::get<Table::holds_varchar>(variant).first.at(index) == cmp;
    }
    if (node->comparator == kLEq) {
        return std::get<Table::holds_varchar>(variant).first.at(index) <= cmp;
    }
    if (node->comparator == kGEq) {
        return std::get<Table::holds_varchar>(variant).first.at(index) >= cmp;
    }
    if (node->comparator == kGreater) {
        return std::get<Table::holds_varchar>(variant).first.at(index) > cmp;
    }
    if (node->comparator == kLess) {
        return std::get<Table::holds_varchar>(variant).first.at(index) < cmp;
    }
    return false;
}

void DataBase::PrintSelected() {
    if (!selected.join.exists) {
        for (const auto& column: selected.columns) {
            std::cout << std::setw(kColumnWidth) << column;
        }
        std::cout << "\n";
        auto table = data[selected.from];
        for (size_t i = 0; i < table->GetSize(); ++i) {
            if (!CheckRowWhere(selected.where, i)) {
                continue;
            }
            table->PrintRow(i, selected.columns);
            std::cout << "\n";
        }
        return;
    }

    for (const auto& column: selected.columns) {
        std::cout << std::setw(kColumnWidth) << column;
    }
    for (const auto& column: selected.join.join_columns) {
        std::cout << std::setw(kColumnWidth) << column;
    }
    std::cout << "\n";
    auto left_table = data[selected.from];
    auto right_table = data[selected.join.table];
    if (selected.join.type == kInner) {
        for (size_t i = 0; i < left_table->GetSize(); ++i) {
            for (int j = 0; j < right_table->GetSize(); ++j) {
                if (!CheckRowWhere(selected.where, i)) {
                    continue;
                }
                if (!CheckRowWhere(selected.join.on, i,j)) {
                    continue;
                } else {
                    left_table->PrintRow(i, selected.columns);
                    right_table->PrintRow(j, selected.join.join_columns);
                    std::cout << "\n";
                }
            }
        }
    }
    if (selected.join.type == kLeft) {
        for (size_t i = 0; i < left_table->GetSize(); ++i) {
            bool connect = false;
            for (int j = 0; j < right_table->GetSize(); ++j) {
                if (!CheckRowWhere(selected.where, i)) {
                    continue;
                }
                if (!CheckRowWhere(selected.join.on, i,j)) {
                    continue;
                } else {
                    connect = true;
                    left_table->PrintRow(i, selected.columns);
                    right_table->PrintRow(j, selected.join.join_columns);
                    std::cout << "\n";
                }
            }
            if (!connect) {
                left_table->PrintRow(i, selected.columns);
                for (const auto& j: selected.join.join_columns) {
                    std::cout << std::setw(kColumnWidth) << "NULL";
                }
                std::cout << "\n";
            }
        }
    }
    if (selected.join.type == kRight) {
        for (size_t i = 0; i < right_table->GetSize(); ++i) {
            bool connect = false;
            for (int j = 0; j < left_table->GetSize(); ++j) {
                if (!CheckRowWhere(selected.where, i)) {
                    continue;
                }
                if (!CheckRowWhere(selected.join.on, j,i)) {
                    continue;
                } else {
                    connect = true;
                    left_table->PrintRow(j, selected.columns);
                    right_table->PrintRow(i, selected.join.join_columns);
                    std::cout << "\n";
                }
            }
            if (!connect) {
                for (const auto& j: selected.columns) {
                    std::cout << std::setw(kColumnWidth) << "NULL";
                }
                right_table->PrintRow(i, selected.join.join_columns);
                std::cout << "\n";
            }
        }
    }
}
