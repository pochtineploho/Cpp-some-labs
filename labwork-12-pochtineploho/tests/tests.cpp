#include "lib/MyCoolDB.h"

#include <gtest/gtest.h>
#include <fstream>

TEST(UploadSave, CheckEqual) {
    MyCoolDB data;
    data.Upload(R"(D:\CLion\labwork-12-pochtineploho\tests\upload.bo)");
    data.Save(R"(D:\CLion\labwork-12-pochtineploho\tests\save_1.bo)");
    std::ifstream input(R"(D:\CLion\labwork-12-pochtineploho\tests\upload.bo)");
    std::ifstream output(R"(D:\CLion\labwork-12-pochtineploho\tests\save_1.bo)");
    while (!input.eof()) {
        std::string str_1;
        std::string str_2;
        std::getline(input, str_1);
        std::getline(output, str_2);
        ASSERT_EQ(str_1, str_2);
    }
}

TEST(Request, CreateTable) {
    MyCoolDB data;
    std::ifstream input(R"(D:\CLion\labwork-12-pochtineploho\tests\upload.bo)");
    data.Request(R"(
CREATE TABLE Students (
id INT PRIMARY KEY,
name VARCHAR(20),
programming FLOAT
);
INSERT INTO Students
VALUES (367917, "Bo", 89.9);
INSERT INTO Students
VALUES (239239, "Nikita", 4.5);
INSERT INTO Students
VALUES (123123, "Sveta", 98.999);
INSERT INTO Students
VALUES (967231, "Tolik", 93);
INSERT INTO Students
VALUES (234561, "Vlad", 3.8);

CREATE TABLE Unis (
id INT PRIMARY KEY,
name VARCHAR(20),
ege DOUBLE,
bo_there BOOL
);

INSERT INTO Unis
VALUES(1, "ITMO", 307, TRUE);
INSERT INTO Unis
VALUES(2, "SPBGU", 270, FALSE);
INSERT INTO Unis
VALUES(3, "SPBGUT", 240, FALSE);
INSERT INTO Unis
VALUES(4, "VSE", 290, FALSE);
)");
    data.Save(R"(D:\CLion\labwork-12-pochtineploho\tests\save_1.bo)");
    std::ifstream output(R"(D:\CLion\labwork-12-pochtineploho\tests\save_1.bo)");
    while (!input.eof()) {
        std::string str_1;
        std::string str_2;
        std::getline(input, str_1);
        std::getline(output, str_2);
        ASSERT_EQ(str_1, str_2);
    }
}

TEST(Request, Update) {
    std::ifstream compare(R"(D:\CLion\labwork-12-pochtineploho\tests\compare_2.bo)");
    std::ifstream output(R"(D:\CLion\labwork-12-pochtineploho\tests\save_2.bo)");
    MyCoolDB data;
    data.Upload(R"(D:\CLion\labwork-12-pochtineploho\tests\upload.bo)");
    data.Request(R"(UPDATE Students
SET programming = -3
WHERE id = 367917;)");
    data.Save(R"(D:\CLion\labwork-12-pochtineploho\tests\save_2.bo)");
    while (!compare.eof()) {
        std::string str_1;
        std::string str_2;
        std::getline(compare, str_1);
        std::getline(output, str_2);
        ASSERT_EQ(str_1, str_2);
    }
}

TEST(Request, Delete) {
    std::ifstream compare(R"(D:\CLion\labwork-12-pochtineploho\tests\compare_3.bo)");
    std::ifstream output(R"(D:\CLion\labwork-12-pochtineploho\tests\save_3.bo)");
    MyCoolDB data;
    data.Upload(R"(D:\CLion\labwork-12-pochtineploho\tests\upload.bo)");
    data.Request(R"(DELETE FROM Students
WHERE id = 367917 OR programming > 90;)");
    data.Save(R"(D:\CLion\labwork-12-pochtineploho\tests\save_3.bo)");
    while (!compare.eof()) {
        std::string str_1;
        std::string str_2;
        std::getline(compare, str_1);
        std::getline(output, str_2);
        ASSERT_EQ(str_1, str_2);
    }
}

TEST(Request, DropTable) {
    std::ifstream compare(R"(D:\CLion\labwork-12-pochtineploho\tests\compare_4.bo)");
    std::ifstream output(R"(D:\CLion\labwork-12-pochtineploho\tests\save_4.bo)");
    MyCoolDB data;
    data.Upload(R"(D:\CLion\labwork-12-pochtineploho\tests\upload.bo)");
    data.Request(R"(DROP TABLE Students;)");
    data.Save(R"(D:\CLion\labwork-12-pochtineploho\tests\save_4.bo)");
    while (!compare.eof()) {
        std::string str_1;
        std::string str_2;
        std::getline(compare, str_1);
        std::getline(output, str_2);
        ASSERT_EQ(str_1, str_2);
    }
}

TEST(Request, Select) {
    MyCoolDB data;
    data.Upload(R"(D:\CLion\labwork-12-pochtineploho\tests\upload.bo)");
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    data.Request(R"(SELECT id
FROM Students
WHERE NOT (name = "Sveta") AND (id < 900000 OR programming >= 60);)");
    std::cout.rdbuf(old);
    std::string output = oss.str();
    ASSERT_EQ(output, "                  id\n              123123\n");
}

TEST(Request, Join) {
    std::ifstream compare(R"(D:\CLion\labwork-12-pochtineploho\tests\compare_5.bo)");
    MyCoolDB data;
    data.Upload(R"(D:\CLion\labwork-12-pochtineploho\tests\upload_2.bo)");
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    data.Request(R"(SELECT Students.name, Students.programming, Unis.ege, Unis.uni
FROM Students
INNER JOIN Unis ON Students.name = Unis.name;)");
    std::cout << "\n";
    std::cout << "\n";
    data.Request(R"(SELECT Students.name, Students.programming, Unis.ege, Unis.uni
FROM Students
LEFT JOIN Unis ON Students.name = Unis.name;)");
    std::cout << "\n";
    std::cout << "\n";
    data.Request(R"(SELECT Students.name, Students.programming, Unis.ege, Unis.uni
FROM Students
RIGHT JOIN Unis ON Students.name = Unis.name;)");
    std::cout.rdbuf(old);
    std::string output = oss.str();
    ASSERT_EQ(output, "                name         programming                 ege                 uni\n                  Bo                89.9                 307                ITMO\n               Tolik                  93                 240              SPBGUT\n\n\n                name         programming                 ege                 uni\n                  Bo                89.9                 307                ITMO\n              Nikita                 4.5                NULL                NULL\n               Sveta              98.999                NULL                NULL\n               Tolik                  93                 240              SPBGUT\n                Vlad                 3.8                NULL                NULL\n\n\n                name         programming                 ege                 uni\n                  Bo                89.9                 307                ITMO\n                NULL                NULL                 270               SPBGU\n               Tolik                  93                 240              SPBGUT\n                NULL                NULL                 290                 VSE\n");
}
