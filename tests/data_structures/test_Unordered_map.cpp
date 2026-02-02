#include <gtest/gtest.h>
#include "data_structures/Unordered_map.hpp"

// ----- Funciones especiales -----
// TEST(UnorderedMapTest, DefaultConstructor) {
//     Unordered_map<Pair<int,int>,int> unordered;
//     EXPECT_EQ(unordered.size_, 0);
//     EXPECT_EQ(unordered.buckets_.size(), 0);
// }

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}