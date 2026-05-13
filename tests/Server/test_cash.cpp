#include <gtest/gtest.h>
#include "../../Server/Headers/Cash.h"

TEST(CashTest, StartsAtZero) {
    Cash c;
    EXPECT_DOUBLE_EQ(c.get_final_cash(), 0.0);
}

TEST(CashTest, CashInIncreasesFinalCash) {
    Cash c;
    c + 10.5;
    EXPECT_DOUBLE_EQ(c.get_final_cash(), 10.5);
}

TEST(CashTest, CashOutDecreasesFinalCash) {
    Cash c;
    c + 20.0;
    c - 7.5;
    EXPECT_DOUBLE_EQ(c.get_final_cash(), 12.5);
}

