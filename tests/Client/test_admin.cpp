#include <gtest/gtest.h>
#include "../../Server/Headers/Admin.h"

TEST(AdminTest, SetName) {
    Admin a;
    a.setName("AdminUser");

    EXPECT_EQ(a.getName(), "AdminUser");
}

TEST(AdminTest, SetAge) {
    Admin a;
    a.setAge(40);

    EXPECT_EQ(a.getAge(), 40);
}

TEST(AdminTest, ObjectIndependence) {
    Admin a1, a2;

    a1.setName("A1");
    a2.setName("A2");

    EXPECT_NE(a1.getName(), a2.getName());
}