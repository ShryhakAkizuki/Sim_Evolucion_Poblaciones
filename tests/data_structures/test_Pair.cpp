#include <gtest/gtest.h>
#include "data_structures/Pair.hpp"

// ----- Funciones especiales -----
TEST(PairTest, DefaultConstructor) {
    Pair<int, int> par;
    EXPECT_EQ(par.First(), int());
    EXPECT_EQ(par.Second(), int());
}

TEST(PairTest, ConstructorWithValue) {
    int a = 2;
    int b = 20;
    Pair<int, int> par(a, b);
    EXPECT_EQ(par.First(), 2);
    EXPECT_EQ(par.Second(), 20);
}

TEST(PairTest, ConstructorWithMoveValue) {
    int a = 2;
    int b = 20;    
    Pair<int, int> par(std::move(a), std::move(b));
    EXPECT_EQ(par.First(), 2);
    EXPECT_EQ(par.Second(), 20);
}

// ----- Acceso de elementos -----
TEST(PairTest, GetFirst_Second) {
    Pair<int, int> par(2, 20);
    EXPECT_EQ(par.First(), 2);
    EXPECT_EQ(par.Second(), 20);
}

// ----- Modificacion -----

TEST(PairTest, SetFirstConst) {
    Pair<int, int> par(2, 20);
    EXPECT_EQ(par.First(), 2);
    EXPECT_EQ(par.Second(), 20);

    int x = 2000;
    par.Set_First(x);
    EXPECT_EQ(par.First(), 2000);
}

TEST(PairTest, SetFirstMove) {
    Pair<int, int> par(2, 20);
    EXPECT_EQ(par.First(), 2);
    EXPECT_EQ(par.Second(), 20);

    par.Set_First(2000);
    EXPECT_EQ(par.First(), 2000);
}

TEST(PairTest, SetFirstForward) {
    Pair<int, int> par(2, 20);
    EXPECT_EQ(par.First(), 2);
    EXPECT_EQ(par.Second(), 20);

    par.Set_First(int(2000));
    EXPECT_EQ(par.First(), 2000);
}

TEST(PairTest, SetSecondConst) {
    Pair<int, int> par(2, 20);
    EXPECT_EQ(par.First(), 2);
    EXPECT_EQ(par.Second(), 20);

    int x = 2000;
    par.Set_Second(x);
    EXPECT_EQ(par.Second(), 2000);
}

TEST(PairTest, SetSecondMove) {
    Pair<int, int> par(2, 20);
    EXPECT_EQ(par.First(), 2);
    EXPECT_EQ(par.Second(), 20);

    par.Set_Second(2000);
    EXPECT_EQ(par.Second(), 2000);
}

TEST(PairTest, SetSecondForward) {
    Pair<int, int> par(2, 20);
    EXPECT_EQ(par.First(), 2);
    EXPECT_EQ(par.Second(), 20);

    par.Set_Second(int(2000));
    EXPECT_EQ(par.Second(), 2000);
}

// ----- Comparadores -----
TEST(PairTest, Operators_EQ_NE) {
    Pair<int, int> par_1(2, 20);
    Pair<int, int> par_2(2, 20);
    Pair<int, int> par_3(1, 10);

    EXPECT_TRUE(par_1==par_2);
    EXPECT_FALSE(par_1==par_3);
    EXPECT_FALSE(par_3==par_2);

    EXPECT_FALSE(par_1!=par_2);
    EXPECT_TRUE(par_1!=par_3);
    EXPECT_TRUE(par_3!=par_2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}