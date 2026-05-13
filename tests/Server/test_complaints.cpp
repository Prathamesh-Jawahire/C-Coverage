#include <gtest/gtest.h>

#include <cstdio>
#include <sstream>

#include "../../Server/Headers/Complaint_C.h"
#include "../../Server/Headers/Complaint_E.h"

TEST(ComplaintTest, CustomerWriteIsUniqueAndEmployeeCanAnswer) {
    const std::string file = "complaint_test.dat";
    std::remove(file.c_str());

    Complaint_C c1("alice", file);
    char text1[500] = "late delivery";
    ASSERT_TRUE(c1.write(text1));

    // second complaint by same client should be rejected (unique constraint)
    char text2[500] = "changed mind";
    EXPECT_FALSE(c1.write(text2));

    Complaint_E e(file);
    char ans[500] = "resolved";
    EXPECT_TRUE(e.answer(ans, "alice"));

    std::ostringstream ss;
    e.view(true, ss);
    const std::string out = ss.str();
    EXPECT_NE(out.find("alice"), std::string::npos);
    EXPECT_NE(out.find("late delivery"), std::string::npos);
    EXPECT_NE(out.find("resolved"), std::string::npos);

    std::remove(file.c_str());
}

TEST(ComplaintTest, CustomerUpdateAllowedBeforeAnsweredButNotAfter) {
    const std::string file = "complaint_update_test.dat";
    std::remove(file.c_str());

    Complaint_C c("bob", file);
    char text[500] = "damaged item";
    ASSERT_TRUE(c.write(text));

    bool answeredAlready = false;
    char newText[500] = "damaged item - need refund";
    EXPECT_TRUE(c.update(newText, answeredAlready));
    EXPECT_FALSE(answeredAlready);

    Complaint_E e(file);
    char ans[500] = "refund issued";
    ASSERT_TRUE(e.answer(ans, "bob"));

    bool status = false;
    char newer[500] = "try update after answer";
    EXPECT_FALSE(c.update(newer, status));
    EXPECT_TRUE(status); // indicates answered, so update refused

    std::remove(file.c_str());
}

TEST(ComplaintTest, EmployeeListAndClear) {
    const std::string file = "complaint_clear_test.dat";
    std::remove(file.c_str());

    Complaint_C c1("u1", file);
    char t1[500] = "c1";
    ASSERT_TRUE(c1.write(t1));

    Complaint_C c2("u2", file);
    char t2[500] = "c2";
    ASSERT_TRUE(c2.write(t2));

    Complaint_E e(file);
    std::ostringstream list;
    e.view(false, list);
    const std::string before = list.str();
    EXPECT_NE(before.find("u1"), std::string::npos);
    EXPECT_NE(before.find("u2"), std::string::npos);

    e.clear();
    std::ostringstream after;
    e.view(false, after);
    EXPECT_TRUE(after.str().empty());

    std::remove(file.c_str());
}

