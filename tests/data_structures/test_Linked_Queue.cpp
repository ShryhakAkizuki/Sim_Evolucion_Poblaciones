#include <gtest/gtest.h>
#include "data_structures/Linked_Queue.hpp"
#include "data_structures/Double_Linked_List.hpp"

// ----- Funciones especiales -----
TEST(LinkedQueueTest, DefaultConstructor) {
    Linked_Queue<int> queue;
    EXPECT_EQ(queue.size(), 0);
    EXPECT_TRUE(queue.empty());
}

TEST(LinkedQueueTest, ConstructorWithValue) {
    Linked_Queue<int> queue(5,42);
    EXPECT_EQ(queue.size(), 5);
    EXPECT_FALSE(queue.empty());

    for(size_t i = 0; i < 5; ++i) EXPECT_EQ(queue.extract(), 42);
}

TEST(LinkedQueueTest, ConstructorWithList) {
    Linked_Queue<int> queue = {1, 2, 3, 4, 5};
    EXPECT_EQ(queue.size(), 5);
    EXPECT_FALSE(queue.empty());
    
    for(size_t i = 1; i <= 5; ++i) EXPECT_EQ(queue.extract(), i);
}

TEST(LinkedQueueTest, ConstructorWithSpan) {
    int tmp[] = {1,2,3,4,5};
    auto s = std::span<int>(tmp);
    Linked_Queue<int> queue(s);    
    EXPECT_EQ(queue.size(), 5);
    EXPECT_FALSE(queue.empty());
    
    for(size_t i = 1; i <= 5; ++i) EXPECT_EQ(queue.extract(), i);
}

TEST(LinkedQueueTest, ConstructorWithIteratos) {
    Double_Linked_List<int> list_1(5, 42);
    EXPECT_EQ(list_1.size(), 5);
    EXPECT_FALSE(list_1.empty());

    Linked_Queue<int> queue(list_1.begin(), list_1.end());
    EXPECT_EQ(queue.size(), 5);
    EXPECT_FALSE(queue.empty());

    for(size_t i = 0; i < 5; ++i) EXPECT_EQ(queue.extract(), 42);
}

// ----- Acceso de elementos -----

TEST(LinkedQueueTest, Front_Back) {
    Linked_Queue<int> queue;    
    EXPECT_THROW(queue.front(), std::out_of_range);
    EXPECT_THROW(queue.back(), std::out_of_range);

    queue = Linked_Queue<int>({1, 2, 3, 4, 5});   
    
    EXPECT_EQ(queue.front(), 1);
    EXPECT_EQ(queue.back(), 5);
}

// ----- Modificacion -----
TEST(LinkedQueueTest, Clear) {
    Linked_Queue<int> queue(5,42);    
    EXPECT_EQ(queue.size(), 5);
    EXPECT_FALSE(queue.empty());

    queue.clear();
    EXPECT_EQ(queue.size(), 0);
    EXPECT_TRUE(queue.empty());
}

TEST(LinkedQueueTest, Enqueue) {
    Linked_Queue<int> queue;
    
   for (int i = 0; i < 100000; ++i) queue.enqueue(i);
   for (int i = 0; i < 100000; ++i) EXPECT_EQ(queue.extract(), i);
}

TEST(LinkedQueueTest, Emplace_Enqueue) {
    Linked_Queue<std::string> queue;
    
    std::string& var_1 = queue.emplace_enqueue("Hello");
    std::string& var_2 = queue.emplace_enqueue("World");
    
    EXPECT_EQ(queue.size(), 2);
    EXPECT_EQ(var_1, "Hello");
    EXPECT_EQ(var_2, "World");

    var_1 = "Hola";
    var_2 = "Mundo";

    EXPECT_EQ(queue.extract(), "Hola");
    EXPECT_EQ(queue.extract(), "Mundo");
}

TEST(LinkedQueueTest, Dequeue) {
    Linked_Queue<int> queue = {1, 2, 3, 4, 5};
    
    queue.dequeue();
    queue.dequeue();
    queue.dequeue();
    
    EXPECT_EQ(queue.size(), 2);
    EXPECT_EQ(queue.extract(), 4);
    EXPECT_EQ(queue.extract(), 5);
}

// ----- Comparadores -----
TEST(LinkedQueueTest, Operators_EQ_NE) {
    Linked_Queue<int> queue_1 = {1, 2, 3, 4, 5};
    Linked_Queue<int> queue_2 = {1, 2, 3, 4, 5};
    Linked_Queue<int> queue_3 = {1, 2, 10, 4, 5};

    EXPECT_TRUE(queue_1==queue_2);
    EXPECT_FALSE(queue_1==queue_3);
    EXPECT_FALSE(queue_2==queue_3);

    EXPECT_FALSE(queue_1!=queue_2);
    EXPECT_TRUE(queue_1!=queue_3);
    EXPECT_TRUE(queue_3!=queue_2);
}

// ----- Helpers -----
TEST(LinkedQueueTest, Operators_SWAP) {
    Linked_Queue<int> queue_1 = {1, 2, 3, 4, 5};
    Linked_Queue<int> queue_2 = {1, 10, 5};

    EXPECT_EQ(queue_1.size(),5);
    EXPECT_EQ(queue_2.size(),3);

    queue_1.swap(queue_2);

    EXPECT_EQ(queue_1.size(),3);
    EXPECT_EQ(queue_2.size(),5);

    EXPECT_EQ(queue_1.extract(),1);
    EXPECT_EQ(queue_1.extract(),10);
    EXPECT_EQ(queue_1.extract(),5);

    EXPECT_EQ(queue_2.extract(),1);
    EXPECT_EQ(queue_2.extract(),2);
    EXPECT_EQ(queue_2.extract(),3);
    EXPECT_EQ(queue_2.extract(),4);
    EXPECT_EQ(queue_2.extract(),5);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}