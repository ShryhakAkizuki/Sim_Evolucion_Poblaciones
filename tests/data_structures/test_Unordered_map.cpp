#include <gtest/gtest.h>
#include "data_structures/Unordered_map.hpp"

// ----- Funciones especiales -----
TEST(UnorderedMapTest, DefaultConstructor) {
    Unordered_map<Pair<int,int>,int> unordered;
    EXPECT_EQ(unordered.size(), 0);
    EXPECT_EQ(unordered.bucket_count(), 0);
}

TEST(UnorderedMapTest, ReserveConstructor) {
    Unordered_map<Pair<int,int>,int> unordered(4);
    EXPECT_EQ(unordered.size(), 0);
    EXPECT_EQ(unordered.bucket_count(), 4);

    int i = 0;
    for(auto it = unordered.cbegin(); it != unordered.cend(); ++it){
        ++i;
    }

    EXPECT_EQ(i, 0);
}

TEST(UnorderedMapTest, ConstructorWithList) {
    Unordered_map<Pair<int,int>,int> unordered = {
        Pair<Pair<int,int>, int>(Pair<int,int>(1,1), 1),    
        Pair<Pair<int,int>, int>(Pair<int,int>(1,2), 2),    
        Pair<Pair<int,int>, int>(Pair<int,int>(2,1), 3),
        Pair<Pair<int,int>, int>(Pair<int,int>(2,2), 4),
        Pair<Pair<int,int>, int>(Pair<int,int>(2,3), 5)      
    };

    EXPECT_EQ(unordered.size(), 5);
    EXPECT_EQ(unordered.bucket_count(), 5);

    int i = 1;
    for(auto it = unordered.cbegin(); it != unordered.cend(); ++it){
        EXPECT_EQ(it->Second(), i);
        ++i;
    }
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}