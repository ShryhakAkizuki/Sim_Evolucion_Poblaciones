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

// ----- Acceso de elementos -----

TEST(NodeBaseTest, GetValue) {
    NodeBase<int> node(1000);
    EXPECT_EQ(node.Value(), 1000);
}

// ----- Modificacion -----

TEST(NodeBaseTest, SetValueConst) {
    NodeBase<int> node(1000);
    EXPECT_EQ(node.Value(), 1000);

    int x = 2000;
    node.SetValue(x);
    EXPECT_EQ(node.Value(), 2000);
}

TEST(NodeBaseTest, SetValueMove) {
    NodeBase<int> node(1000);
    EXPECT_EQ(node.Value(), 1000);

    node.SetValue(2000);
    EXPECT_EQ(node.Value(), 2000);
}

TEST(NodeBaseTest, SetValueForward) {
    NodeBase<int> node(1000);
    EXPECT_EQ(node.Value(), 1000);

    node.SetValue(int(2000));
    EXPECT_EQ(node.Value(), 2000);
}

// ----- Comparadores -----

TEST(NodeBaseTest, Operators_EQ_NE) {
    NodeBase<int> node(1000);
    NodeBase<int> node1(1000);
    NodeBase<int> node2(100);

    EXPECT_TRUE(node==node1);
    EXPECT_FALSE(node==node2);
    EXPECT_FALSE(node2==node1);

    EXPECT_FALSE(node!=node1);
    EXPECT_TRUE(node!=node2);
    EXPECT_TRUE(node2!=node1);
}

// #################### DListNode - Class ###################

TEST(DListNodeTest, DefaultConstructor) {
    DListNode<int> node;
    EXPECT_EQ(node.Value(), int());
    EXPECT_EQ(node.Next(), nullptr);
    EXPECT_EQ(node.Back(), nullptr);
}

TEST(DListNodeTest, ConstructorWithValue) {
    int xd = 100;
    DListNode<int> node(xd);
    EXPECT_EQ(node.Value(), 100);
    EXPECT_EQ(node.Next(), nullptr);
    EXPECT_EQ(node.Back(), nullptr);
}

TEST(DListNodeTest, ConstructorWithValue_Pointers_1) {
    int xd = 100;
    DListNode<int> node(xd);
    std::unique_ptr<DListNode<int>> xD = std::make_unique<
    EXPECT_EQ(node.Value(), 100);
    EXPECT_EQ(node.Next(), nullptr);
    EXPECT_EQ(node.Back(), nullptr);
}





















int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}