#include "lib/CCircularBufferExt.h"
#include <gtest/gtest.h>
#include <vector>

auto kEmptyTestBufferExt = CCircularBufferExt<int>();
auto kIntTestBufferExt = CCircularBufferExt<int>({0, 1, 2, 3, 4});
std::vector<std::string> kStrExtTestVector{"I", "love", "eating", "khachapuri"};
std::vector<int> kIntExtTestVector{6, 9, 4, 2, 0};
std::initializer_list<std::string> kStrExtTestList{"I", "wanna", "sleep", "so", "badly"};

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

TEST(BufferExtTest, ExtCapacityTest) {
    CCircularBufferExt<std::string> buffer(5808);
    ASSERT_EQ(buffer.capacity(), 5808);
}

TEST(BufferExtTest, ExtEmptyTest) {
    ASSERT_TRUE(kEmptyTestBufferExt.empty());
    ASSERT_FALSE(kIntTestBufferExt.empty());
}

TEST(BufferExtTest, ExtDefaultConstructorTest) {
    CCircularBufferExt<std::vector<std::string>> buffer_1;
    ASSERT_TRUE(buffer_1.empty());
    ASSERT_TRUE(buffer_1.cbegin() == buffer_1.cend());
    CCircularBufferExt<size_t> buffer_2(96);
    ASSERT_EQ(buffer_2.capacity(), 96);
}

TEST(BufferExtTest, ExtCopyConstructorTest) {
    auto buffer(kEmptyTestBufferExt);
    ASSERT_TRUE(buffer.empty());
    buffer = kIntTestBufferExt;
    ASSERT_NO_FATAL_FAILURE(buffer = buffer);
    ASSERT_EQ(buffer[2], 2);
}

TEST(BufferExtTest, ExtIteratorTest) {
    auto buffer = kIntTestBufferExt;
    auto iterator_ = buffer.begin();
    iterator_++;
    ASSERT_EQ(*iterator_, 1);
    iterator_ -= 2;
    ASSERT_EQ(iterator_, buffer.end());
    iterator_ += 4;
    ASSERT_EQ(*iterator_, 3);
    iterator_ -= 3;
    iterator_ = iterator_ + 3;
    ASSERT_EQ(iterator_ - buffer.begin(), 3);
    ASSERT_EQ(iterator_[3], 0);
}

TEST(BufferExtTest, ExtQuantityElementConstructorTest) {
    CCircularBufferExt<std::string> buffer(300, "spartan");
    for (int i = 0; i < 300; ++i) {
        ASSERT_EQ(buffer[i], "spartan");
    }
}

TEST(BufferExtTest, ExtOutputTest) {
    auto buffer = CCircularBufferExt<std::string>(3, "-=-");
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

TEST(BufferExtTest, ExtIteratorConstructorTest) {
    CCircularBufferExt<std::string> buffer(kStrExtTestVector.begin(), kStrExtTestVector.end());
    std::string output;
    PrintBuffer(buffer, output);
    ASSERT_EQ(output, "I love eating khachapuri");
}

TEST(BufferExtTest, ExtListConstructorTest) {
    CCircularBufferExt<std::string> buffer(kStrExtTestList);
    std::string output;
    PrintBuffer(buffer, output);
    ASSERT_EQ(output, "I wanna sleep so badly");
    CCircularBufferExt<std::string> copy = kStrExtTestList;
    ASSERT_TRUE(buffer == copy);
}

TEST(BufferExtTest, ExtBeginEndTest) {
    auto buffer = kIntTestBufferExt;
    size_t sum = buffer.cend()[-1] + buffer.cbegin()[1];
    ASSERT_EQ(sum, 5);
}

TEST(BufferExtTest, ExtIndexTest) {
    auto buffer = kIntTestBufferExt;
    ASSERT_EQ(buffer[3], 3);
    ASSERT_EQ(buffer[5], 0);
    ASSERT_EQ(buffer[150], 0);

    ASSERT_EQ(buffer.at(69), 4);
    buffer = kEmptyTestBufferExt;
    ASSERT_ANY_THROW(buffer.at(1));
}

TEST(BufferExtTest, ExtFrontBackTest) {
    auto buffer = kIntTestBufferExt;
    ASSERT_EQ(buffer.front(), 0);
    ASSERT_EQ(buffer.back(), 4);
    buffer.push_back(2023);
    ASSERT_EQ(buffer.front(), 0);
    ASSERT_EQ(buffer.back(), 2023);
}

TEST(BufferExtTest, ExtPushTest) {
    auto buffer = kIntTestBufferExt;
    buffer.push_back(13);
    ASSERT_EQ(buffer.back(), 13);
    ASSERT_EQ(buffer.front(), 0);
    buffer.push_front(-3);
    ASSERT_EQ(buffer.front(), -3);
}

TEST(BufferExtTest, ExtPopTest) {
    auto buffer = kIntTestBufferExt;
    buffer.pop_back();
    ASSERT_EQ(buffer.size(), 4);
    ASSERT_EQ(buffer.back(), 3);
    buffer.pop_front();
    ASSERT_EQ(buffer.size(), 3);
    ASSERT_EQ(buffer.front(), 1);

}

TEST(BufferExtTest, ExtPushPopIndexTest) {
    auto buffer = kIntTestBufferExt;
    buffer.push_back(777);
    buffer.pop_back();
    ASSERT_EQ(buffer, kIntTestBufferExt);
    for (int i = 0; i < 24; ++i) {
        buffer.push_back(666);
    }
    for (int i = 5; i < 29; ++i) {
        ASSERT_EQ(buffer[i], 666);
    }
    for (int i = 0; i < 5; ++i) {
        buffer.pop_back();
    }
    ASSERT_FALSE(buffer.empty());
}

TEST (BufferExtTest, ExtEqualityTest) {
    CCircularBufferExt<int> buffer_1(293);
    CCircularBufferExt<int> buffer_2;
    ASSERT_EQ(buffer_1, buffer_2);
    buffer_2 = kIntTestBufferExt;
    ASSERT_NE(buffer_2, kEmptyTestBufferExt);
    buffer_1 = kIntTestBufferExt;
    ASSERT_EQ(buffer_1, buffer_2);
}

TEST(BufferExtTest,ExtClearTest) {
    auto buffer = kIntTestBufferExt;
    size_t capacity = buffer.capacity();
    buffer.clear();
    ASSERT_TRUE(buffer.empty());
    ASSERT_EQ(buffer.size(), 0);
    ASSERT_EQ(buffer.capacity(), capacity);
}

TEST(BufferExtTest, ExtAssignTest) {
    CCircularBufferExt<std::string> buffer;
    buffer.assign(kStrExtTestVector.begin(), kStrExtTestVector.end());
    std::string output;
    PrintBuffer(buffer, output);
    ASSERT_EQ(output, "I love eating khachapuri");
    output.clear();

    buffer.assign(15, "cm");
    PrintBuffer(buffer, output);
    ASSERT_EQ(output, "cm cm cm cm cm cm cm cm cm cm cm cm cm cm cm");
    output.clear();

    buffer.assign(kStrExtTestList);
    PrintBuffer(buffer, output);
    ASSERT_EQ(output, "I wanna sleep so badly");
}

TEST(BufferExtTest, ExtSwapTest) {
    auto buffer_1 = kIntTestBufferExt;
    CCircularBufferExt<int> buffer_2(kIntExtTestVector.begin(), kIntExtTestVector.end());
    auto tmp = buffer_2;
    swap(buffer_1, buffer_2);
    ASSERT_EQ(buffer_2, kIntTestBufferExt);
    ASSERT_EQ(buffer_1, tmp);
    buffer_1.swap(buffer_2);
    ASSERT_EQ(buffer_2, tmp);
    ASSERT_EQ(buffer_1, kIntTestBufferExt);
}

TEST(BufferExtTest, ExtReserveTest) {
    auto buffer = kIntTestBufferExt;
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

TEST(BufferExtTest, ExtResizeTest) {
    auto buffer = kIntTestBufferExt;
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

TEST(BufferExtTest, ExtInsertTestList) {
    auto buffer = kIntTestBufferExt;
    auto iterator = buffer.insert(buffer.cend(), {20000, 585});
    ASSERT_EQ(*iterator, 20000);
    std::string output;
    PrintIntBuffer(buffer, output);
    ASSERT_EQ(output, "0 1 2 3 4 20000 585");
}

TEST(BufferExtTest, ExtInsertTestIterator) {
    auto buffer = kIntTestBufferExt;
    buffer.insert(buffer.cbegin(), kIntExtTestVector.begin(), kIntExtTestVector.end());
    std::string output;
    PrintIntBuffer(buffer, output);
    ASSERT_EQ(output, "6 9 4 2 0 0 1 2 3 4");
}

TEST(BufferExtTest, ExtInsertTestQuantity) {
    auto buffer = kIntTestBufferExt;
    buffer.insert(buffer.cbegin(), 4, 4);
    std::string output;
    PrintIntBuffer(buffer, output);
    ASSERT_EQ(output, "4 4 4 4 0 1 2 3 4");
    output.clear();
    buffer.insert(buffer.cend(), 330);
    PrintIntBuffer(buffer, output);
    ASSERT_EQ(output, "4 4 4 4 0 1 2 3 4 330");
}

TEST(BufferExtTest, ExtEraseTest) {
    CCircularBufferExt<int> buffer = kIntTestBufferExt;
    buffer.erase(buffer.cbegin() + 2);
    std::string output;
    PrintIntBuffer(buffer, output);
    ASSERT_EQ(output, "0 1 3 4");
    auto iterator = buffer.erase(buffer.cbegin(), buffer.cend() - 1);
    ASSERT_EQ(iterator, buffer.end());
}

TEST(BufferExtTest, ExtEpmlaceTest) {
    std::vector<std::pair<int, std::string>> input = {{0, "a"}, {1, "b"}, {2, "c"}, {3, "d"}};
    CCircularBufferExt<std::pair<int, std::string>> buffer(input.begin(), input.end());
    buffer.emplace(buffer.cbegin() + 2, 7, "8");
    auto pair_check = std::pair<int, std::string>(7, "8");
    ASSERT_EQ(buffer[2], pair_check);
    buffer.emplace_front(666, "777");
    pair_check = {666, "777"};
    ASSERT_EQ(buffer.front(), pair_check);
    buffer.emplace_back(-3, "yes, woohoo");
    pair_check = {-3, "yes, woohoo"};
    ASSERT_EQ(buffer.back(), pair_check);
}

TEST(BufferExtTest, ExtFinalBossTest) {
    CCircularBufferExt<int> buffer({420, -3, 69, 239, -366, 0});
    std::sort(buffer.begin(), buffer.end());
    std::string output;
    PrintIntBuffer(buffer, output);
    ASSERT_EQ(output, "-366 -3 0 69 239 420");
}
