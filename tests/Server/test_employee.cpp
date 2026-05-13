#include <gtest/gtest.h>
#include "../../Server/Headers/Employee.h"

TEST(EmployeeTest, BasicSetGet) {
    Employee e;
    e.setName("Bob");
    e.setAge(35);

    EXPECT_EQ(e.getName(), "Bob");
    EXPECT_EQ(e.getAge(), 35);
}

TEST(EmployeeTest, EdgeCaseZeroAge) {
    Employee e;
    e.setAge(0);

    EXPECT_EQ(e.getAge(), 0);
}

TEST(EmployeeTest, MultipleEmployees) {
    Employee e1, e2;

    e1.setName("Emp1");
    e2.setName("Emp2");

    EXPECT_NE(e1.getName(), e2.getName());
}