#include "lib/CCircularBuffer.h"
#include <gtest/gtest.h>
#include <vector>

auto kEmptyTestBuffer = CCircularBuffer<int>();
auto kIntTestBuffer = CCircularBuffer<int>({0, 1, 2, 3, 4});
std::vector<std::string> kStrTestVector{"I", "love", "eating", "khachapuri"};
std::vector<int> kIntTestVector{6, 9, 4, 2, 0};
std::initializer_list<std::string> kStrTestList{"I", "wanna", "sleep", "so", "badly"};

void PrintBuffer(auto& buffer, std::string& output) {
    for (auto& it: buffer) {
        output += it + " ";
    }
    output.pop_back();
}

void PrintIntBuffer(auto& buffer, std::string& output) {
    for (auto& it: buffer) {
        output += std::to_string(it) + " ";
    }
    output.pop_back();
}

TEST(BufferTest, CapacityTest) {
    CCircularBuffer<std::string> buffer(5808);
    ASSERT_EQ(buffer.capacity(), 5808);
}

TEST(BufferTest, EmptyTest) {
    ASSERT_TRUE(kEmptyTestBuffer.empty());
    ASSERT_FALSE(kIntTestBuffer.empty());
}

TEST(BufferTest, DefaultConstructorTest) {
    CCircularBuffer<std::vector<std::string>> buffer_1;
    ASSERT_TRUE(buffer_1.empty());
    ASSERT_TRUE(buffer_1.cbegin() == buffer_1.cend());
    CCircularBuffer<size_t> buffer_2(96);
    ASSERT_EQ(buffer_2.capacity(), 96);
}

TEST(BufferTest, CopyConstructorTest) {
    auto buffer(kEmptyTestBuffer);
    ASSERT_TRUE(buffer.empty());
    buffer = kIntTestBuffer;
    ASSERT_NO_FATAL_FAILURE(buffer = buffer);
    ASSERT_EQ(buffer[2], 2);
}

TEST(BufferTest, IteratorTest) {
    auto buffer = kIntTestBuffer;
    auto iterator_ = buffer.begin();
    iterator_++;
    ASSERT_EQ(*iterator_, 1);
    iterator_ -= 2;
    ASSERT_EQ(iterator_, buffer.end());
    iterator_ += 4;
    ASSERT_EQ(*iterator_, 3);
    iterator_ -= 3;
    iterator_ = iterator_ + 3;
    ASSERT_EQ(iterator_ - buffer.begin(), 3); //TODO more "-" tests
    ASSERT_EQ(iterator_[3], 0);
}

TEST(BufferTest, QuantityElementConstructorTest) {
    CCircularBuffer<std::string> buffer(300, "spartan");
    for (int i = 0; i < 300; ++i) {
        ASSERT_EQ(buffer[i], "spartan");
    }
}

TEST(BufferTest, OutputTest) {
    auto buffer = CCircularBuffer<std::string>(3, "-=-");
    std::string output;
    PrintBuffer(buffer, output);
    ASSERT_EQ(output, "-=- -=- -=-");
    output.clear();
    for (int i = 0; i < buffer.size(); ++i) {
        output += buffer[i] + " ";
    }
    output.pop_back();
    ASSERT_EQ(output, "-=- -=- -=-");
    output.clear();
    for (auto it = buffer.begin(); it != buffer.end(); ++it) {
        output += *it + " ";
    }
    output.pop_back();
    ASSERT_EQ(output, "-=- -=- -=-");
}

TEST(BufferTest, IteratorConstructorTest) {
    CCircularBuffer<std::string> buffer(kStrTestVector.begin(), kStrTestVector.end());
    std::string output;
    PrintBuffer(buffer, output);
    ASSERT_EQ(output, "I love eating khachapuri");
}

TEST(BufferTest, ListConstructorTest) {
    CCircularBuffer<std::string> buffer(kStrTestList);
    std::string output;
    PrintBuffer(buffer, output);
    ASSERT_EQ(output, "I wanna sleep so badly");
    CCircularBuffer<std::string> copy = kStrTestList;
    ASSERT_TRUE(buffer == copy);
}

TEST(BufferTest, BeginEndTest) {
    auto buffer = kIntTestBuffer;
    size_t sum = buffer.cend()[-1] + buffer.cbegin()[1];
    ASSERT_EQ(sum, 5);
}

TEST(BufferTest, IndexTest) {
    auto buffer = kIntTestBuffer;
    ASSERT_EQ(buffer[3], 3);
    ASSERT_EQ(buffer[5], 0);
    ASSERT_EQ(buffer[150], 0);

    ASSERT_EQ(buffer.at(69), 4);
    buffer = kEmptyTestBuffer;
    ASSERT_ANY_THROW(buffer.at(1));
}

TEST(BufferTest, FrontBackTest) {
    auto buffer = kIntTestBuffer;
    ASSERT_EQ(buffer.front(), 0);
    ASSERT_EQ(buffer.back(), 4);
    buffer.push_back(2023);
    ASSERT_EQ(buffer.front(), 1);
    ASSERT_EQ(buffer.back(), 2023);
}

TEST(BufferTest, PushTest) {
    auto buffer = kIntTestBuffer;
    buffer.push_back(13);
    ASSERT_EQ(buffer.back(), 13);
    ASSERT_EQ(buffer.front(), 1);
    buffer.push_front(0);
    ASSERT_EQ(buffer, kIntTestBuffer);
}

TEST(BufferTest, PopTest) {
    auto buffer = kIntTestBuffer;
    buffer.pop_back();
    ASSERT_EQ(buffer.size(), 4);
    ASSERT_EQ(buffer.back(), 3);
    buffer.pop_front();
    ASSERT_EQ(buffer.size(), 3);
    ASSERT_EQ(buffer.front(), 1);

}

TEST(BufferTest, PushPopIndexTest) {
    auto buffer = kIntTestBuffer;
    buffer.push_back(777);
    buffer.pop_back();
    ASSERT_NE(buffer, kIntTestBuffer);
    for (int i = 0; i < 24; ++i) {
        buffer.push_back(666);
    }
    for (auto& element: buffer) {
        ASSERT_EQ(element, 666);
    }
    for (int i = 0; i < 5; ++i) {
        buffer.pop_back();
    }
    ASSERT_TRUE(buffer.empty());
    ASSERT_NO_FATAL_FAILURE(buffer.pop_back());
    ASSERT_NO_FATAL_FAILURE(buffer.pop_back());
}

TEST (BufferTest, EqualityTest) {
    CCircularBuffer<int> buffer_1(293);
    CCircularBuffer<int> buffer_2;
    ASSERT_EQ(buffer_1, buffer_2);
    buffer_2 = kIntTestBuffer;
    ASSERT_TRUE(buffer_2 != kEmptyTestBuffer);
    buffer_1 = kIntTestBuffer;
    buffer_1.push_back(0);
    buffer_1.push_back(0);
    int x = 0;
    for (int i = 0; i < 5; ++i) {
        buffer_1[i] = x;
        ++x;
    }
    ASSERT_EQ(buffer_1, buffer_2);
    buffer_1.push_back(-3);
    ASSERT_NE(buffer_1, buffer_2);
}

TEST(BufferTest, ClearTest) {
    auto buffer = kIntTestBuffer;
    size_t capacity = buffer.capacity();
    buffer.clear();
    ASSERT_TRUE(buffer.empty());
    ASSERT_EQ(buffer.size(), 0);
    ASSERT_EQ(buffer.capacity(), capacity);
}

TEST(BufferTest, AssignTest) {
    CCircularBuffer<std::string> buffer;
    buffer.assign(kStrTestVector.begin(), kStrTestVector.end());
    std::string output;
    PrintBuffer(buffer, output);
    ASSERT_EQ(output, "I love eating khachapuri");
    output.clear();

    buffer.assign(15, "cm");
    PrintBuffer(buffer, output);
    ASSERT_EQ(output, "cm cm cm cm cm cm cm cm cm cm cm cm cm cm cm");
    output.clear();

    buffer.assign(kStrTestList);
    PrintBuffer(buffer, output);
    ASSERT_EQ(output, "I wanna sleep so badly");
}

TEST(BufferTest, SwapTest) {
    auto buffer_1 = kIntTestBuffer;
    CCircularBuffer<int> buffer_2(kIntTestVector.begin(), kIntTestVector.end());
    auto tmp = buffer_2;
    swap(buffer_1, buffer_2);
    ASSERT_EQ(buffer_2, kIntTestBuffer);
    ASSERT_EQ(buffer_1, tmp);
    buffer_1.swap(buffer_2);
    ASSERT_EQ(buffer_2, tmp);
    ASSERT_EQ(buffer_1, kIntTestBuffer);
}

TEST(BufferTest, ReserveTest) {
    auto buffer = kIntTestBuffer;
    buffer.reserve(1);
    ASSERT_EQ(buffer.size(), 1);
    ASSERT_EQ(buffer.capacity(), 1);
    ASSERT_EQ(buffer[52], 0);

    buffer.reserve(18);
    ASSERT_EQ(buffer.capacity(), 18);
    ASSERT_EQ(buffer.size(), 1);
    buffer.push_back(1);
    ASSERT_EQ(buffer[1], 1);
}

TEST(BufferTest, ResizeTest) {
    auto buffer = kIntTestBuffer;
    buffer.resize(1);
    ASSERT_EQ(buffer.size(), 1);
    ASSERT_EQ(buffer.capacity(), 1);
    ASSERT_EQ(buffer[52], 0);

    buffer.resize(18);
    ASSERT_EQ(buffer.capacity(), 18);
    ASSERT_EQ(buffer.size(), 18);
    for (int i = 0; i < 18; ++i) {
        ASSERT_EQ(buffer[i], 0);
    }
}

TEST(BufferTest, InsertTestList) {
    CCircularBuffer<int> buffer(7);
    std::string output;
    for (int i = 0; i < 3; ++i) {
        buffer.push_back(7);
    }
    buffer.insert(buffer.cbegin() + 1, {0, 9});
    buffer.insert(buffer.cend() - 1, {0, 3});
    buffer.insert(buffer.cbegin() + 3, {0, 2});
    ASSERT_EQ(output, "7 0 9 0 2 7 0 3 7");
}

TEST(BufferTest, InsertTestIterator) {
    CCircularBuffer<int> buffer(7);
    for (int i = 0; i < 3; ++i) {
        buffer.push_back(7);
    }
    buffer.insert(buffer.cbegin() + 1, kIntTestVector.begin(), kIntTestVector.end());
    std::string output;
    PrintIntBuffer(buffer, output);
    ASSERT_EQ(output, "7 6 9 4 2 0 7 7");
}

TEST(BufferTest, InsertTestQuantity) {
    CCircularBuffer<std::string> buffer(7);
    for (int i = 0; i < 3; ++i) {
        buffer.push_back("#");
    }
    buffer.insert(buffer.cbegin() + 1, 2, "@");
    std::string output;
    PrintBuffer(buffer, output);
    ASSERT_EQ(output, "# @ @ # #");
}

TEST(BufferTest, EraseTest) {
    CCircularBuffer<int> buffer = kIntTestBuffer;
    buffer.erase(buffer.cbegin() + 2);
    std::string output;
    PrintIntBuffer(buffer, output);
    ASSERT_EQ(output, "0 1 3 4");
    auto iterator = buffer.erase(buffer.cbegin(), buffer.cend() - 1);
    ASSERT_EQ(iterator, buffer.end());
}

TEST(BufferTest, EpmlaceTest) {
    std::vector<std::pair<int, std::string>> input = {{0, "a"}, {1, "b"}, {2, "c"}, {3, "d"}};
    CCircularBuffer<std::pair<int, std::string>> buffer(input.begin(), input.end());
    buffer.emplace(buffer.cbegin() + 2, 7, "8");
    auto pair_check = std::pair<int, std::string>(7, "8");
    ASSERT_EQ(buffer[1], pair_check);
    buffer.emplace_front(666, "777");
    pair_check = {666, "777"};
    ASSERT_EQ(buffer.front(), pair_check);
    buffer.emplace_back(-3, "yes, woohoo");
    pair_check = {-3, "yes, woohoo"};
    ASSERT_EQ(buffer.back(), pair_check);
}

TEST(BufferTest, FinalBossTest) {
    CCircularBuffer<int> buffer({420, -3, 69, 239, -366, 0});
    std::sort(buffer.begin(), buffer.end());
    std::string output;
    PrintIntBuffer(buffer, output);
    ASSERT_EQ(output, "-366 -3 0 69 239 420");
}