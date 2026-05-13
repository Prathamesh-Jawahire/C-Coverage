#include <gtest/gtest.h>
#include "../../Server/Headers/Customer.h"

TEST(CustomerTest, SetAndGetName) {
    Customer c;
    c.setName("Harshal");

    EXPECT_EQ(c.getName(), "Harshal");
}

TEST(CustomerTest, SetAndGetAge) {
    Customer c;
    c.setAge(22);

    EXPECT_EQ(c.getAge(), 22);
}

TEST(CustomerTest, MultipleCustomersIndependence) {
    Customer c1, c2;

    c1.setName("User1");
    c2.setName("User2");

    EXPECT_NE(c1.getName(), c2.getName());
}