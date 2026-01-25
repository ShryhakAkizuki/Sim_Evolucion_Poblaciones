#include <gtest/gtest.h>
#include "data_structures/DynamicArray.hpp"

// ----- Funciones especiales -----
TEST(DynamicArrayTest, DefaultConstructor) {
    DynamicArray<int> arr;
    EXPECT_EQ(arr.size(), 0);
    EXPECT_EQ(arr.capacity(), 0);
    EXPECT_TRUE(arr.empty());
}

TEST(DynamicArrayTest, ConstructorWithCapacity) {
    DynamicArray<int> arr(Reserve, 10);
    EXPECT_EQ(arr.size(), 0);
    EXPECT_EQ(arr.capacity(), 10);
    EXPECT_TRUE(arr.empty());
}

TEST(DynamicArrayTest, ConstructorWithValue) {
    DynamicArray<int> arr(5, 42);
    EXPECT_EQ(arr.size(), 5);
    EXPECT_EQ(arr.capacity(), 5);
    EXPECT_FALSE(arr.empty());
    
    for (size_t i = 0; i < arr.size(); ++i) {
        EXPECT_EQ(arr[i], 42);
    }
}

TEST(DynamicArrayTest, ConstructorWithForwardValue) {
    DynamicArray<int> arr(5, int(42));
    EXPECT_EQ(arr.size(), 5);
    EXPECT_EQ(arr.capacity(), 5);
    EXPECT_FALSE(arr.empty());
    
    for (size_t i = 0; i < arr.size(); ++i) {
        EXPECT_EQ(arr[i], 42);
    }
}

TEST(DynamicArrayTest, ConstructorWithList) {
    DynamicArray<int> arr = {1, 2, 3, 4, 5};
    EXPECT_EQ(arr.size(), 5);
    EXPECT_EQ(arr.capacity(), 5);
    EXPECT_FALSE(arr.empty());
    
    for (size_t i = 0; i < arr.size(); ++i) {
        EXPECT_EQ(arr[i], i+1);
    }
}

TEST(DynamicArrayTest, ConstructorWithSpan) {
    int tmp[] = {1,2,3,4,5};
    auto s = std::span<int>(tmp);
    DynamicArray<int> arr(s);    
    EXPECT_EQ(arr.size(), 5);
    EXPECT_EQ(arr.capacity(), 5);
    EXPECT_FALSE(arr.empty());
    
    for (size_t i = 0; i < arr.size(); ++i) {
        EXPECT_EQ(arr[i], i+1);
    }
}

TEST(DynamicArrayTest, ConstructorWithIteratos) {
    DynamicArray<int> arr1(5, 42);
    EXPECT_EQ(arr1.size(), 5);
    EXPECT_EQ(arr1.capacity(), 5);
    EXPECT_FALSE(arr1.empty());

    DynamicArray<int> arr2(arr1.begin(), arr1.end());
    EXPECT_EQ(arr2.size(), 5);
    EXPECT_EQ(arr2.capacity(), 5);
    EXPECT_FALSE(arr2.empty());

    for (size_t i = 0; i < arr2.size(); ++i) {
        EXPECT_EQ(arr1[i], 42);
        EXPECT_EQ(arr2[i], 42);
    }
}

// ----- Acceso de elementos -----
TEST(DynamicArrayTest, AtMethod) {
    DynamicArray<int> arr = {1, 2, 3, 4, 5};
    
    EXPECT_EQ(arr.at(0), 1);
    EXPECT_EQ(arr.at(2), 3);
    EXPECT_EQ(arr.at(4), 5);
    
    EXPECT_THROW(arr.at(10), std::out_of_range);
    EXPECT_THROW(arr.at(-2), std::out_of_range);
}

TEST(DynamicArrayTest, Front_Back) {
    DynamicArray<int> arr;    
    EXPECT_THROW(arr.front(), std::out_of_range);
    EXPECT_THROW(arr.back(), std::out_of_range);

    arr = DynamicArray<int>({1, 2, 3, 4, 5});   
    
    EXPECT_EQ(arr.front(), 1);
    EXPECT_EQ(arr.back(), 5);
}

TEST(DynamicArrayTest, Data) {
    DynamicArray<int> arr;    
    EXPECT_EQ(arr.data(), nullptr);
    
    arr = DynamicArray<int>({1, 2, 3, 4, 5});    
    EXPECT_NE(arr.data(), nullptr);
}

// ----- Iteradores -----
TEST(DynamicArrayTest, Iterator_Begin) {
    DynamicArray<int> arr;    
    EXPECT_EQ(arr.begin(), nullptr);
    
    arr.push_back(1);    
    EXPECT_NE(arr.begin(), nullptr);
    EXPECT_EQ(*arr.begin(), 1);
}

TEST(DynamicArrayTest, Iterator_End) {
    DynamicArray<int> arr;    
    EXPECT_EQ(arr.end(), nullptr);
    
    arr.push_back(1);    
    EXPECT_NE(arr.end(), nullptr);
    EXPECT_EQ(*(arr.end()-1), 1);
}

TEST(DynamicArrayTest, Iterator_Insert_1) {
    DynamicArray<int> arr;  
    
    EXPECT_NO_THROW(arr.insert(arr.begin(),100));
    EXPECT_EQ(arr.size(), 1);
    EXPECT_EQ(arr[0], 100);
    
    arr.push_back(1);  
    EXPECT_EQ(arr.size(), 2);
    EXPECT_EQ(arr[1], 1);

    arr.insert(arr.begin(),5);
    EXPECT_EQ(arr.front(),5);
    EXPECT_EQ(arr.size(), 3);
    EXPECT_EQ(arr[0], 5);
    
    arr.insert(arr.end(),10);
    EXPECT_EQ(arr.back(),10);
    EXPECT_EQ(arr.size(), 4);
    EXPECT_EQ(arr[3], 10);

    EXPECT_EQ(arr[0],5);
    EXPECT_EQ(arr[1],100);
    EXPECT_EQ(arr[2],1);
    EXPECT_EQ(arr[3],10);
}

TEST(DynamicArrayTest, Iterator_Insert_2) {
    DynamicArray<int> arr = {1, 2, 4, 5};
    
    auto it = arr.insert(arr.begin() + 2, 3);
    EXPECT_EQ(arr.size(), 5);
    
    EXPECT_EQ(*it, 3);
    
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
    EXPECT_EQ(arr[3], 4);
    EXPECT_EQ(arr[4], 5);
}

TEST(DynamicArrayTest, Iterator_Insert_3) {
    DynamicArray<int> arr = {1, 2, 4, 5};
    
    auto it_1 = arr.insert(arr.begin() + 2, 3);
    EXPECT_EQ(arr.size(), 5);
    
    EXPECT_EQ(*it_1, 3);

    auto it_2 = arr.insert(arr.begin(), 0);
    EXPECT_EQ(arr.size(), 6);
    
    EXPECT_EQ(*it_2, 0);

    auto it_3 = arr.insert(arr.end(), 6);
    EXPECT_EQ(arr.size(), 7);
    
    EXPECT_EQ(*it_3, 6);

    auto it_4 = arr.insert(arr.end()-2, 5);
    EXPECT_EQ(arr.size(), 8);
    
    EXPECT_EQ(*it_4, 5);

    EXPECT_EQ(arr[0], 0);
    EXPECT_EQ(arr[1], 1);
    EXPECT_EQ(arr[2], 2);
    EXPECT_EQ(arr[3], 3);
    EXPECT_EQ(arr[4], 4);
    EXPECT_EQ(arr[5], 5);
    EXPECT_EQ(arr[6], 5);
    EXPECT_EQ(arr[7], 6);
}

TEST(DynamicArrayTest, Iterator_Insert_Range) {
    DynamicArray<int> arr = {1, 2, 4, 5};
    DynamicArray<int> arr2;

    auto it_pointer = arr2.insert(arr2.begin(), arr.begin()+1, arr.end());

    EXPECT_EQ(*it_pointer, 2);
    EXPECT_EQ(arr2.size(), 3);
    EXPECT_EQ(arr2[0], 2);
    EXPECT_EQ(arr2[1], 4);
    EXPECT_EQ(arr2[2], 5);

    it_pointer = arr2.insert(arr2.end(), arr.begin(), arr.end());

    EXPECT_EQ(*it_pointer, 1);
    EXPECT_EQ(arr2.size(), 7);
    EXPECT_EQ(arr2[3], 1);
    EXPECT_EQ(arr2[4], 2);
    EXPECT_EQ(arr2[5], 4);
    EXPECT_EQ(arr2[6], 5);

    it_pointer = arr2.insert(arr2.begin()+4, arr.begin()+2, arr.begin()+3);

    EXPECT_EQ(*it_pointer, 4);
    EXPECT_EQ(arr2.size(), 8);
    EXPECT_EQ(arr2[4], 4);
}

TEST(DynamicArrayTest, Iterator_Emplace_1) {
    DynamicArray<int> arr;  
    
    EXPECT_NO_THROW(arr.emplace(arr.begin(),int(100)));
    EXPECT_EQ(arr.size(), 1);
    EXPECT_EQ(arr[0], 100);
    
    arr.push_back(1);  
    EXPECT_EQ(arr.size(), 2);
    EXPECT_EQ(arr[1], 1);

    arr.emplace(arr.begin(),int(5));
    EXPECT_EQ(arr.front(),5);
    EXPECT_EQ(arr.size(), 3);
    EXPECT_EQ(arr[0], 5);
    
    arr.emplace(arr.end(),int(10));
    EXPECT_EQ(arr.back(),10);
    EXPECT_EQ(arr.size(), 4);
    EXPECT_EQ(arr[3], 10);

    EXPECT_EQ(arr[0],5);
    EXPECT_EQ(arr[1],100);
    EXPECT_EQ(arr[2],1);
    EXPECT_EQ(arr[3],10);
}

TEST(DynamicArrayTest, Iterator_Emplace_2) {
    DynamicArray<int> arr = {1, 2, 4, 5};
    
    auto it = arr.emplace(arr.begin() + 2, int(3));
    EXPECT_EQ(arr.size(), 5);
    
    EXPECT_EQ(*it, 3);
    
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
    EXPECT_EQ(arr[3], 4);
    EXPECT_EQ(arr[4], 5);
}

TEST(DynamicArrayTest, Iterator_Emplace_3) {
    DynamicArray<int> arr = {1, 2, 4, 5};
    
    auto it_1 = arr.emplace(arr.begin() + 2, int(3));
    EXPECT_EQ(arr.size(), 5);
    
    EXPECT_EQ(*it_1, 3);

    auto it_2 = arr.emplace(arr.begin(), int(0));
    EXPECT_EQ(arr.size(), 6);
    
    EXPECT_EQ(*it_2, 0);

    auto it_3 = arr.emplace(arr.end(), int(6));
    EXPECT_EQ(arr.size(), 7);
    
    EXPECT_EQ(*it_3, 6);

    auto it_4 = arr.emplace(arr.end()-2, int(5));
    EXPECT_EQ(arr.size(), 8);
    
    EXPECT_EQ(*it_4, 5);

    EXPECT_EQ(arr[0], 0);
    EXPECT_EQ(arr[1], 1);
    EXPECT_EQ(arr[2], 2);
    EXPECT_EQ(arr[3], 3);
    EXPECT_EQ(arr[4], 4);
    EXPECT_EQ(arr[5], 5);
    EXPECT_EQ(arr[6], 5);
    EXPECT_EQ(arr[7], 6);
}

TEST(DynamicArrayTest, Iterator_Erase) {
    DynamicArray<int> arr = {1, 2, 3, 4, 5};
    
    auto it = arr.erase(arr.begin() + 2);
    EXPECT_EQ(arr.size(), 4);
    EXPECT_EQ(*it, 4);
    
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 4);
    EXPECT_EQ(arr[3], 5);
}

TEST(DynamicArrayTest, Iterator_Erase_Range) {
    DynamicArray<int> arr = {1, 2, 3, 4, 5};
    
    auto it = arr.erase(arr.begin() + 2, arr.end());
    EXPECT_EQ(arr.size(), 2);
    EXPECT_EQ(it, arr.end());
    
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
}

TEST(DynamicArrayTest, Iterators) {
    DynamicArray<int> arr = {1, 2, 3, 4, 5};
    
    int sum = 0;
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 15);
    
    // Range-based for loop
    sum = 0;
    for (int val : arr) {
        sum += val;
    }
    EXPECT_EQ(sum, 15);
}

// ----- Modificacion -----
TEST(DynamicArrayTest, Clear) {
    DynamicArray<int> arr(5,42);    
    EXPECT_NE(arr.data(), nullptr);
    EXPECT_EQ(arr.size(), 5);
    EXPECT_EQ(arr.capacity(), 5);
    EXPECT_FALSE(arr.empty());

    arr.clear();
    EXPECT_NE(arr.data(), nullptr);
    EXPECT_EQ(arr.size(), 0);
    EXPECT_EQ(arr.capacity(), 5);
    EXPECT_TRUE(arr.empty());
}

TEST(DynamicArrayTest, Rerserve) {
    DynamicArray<int> arr;    
    EXPECT_EQ(arr.data(), nullptr);
    EXPECT_EQ(arr.size(), 0);
    EXPECT_EQ(arr.capacity(), 0);
    EXPECT_TRUE(arr.empty());

    arr.reserve(1234);

    EXPECT_NE(arr.data(), nullptr);
    EXPECT_EQ(arr.size(), 0);
    EXPECT_EQ(arr.capacity(), 1234);
    EXPECT_TRUE(arr.empty());
}

TEST(DynamicArrayTest, Shrink_To_Fit) {
    DynamicArray<int> arr;    
    EXPECT_EQ(arr.data(), nullptr);
    EXPECT_EQ(arr.size(), 0);
    EXPECT_EQ(arr.capacity(), 0);
    EXPECT_TRUE(arr.empty());

    arr.reserve(1234);

    EXPECT_NE(arr.data(), nullptr);
    EXPECT_EQ(arr.size(), 0);
    EXPECT_EQ(arr.capacity(), 1234);
    EXPECT_TRUE(arr.empty());
    
    arr.push_back(1);
    arr.push_back(2);
    arr.push_back(3);
    arr.push_back(4);
    arr.push_back(5);

    EXPECT_NE(arr.data(), nullptr);
    EXPECT_EQ(arr.size(), 5);
    EXPECT_EQ(arr.capacity(), 1234);
    EXPECT_FALSE(arr.empty());

    arr.shrink_to_fit();

    EXPECT_NE(arr.data(), nullptr);
    EXPECT_EQ(arr.size(), 5);
    EXPECT_EQ(arr.capacity(), 5);
    EXPECT_FALSE(arr.empty());
}

TEST(DynamicArrayTest, ReserveAndShrink) {
    DynamicArray<int> arr;
    
    arr.reserve(100);
    EXPECT_EQ(arr.capacity(), 100);
    EXPECT_EQ(arr.size(), 0);
    
    for (int i = 0; i < 50; ++i) {
        arr.push_back(i);
    }
    
    arr.shrink_to_fit();
    EXPECT_EQ(arr.capacity(), arr.size());
}

TEST(DynamicArrayTest, PushBack) {
    DynamicArray<int> arr;
    
   for (int i = 0; i < 100000; ++i) {
        arr.push_back(i);
        EXPECT_EQ(arr[i], i);
    }
}

TEST(DynamicArrayTest, EmplaceBack) {
    DynamicArray<std::string> arr;
    
    std::string& var_1 = arr.emplace_back("Hello");
    std::string& var_2 = arr.emplace_back("World");
    
    EXPECT_EQ(arr.size(), 2);
    EXPECT_EQ(arr[0], "Hello");
    EXPECT_EQ(arr[1], "World");
    EXPECT_EQ(var_1, "Hello");
    EXPECT_EQ(var_2, "World");

    arr[0] = "Hola";
    arr[1] = "Mundo";

    EXPECT_EQ(var_1, "Hola");
    EXPECT_EQ(var_2, "Mundo");
}

TEST(DynamicArrayTest, PopBack) {
    DynamicArray<int> arr = {1, 2, 3, 4, 5};
    
    arr.pop_back();
    arr.pop_back();
    arr.pop_back();
    
    EXPECT_EQ(arr.size(), 2);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
}

TEST(DynamicArrayTest, Operators_EQ_NE) {
    DynamicArray<int> arr = {1, 2, 3, 4, 5};
    DynamicArray<int> arr1 = {1, 2, 3, 4, 5};
    DynamicArray<int> arr2 = {1, 2, 10, 4, 5};

    EXPECT_TRUE(arr==arr1);
    EXPECT_FALSE(arr==arr2);
    EXPECT_FALSE(arr2==arr1);

    EXPECT_FALSE(arr!=arr1);
    EXPECT_TRUE(arr!=arr2);
    EXPECT_TRUE(arr2!=arr1);
}

TEST(DynamicArrayTest, Operators_SWAP) {
    DynamicArray<int> arr = {1, 2, 3, 4, 5};
    DynamicArray<int> arr2 = {1, 10, 5};

    EXPECT_EQ(arr.size(),5);
    EXPECT_EQ(arr2.size(),3);

    arr.swap(arr2);

    EXPECT_EQ(arr.size(),3);
    EXPECT_EQ(arr2.size(),5);

    EXPECT_EQ(arr[0],1);
    EXPECT_EQ(arr[1],10);
    EXPECT_EQ(arr[2],5);

    EXPECT_EQ(arr2[0],1);
    EXPECT_EQ(arr2[1],2);
    EXPECT_EQ(arr2[2],3);
    EXPECT_EQ(arr2[3],4);
    EXPECT_EQ(arr2[4],5);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}