#include <gtest/gtest.h>
#include "data_structures/Double_Linked_List.hpp"

// ----- Funciones especiales -----
TEST(DoubleLinkedListTest, DefaultConstructor) {
    Double_Linked_List<int> list;
    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.empty());
}

TEST(DoubleLinkedListTest, ConstructorWithValue) {
    Double_Linked_List<int> list(5,42);
    EXPECT_EQ(list.size(), 5);
    EXPECT_FALSE(list.empty());

    for(Double_Linked_List<int>::const_iterator it = list.cbegin(); it != list.cend(); ++it) EXPECT_EQ(*it, 42);
}

TEST(DoubleLinkedListTest, ConstructorWithList) {
    Double_Linked_List<int> list = {1, 2, 3, 4, 5};
    EXPECT_EQ(list.size(), 5);
    EXPECT_FALSE(list.empty());
    
    int i = 1;
    for(Double_Linked_List<int>::const_iterator it = list.cbegin(); it != list.cend(); ++it, ++i) EXPECT_EQ(*it, i);
}

TEST(DoubleLinkedListTest, ConstructorWithSpan) {
    int tmp[] = {1,2,3,4,5};
    auto s = std::span<int>(tmp);
    Double_Linked_List<int> list(s);    
    EXPECT_EQ(list.size(), 5);
    EXPECT_FALSE(list.empty());
    
    int i = 1;
    for(Double_Linked_List<int>::const_iterator it = list.cbegin(); it != list.cend(); ++it, ++i) EXPECT_EQ(*it, i);
}

TEST(DoubleLinkedListTest, ConstructorWithIteratos) {
    Double_Linked_List<int> list_1(5, 42);
    EXPECT_EQ(list_1.size(), 5);
    EXPECT_FALSE(list_1.empty());

    Double_Linked_List<int> list_2(list_1.begin(), list_1.end());
    EXPECT_EQ(list_2.size(), 5);
    EXPECT_FALSE(list_2.empty());

    Double_Linked_List<int>::const_iterator it_2 = list_2.cbegin();
    for(Double_Linked_List<int>::const_iterator it_1 = list_1.cbegin(); it_1 != list_1.cend(); ++it_1, ++it_2) { 
        EXPECT_EQ(*it_1, 42);
        EXPECT_EQ(*it_2, 42);
    }
}

// ----- Acceso de elementos -----

TEST(DoubleLinkedListTest, AtMethod) {
    Double_Linked_List<int> list = {1, 2, 3, 4, 5};
    
    EXPECT_EQ(list.at(0), 1);
    EXPECT_EQ(list.at(2), 3);
    EXPECT_EQ(list.at(4), 5);
    
    EXPECT_THROW(list.at(10), std::out_of_range);
    EXPECT_THROW(list.at(-2), std::out_of_range);
}

TEST(DoubleLinkedListTest, Front_Back) {
    Double_Linked_List<int> list;    
    EXPECT_THROW(list.front(), std::out_of_range);
    EXPECT_THROW(list.back(), std::out_of_range);

    list = Double_Linked_List<int>({1, 2, 3, 4, 5});   
    
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.back(), 5);
}

// ----- Iteradores -----

TEST(DoubleLinkedListTest, Iterator_Begin) {
    Double_Linked_List<int> list;    
    EXPECT_EQ(list.begin(), Double_Linked_List<int>::iterator(nullptr, &list));
    
    list.push_back(1);    
    EXPECT_NE(list.begin(), Double_Linked_List<int>::iterator(nullptr, &list));
    EXPECT_EQ(*list.begin(), 1);
}

TEST(DoubleLinkedListTest, Iterator_End) {
    Double_Linked_List<int> list;    
    EXPECT_EQ(list.end(), Double_Linked_List<int>::iterator(nullptr, &list));
    
    list.push_back(1);    
    EXPECT_EQ(list.end(), Double_Linked_List<int>::iterator(nullptr, &list));
    EXPECT_EQ(*(--list.end()), 1);
}

TEST(DoubleLinkedListTest, Iterator_Insert_1) {
    Double_Linked_List<int> list;  
    EXPECT_NO_THROW(list.insert(list.cbegin(),100));
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.at(0), 100);

    list.push_back(1);
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.at(1), 1);

    list.insert(list.cbegin(),5);
    EXPECT_EQ(list.front(),5);
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list.at(0), 5);

    list.insert(list.cend(),10);
    EXPECT_EQ(list.back(),10);
    EXPECT_EQ(list.size(), 4);
    EXPECT_EQ(list.at(3), 10);

    EXPECT_EQ(list.at(0),5);
    EXPECT_EQ(list.at(1),100);
    EXPECT_EQ(list.at(2),1);
    EXPECT_EQ(list.at(3),10);
}

TEST(DoubleLinkedListTest, Iterator_Insert_2) {
    Double_Linked_List<int> list = {1, 2, 4, 5};
    
    Double_Linked_List<int>::const_iterator pos = list.cbegin();
    for(int i = 0; i < 2; ++i) ++pos;

    Double_Linked_List<int>::iterator it = list.insert(pos, 3);
    EXPECT_EQ(list.size(), 5);
    
    EXPECT_EQ(*it, 3);
    
    EXPECT_EQ(list.at(0), 1);
    EXPECT_EQ(list.at(1), 2);
    EXPECT_EQ(list.at(2), 3);
    EXPECT_EQ(list.at(3), 4);
    EXPECT_EQ(list.at(4), 5);
}

TEST(DoubleLinkedListTest, Iterator_Insert_3) {
    Double_Linked_List<int> list = {1, 2, 4, 5};
    
    Double_Linked_List<int>::const_iterator pos = list.cbegin();
    for(int i = 0; i < 2; ++i) ++pos;


    Double_Linked_List<int>::iterator it_1 = list.insert(pos, 3);
    EXPECT_EQ(list.size(), 5);
    
    EXPECT_EQ(*it_1, 3);

    Double_Linked_List<int>::iterator it_2 = list.insert(list.cbegin(), 0);
    EXPECT_EQ(list.size(), 6);
    
    EXPECT_EQ(*it_2, 0);

    Double_Linked_List<int>::iterator it_3 = list.insert(list.cend(), 6);
    EXPECT_EQ(list.size(), 7);
    
    EXPECT_EQ(*it_3, 6);

    pos = list.cend();
    for(int i = 0; i < 2; ++i) --pos;

    Double_Linked_List<int>::iterator it_4 = list.insert(pos, 5);
    EXPECT_EQ(list.size(), 8);
    
    EXPECT_EQ(*it_4, 5);

    EXPECT_EQ(list.at(0), 0);
    EXPECT_EQ(list.at(1), 1);
    EXPECT_EQ(list.at(2), 2);
    EXPECT_EQ(list.at(3), 3);
    EXPECT_EQ(list.at(4), 4);
    EXPECT_EQ(list.at(5), 5);
    EXPECT_EQ(list.at(6), 5);
    EXPECT_EQ(list.at(7), 6);
}

TEST(DoubleLinkedListTest, Iterator_Insert_Range) {
    Double_Linked_List<int> list_1 = {1, 2, 4, 5};
    Double_Linked_List<int> list_2;

    Double_Linked_List<int>::const_iterator pos = list_1.cbegin();
    ++pos;

    Double_Linked_List<int>::iterator it_pointer_1 = list_2.insert(list_2.cbegin(), pos, list_1.cend());

    EXPECT_EQ(*it_pointer_1, 2);
    EXPECT_EQ(list_2.size(), 3);
    EXPECT_EQ(list_2.at(0), 2);
    EXPECT_EQ(list_2.at(1), 4);
    EXPECT_EQ(list_2.at(2), 5);

    Double_Linked_List<int>::iterator it_pointer_2 = list_2.insert(list_2.cend(), list_1.cbegin(), list_1.cend());

    EXPECT_EQ(*it_pointer_2, 1);
    EXPECT_EQ(list_2.size(), 7);
    EXPECT_EQ(list_2.at(3), 1);
    EXPECT_EQ(list_2.at(4), 2);
    EXPECT_EQ(list_2.at(5), 4);
    EXPECT_EQ(list_2.at(6), 5);

    Double_Linked_List<int>::const_iterator pos_1 = list_2.cbegin();
    Double_Linked_List<int>::const_iterator pos_2 = list_1.cbegin();
    Double_Linked_List<int>::const_iterator pos_3 = list_1.cbegin();

    for(int i = 0; i < 4; ++i) ++pos_1;
    for(int i = 0; i < 2; ++i) ++pos_2;
    for(int i = 0; i < 3; ++i) ++pos_3;


    Double_Linked_List<int>::iterator it_pointer_3 = list_2.insert(pos_1, pos_2, pos_3);

    EXPECT_EQ(*it_pointer_3, 4);
    EXPECT_EQ(list_2.size(), 8);
    EXPECT_EQ(list_2.at(4), 4);
}

TEST(DoubleLinkedListTest, Iterator_Emplace_1) {
    Double_Linked_List<int> list;  
    
    EXPECT_NO_THROW(list.emplace(list.cbegin(),int(100)));
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.at(0), 100);
    
    list.push_back(1);  
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.at(1), 1);

    list.emplace(list.cbegin(),int(5));
    EXPECT_EQ(list.front(),5);
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list.at(0), 5);
    
    list.emplace(list.cend(),int(10));
    EXPECT_EQ(list.back(),10);
    EXPECT_EQ(list.size(), 4);
    EXPECT_EQ(list.at(3), 10);

    EXPECT_EQ(list.at(0),5);
    EXPECT_EQ(list.at(1),100);
    EXPECT_EQ(list.at(2),1);
    EXPECT_EQ(list.at(3),10);
}

TEST(DoubleLinkedListTest, Iterator_Emplace_2) {
    Double_Linked_List<int> list = {1, 2, 4, 5};
    
    Double_Linked_List<int>::const_iterator pos = list.cbegin();
    for(int i = 0; i < 2; ++i) ++pos;


    Double_Linked_List<int>::iterator it = list.emplace(pos, int(3));
    EXPECT_EQ(list.size(), 5);
    
    EXPECT_EQ(*it, 3);
    
    EXPECT_EQ(list.at(0), 1);
    EXPECT_EQ(list.at(1), 2);
    EXPECT_EQ(list.at(2), 3);
    EXPECT_EQ(list.at(3), 4);
    EXPECT_EQ(list.at(4), 5);
}

TEST(DoubleLinkedListTest, Iterator_Emplace_3) {
    Double_Linked_List<int> list = {1, 2, 4, 5};
    
    Double_Linked_List<int>::const_iterator pos = list.cbegin();
    for(int i = 0; i < 2; ++i) ++pos;


    Double_Linked_List<int>::iterator it_1 = list.emplace(pos, int(3));
    EXPECT_EQ(list.size(), 5);
    
    EXPECT_EQ(*it_1, 3);

    Double_Linked_List<int>::iterator it_2 = list.emplace(list.cbegin(), int(0));
    EXPECT_EQ(list.size(), 6);
    
    EXPECT_EQ(*it_2, 0);

    Double_Linked_List<int>::iterator it_3 = list.emplace(list.cend(), int(6));
    EXPECT_EQ(list.size(), 7);
    
    EXPECT_EQ(*it_3, 6);

    pos = list.cend();
    for(int i = 0; i < 2; ++i) --pos;

    Double_Linked_List<int>::iterator it_4 = list.emplace(pos, int(5));
    EXPECT_EQ(list.size(), 8);
    
    EXPECT_EQ(*it_4, 5);

    EXPECT_EQ(list.at(0), 0);
    EXPECT_EQ(list.at(1), 1);
    EXPECT_EQ(list.at(2), 2);
    EXPECT_EQ(list.at(3), 3);
    EXPECT_EQ(list.at(4), 4);
    EXPECT_EQ(list.at(5), 5);
    EXPECT_EQ(list.at(6), 5);
    EXPECT_EQ(list.at(7), 6);
}

TEST(DoubleLinkedListTest, Iterator_Erase) {
    Double_Linked_List<int> list = {1, 2, 3, 4, 5};
    
    Double_Linked_List<int>::const_iterator pos = list.cbegin();
    for(int i = 0; i < 2; ++i) ++pos;

    Double_Linked_List<int>::iterator it = list.erase(pos);
    EXPECT_EQ(list.size(), 4);
    EXPECT_EQ(*it, 4);
    
    EXPECT_EQ(list.at(0), 1);
    EXPECT_EQ(list.at(1), 2);
    EXPECT_EQ(list.at(2), 4);
    EXPECT_EQ(list.at(3), 5);
}

TEST(DoubleLinkedListTest, Iterator_Erase_Range) {
    Double_Linked_List<int> list = {1, 2, 3, 4, 5};
    
    Double_Linked_List<int>::const_iterator pos = list.cbegin();
    for(int i = 0; i < 2; ++i) ++pos;

    Double_Linked_List<int>::iterator it = list.erase(pos, list.cend());
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(it, list.end());
    
    EXPECT_EQ(list.at(0), 1);
    EXPECT_EQ(list.at(1), 2);
}

TEST(DoubleLinkedListTest, Iterators) {
    Double_Linked_List<int> list = {1, 2, 3, 4, 5};
    
    int sum = 0;
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 15);
    
    // Range-based for loop
    sum = 0;
    for (int val : list) {
        sum += val;
    }
    EXPECT_EQ(sum, 15);
}

TEST(DoubleLinkedListTest, Iterator_Find) {
    Double_Linked_List<int> list = {1, 3, 5, 7, 9};
    
    auto find = list.find(1);
    EXPECT_EQ(*find, 1);

    find = list.find(2);
    EXPECT_EQ(find, list.end());

    find = list.find(3);
    EXPECT_EQ(*find, 3);

    find = list.find(4);
    EXPECT_EQ(find, list.end());

    find = list.find(5);
    EXPECT_EQ(*find, 5);

    find = list.find(6);
    EXPECT_EQ(find, list.end());

    find = list.find(7);
    EXPECT_EQ(*find, 7);

    find = list.find(8);
    EXPECT_EQ(find, list.end());

    find = list.find(9);
    EXPECT_EQ(*find, 9);
}

// ----- Modificacion -----
TEST(DoubleLinkedListTest, Clear) {
    Double_Linked_List<int> list(5,42);    
    EXPECT_EQ(list.size(), 5);
    EXPECT_FALSE(list.empty());

    list.clear();
    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.empty());
}

TEST(DoubleLinkedListTest, PushBack) {
    Double_Linked_List<int> list;
    
   for (int i = 0; i < 100000; ++i) {
        list.push_back(i);
        EXPECT_EQ(list.at(i), i);
    }
}

TEST(DoubleLinkedListTest, EmplaceBack) {
    Double_Linked_List<std::string> list;
    
    std::string& var_1 = list.emplace_back("Hello");
    std::string& var_2 = list.emplace_back("World");
    
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.at(0), "Hello");
    EXPECT_EQ(list.at(1), "World");
    EXPECT_EQ(var_1, "Hello");
    EXPECT_EQ(var_2, "World");

    list.at(0) = "Hola";
    list.at(1) = "Mundo";

    EXPECT_EQ(var_1, "Hola");
    EXPECT_EQ(var_2, "Mundo");
}

TEST(DoubleLinkedListTest, PushFront) {
    Double_Linked_List<int> list;
    
    for (int i = 0; i < 100000; ++i) list.push_front(i);   
    for (int i = 0; i < 100000; ++i) EXPECT_EQ(list.at(i), 100000-1-i);
}   

TEST(DoubleLinkedListTest, EmplaceFront) {
    Double_Linked_List<std::string> list;
    
    std::string& var_1 = list.emplace_front("Hello");
    std::string& var_2 = list.emplace_front("World");
    
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.at(1), "Hello");
    EXPECT_EQ(list.at(0), "World");
    EXPECT_EQ(var_1, "Hello");
    EXPECT_EQ(var_2, "World");

    list.at(1) = "Hola";
    list.at(0) = "Mundo";

    EXPECT_EQ(var_1, "Hola");
    EXPECT_EQ(var_2, "Mundo");
}

TEST(DoubleLinkedListTest, PopBack) {
    Double_Linked_List<int> list = {1, 2, 3, 4, 5};
    
    list.pop_back();
    list.pop_back();
    list.pop_back();
    
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.at(0), 1);
    EXPECT_EQ(list.at(1), 2);
}

TEST(DoubleLinkedListTest, PopFront) {
    Double_Linked_List<int> list = {1, 2, 3, 4, 5};
    
    list.pop_front();
    list.pop_front();
    list.pop_front();
    
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.at(0), 4);
    EXPECT_EQ(list.at(1), 5);
}

// ----- Comparadores -----
TEST(DynamicArrayTest, Operators_EQ_NE) {
    Double_Linked_List<int> list_1 = {1, 2, 3, 4, 5};
    Double_Linked_List<int> list_2 = {1, 2, 3, 4, 5};
    Double_Linked_List<int> list_3 = {1, 2, 10, 4, 5};

    EXPECT_TRUE(list_1==list_2);
    EXPECT_FALSE(list_1==list_3);
    EXPECT_FALSE(list_2==list_3);

    EXPECT_FALSE(list_1!=list_2);
    EXPECT_TRUE(list_1!=list_3);
    EXPECT_TRUE(list_3!=list_2);
}

// ----- Helpers -----
TEST(DynamicArrayTest, Operators_SWAP) {
    Double_Linked_List<int> list_1 = {1, 2, 3, 4, 5};
    Double_Linked_List<int> list_2 = {1, 10, 5};

    EXPECT_EQ(list_1.size(),5);
    EXPECT_EQ(list_2.size(),3);

    list_1.swap(list_2);

    EXPECT_EQ(list_1.size(),3);
    EXPECT_EQ(list_2.size(),5);

    EXPECT_EQ(list_1.at(0),1);
    EXPECT_EQ(list_1.at(1),10);
    EXPECT_EQ(list_1.at(2),5);

    EXPECT_EQ(list_2.at(0),1);
    EXPECT_EQ(list_2.at(1),2);
    EXPECT_EQ(list_2.at(2),3);
    EXPECT_EQ(list_2.at(3),4);
    EXPECT_EQ(list_2.at(4),5);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}