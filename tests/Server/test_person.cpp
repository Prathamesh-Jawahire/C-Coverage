#include <gtest/gtest.h>
#include "../../Server/Headers/Person.h"

TEST(PersonTest, DefaultConstructor) {
    Person p;

    EXPECT_EQ(p.getName(), "");
    EXPECT_EQ(p.getAge(), 0);
}

TEST(PersonTest, ParameterizedConstructor) {
    Person p("John", 25);

    EXPECT_EQ(p.getName(), "John");
    EXPECT_EQ(p.getAge(), 25);
}

TEST(PersonTest, SettersAndGetters) {
    Person p;
    p.setName("Alice");
    p.setAge(30);

    EXPECT_EQ(p.getName(), "Alice");
    EXPECT_EQ(p.getAge(), 30);
}

TEST(PersonTest, EdgeCaseEmptyName) {
    Person p("", 20);

    EXPECT_EQ(p.getName(), "");
}