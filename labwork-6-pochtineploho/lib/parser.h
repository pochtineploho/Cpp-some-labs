#pragma once

#include <filesystem>
#include <istream>
#include <vector>
#include <map>

const std::string kDigits = "0123456789";
const std::string kArrayName = "array";
const std::string kIndexName = "List";
const std::string kRoot = "root";
const int kSpacesInTabulation = 4;

enum Type {
    kZero = 0, kInt = 1, kString = 2, kBool = 3, kFloat = 4, kArray = 5
};

class Unit {
public:
    Type what = kZero;
    float float_{};
    int int_{};
    std::string string_;
    bool bool_{};
    std::vector<Unit> array_;

    const Unit& operator[](size_t index) const;

    bool IsInt() const;

    bool IsFloat() const;

    bool IsString() const;

    bool IsBool() const;

    bool IsArray() const;

    int AsInt() const;

    float AsFloat() const;

    const std::string& AsString() const;

    bool AsBool() const;

    int AsIntOrDefault(int number) const;

    float AsFloatOrDefault(float number) const;

    const std::string AsStringOrDefault(const std::string& line) const;
};

const Unit kEmptyUnit = Unit();

namespace omfl {

    class Section {
    public:
        const Section& Get(const std::string& directory) const;

        const Unit& operator[](size_t index) const;

        bool PlaceUnit(std::string& directory, const Unit& unit_value, omfl::Section& current_section);

        bool IsInt() const;

        bool IsFloat() const;

        bool IsString() const;

        bool IsBool() const;

        bool IsArray() const;

        int AsInt() const;

        float AsFloat() const;

        const std::string& AsString() const;

        bool AsBool() const;

        int AsIntOrDefault(int number) const;

        float AsFloatOrDefault(float number) const;

        const std::string AsStringOrDefault(const std::string& line) const;

        Unit unit;
        std::map<std::string, Section> sections;
        bool is_key = false;

    };

    class OMFL {

    public:
        std::filesystem::path path;

        bool valid() const;

        Unit KeyWork(std::string_view& line);

        void PlaceUnit(std::string& key, const Unit& unit_value);

        void CheckSection(const std::string_view& line);

        const Section& Get(const std::string& directory) const;

        void ToXml(const std::string& name);

        void ToJson(const std::string& name);

        void ToYaml(const std::string& name);

    private:

        void FillXml(const Section& section, std::ofstream& file, int tabulation);

        void UnitValueXml(const Unit& unit, std::ofstream& file, int tabulation, const std::string& key);

        void ArrayXml(const Unit& unit, std::ofstream& file, int tabulation);

        void FillJson(const Section& section, std::ofstream& file, int tabulation);

        void UnitValueJson(const Unit& unit, std::ofstream& file, int tabulation, const std::string& key);

        void ArrayJson(const Unit& unit, std::ofstream& file, int tabulation);

        void FillYaml(const Section& section, std::ofstream& file, int tabulation);

        void UnitValueYaml(const Unit& unit, std::ofstream& file, int tabulation, const std::string& key);

        void ArrayYaml(const Unit& unit, std::ofstream& file, int tabulation);

        bool valid_ = true;

        Section all_sections;

        void CheckName(const std::string_view& line);

        void
        FillUnit(Unit& unit, std::string_view& line, std::vector<std::pair<size_t, size_t>>& brackets, int bracket);

        void FillUnitValue(Unit& unit, std::string_view& line, std::vector<std::pair<size_t, size_t>>& brackets,
                           int bracket);

        void FillUnitArray(Unit& unit, std::string_view& line, std::vector<std::pair<size_t, size_t>>& brackets,
                           int bracket_new);
    };

    OMFL parse(const std::filesystem::path& path);

    OMFL parse(const std::string& str);

    std::string_view& DeleteTrash(std::string_view& line);

    bool BoolIntNumber(std::string_view& line);

    bool BoolFloatNumber(std::string_view& line);

    bool CheckNameS(const std::string_view& line);

    size_t FindComma(std::string_view& line, size_t index);

    std::string Tabulation(int size);
}// namespace
