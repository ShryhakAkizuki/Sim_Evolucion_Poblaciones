#include <gtest/gtest.h>
#include "data_structures/Node_Templates.hpp"

// #################### NodeBase - Class ###################
// ----- Funciones especiales -----
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
// ----- Funciones especiales -----
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

TEST(DListNodeTest, ConstructorWithUniquePointer) {
    int xd = 100;
    std::unique_ptr<DListNode<int>> xD = std::make_unique<DListNode<int>>(50);

    DListNode<int> node(xd, std::move(xD));
    EXPECT_EQ(node.Value(), 100);
    EXPECT_NE(node.Next(), nullptr);
    EXPECT_EQ(node.Back(), nullptr);

    EXPECT_EQ(xD, nullptr);

    EXPECT_EQ(node.Next()->Value(), 50);
    EXPECT_EQ(node.Next()->Next(), nullptr);
    EXPECT_EQ(node.Next()->Back(), nullptr);
}

TEST(DListNodeTest, ConstructorWithrawPointer) {
    int xd = 100;
    std::unique_ptr<DListNode<int>> xD = std::make_unique<DListNode<int>>(50);

    DListNode<int> node(xd, nullptr, xD.get());
    EXPECT_EQ(node.Value(), 100);
    EXPECT_EQ(node.Next(), nullptr);
    EXPECT_EQ(node.Back(), xD.get());

    EXPECT_NE(xD, nullptr);

    EXPECT_EQ(node.Back()->Value(), 50);
    EXPECT_EQ(node.Back()->Next(), nullptr);
    EXPECT_EQ(node.Back()->Back(), nullptr);
}

TEST(DListNodeTest, ConstructorWithfullPointer) {
    int xd = 100;
    std::unique_ptr<DListNode<int>> xD = std::make_unique<DListNode<int>>(50);
    std::unique_ptr<DListNode<int>> xD_2 = std::make_unique<DListNode<int>>(200);

    DListNode<int> node(xd, std::move(xD), xD_2.get());
    EXPECT_EQ(node.Value(), 100);
    EXPECT_NE(node.Next(), nullptr);
    EXPECT_EQ(node.Back(), xD_2.get());

    EXPECT_EQ(xD, nullptr);

    EXPECT_EQ(node.Next()->Value(), 50);
    EXPECT_EQ(node.Next()->Next(), nullptr);
    EXPECT_EQ(node.Next()->Back(), nullptr);

    EXPECT_EQ(node.Back()->Value(), 200);
    EXPECT_EQ(node.Back()->Next(), nullptr);
    EXPECT_EQ(node.Back()->Back(), nullptr);
}

// ----- Modificacion -----
TEST(NodeBaseTest, SetNext) {
    int xd = 100;
    std::unique_ptr<DListNode<int>> xD = std::make_unique<DListNode<int>>(50);

    DListNode<int> node(xd);
    EXPECT_EQ(node.Value(), 100);
    EXPECT_EQ(node.Next(), nullptr);
    EXPECT_EQ(node.Back(), nullptr);
    
    node.SetNext(std::move(xD));
    EXPECT_EQ(xD, nullptr);

    EXPECT_EQ(node.Value(), 100);
    EXPECT_NE(node.Next(), nullptr);
    EXPECT_EQ(node.Back(), nullptr);

    EXPECT_EQ(node.Next()->Value(), 50);
    EXPECT_EQ(node.Next()->Next(), nullptr);
    EXPECT_EQ(node.Next()->Back(), nullptr);
}

TEST(NodeBaseTest, SetBack) {
    int xd = 100;
    std::unique_ptr<DListNode<int>> xD = std::make_unique<DListNode<int>>(50);

    DListNode<int> node(xd);
    EXPECT_EQ(node.Value(), 100);
    EXPECT_EQ(node.Next(), nullptr);
    EXPECT_EQ(node.Back(), nullptr);
    
    node.SetBack(xD.get());
    EXPECT_NE(xD, nullptr);

    EXPECT_EQ(node.Value(), 100);
    EXPECT_EQ(node.Next(), nullptr);
    EXPECT_NE(node.Back(), nullptr);

    EXPECT_EQ(node.Back()->Value(), 50);
    EXPECT_EQ(node.Back()->Next(), nullptr);
    EXPECT_EQ(node.Back()->Back(), nullptr);
}

TEST(NodeBaseTest, SetNextBack) {
    int xd = 100;
    std::unique_ptr<DListNode<int>> xD = std::make_unique<DListNode<int>>(50);
    std::unique_ptr<DListNode<int>> xD_2 = std::make_unique<DListNode<int>>(200);

    DListNode<int> node(xd);
    EXPECT_EQ(node.Value(), 100);
    EXPECT_EQ(node.Next(), nullptr);
    EXPECT_EQ(node.Back(), nullptr);
    
    node.SetNext(std::move(xD));
    EXPECT_EQ(xD, nullptr);

    node.SetBack(xD_2.get());
    EXPECT_NE(xD_2, nullptr);

    EXPECT_EQ(node.Value(), 100);
    EXPECT_NE(node.Next(), nullptr);
    EXPECT_NE(node.Back(), nullptr);

    EXPECT_EQ(node.Next()->Value(), 50);
    EXPECT_EQ(node.Next()->Next(), nullptr);
    EXPECT_EQ(node.Next()->Back(), nullptr);

    EXPECT_EQ(node.Back()->Value(), 200);
    EXPECT_EQ(node.Back()->Next(), nullptr);
    EXPECT_EQ(node.Back()->Back(), nullptr);
}

TEST(NodeBaseTest, ReleaseNext) {
    int xd = 100;
    std::unique_ptr<DListNode<int>> xD = std::make_unique<DListNode<int>>(50);

    DListNode<int> node(xd);
    EXPECT_EQ(node.Value(), 100);
    EXPECT_EQ(node.Next(), nullptr);
    EXPECT_EQ(node.Back(), nullptr);
    
    node.SetNext(std::move(xD));
    EXPECT_EQ(xD, nullptr);

    EXPECT_EQ(node.Value(), 100);
    EXPECT_NE(node.Next(), nullptr);
    EXPECT_EQ(node.Back(), nullptr);

    EXPECT_EQ(node.Next()->Value(), 50);
    EXPECT_EQ(node.Next()->Next(), nullptr);
    EXPECT_EQ(node.Next()->Back(), nullptr);

    xD = node.release_next();
    EXPECT_NE(xD, nullptr);
    EXPECT_EQ(xD->Value(), 50);
    EXPECT_EQ(xD->Next(), nullptr);
    EXPECT_EQ(xD->Back(), nullptr);

    EXPECT_EQ(node.Value(), 100);
    EXPECT_EQ(node.Next(), nullptr);
    EXPECT_EQ(node.Back(), nullptr);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}