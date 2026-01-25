#include <gtest/gtest.h>
#include "data_structures/Node_Templates.hpp"

// #################### NodeBase - Class ###################

TEST(NodeBaseTest, DefaultConstructor) {
    NodeBase<int> node;
    EXPECT_EQ(node.Value(), int());
}

TEST(NodeBaseTest, ConstructorWithValue) {
    int a = 2;
    NodeBase<int> node(a);
    EXPECT_EQ(node.Value(), 2);
}

TEST(NodeBaseTest, ConstructorWithMoveValue) {
    int a = 2;
    NodeBase<int> node(std::move(a));
    EXPECT_EQ(node.Value(), 2);
}

TEST(NodeBaseTest, ConstructorWithForward) {
    int a = 2;
    NodeBase<int> node(std::move(a));
    EXPECT_EQ(node.Value(), 2);
}




// #################### DListNode - Class ###################



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}