#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>

#include "../../Server/Headers/Customer.h"
#include "../../Server/Headers/Employee.h"
#include "../../Server/Headers/Thread.h"

namespace {

class TestPersonLogin final : public Person {
public:
    explicit TestPersonLogin(Thread& t) : Person(t) {}
    void home() override {} // don't enter menus during login tests
};

class TestCustomerRunner final : public Customer {
public:
    explicit TestCustomerRunner(Thread& t) : Customer(t) {}
};

class TestEmployeeRunner final : public Employee {
public:
    explicit TestEmployeeRunner(Thread& t) : Employee(t) {}
    void runHomeForTest() { home(); }
};

static void writeLoginFile(const std::string& fileName)
{
    std::ofstream f(fileName, std::ios::trunc);
    ASSERT_TRUE(f.is_open());
    f << "Username: u1\n"
      << "Password: p1\n\n";
}

} // namespace

TEST(LoginFlowTest, LoginCoversIncorrectUserPassAndCorrect) {
    writeLoginFile("customer.txt");

    Thread t;
    t.enableTestMode(true);
    TestPersonLogin p(t);

    // attempt 1: wrong user
    t.testPushRecv("wrong");
    t.testPushRecv("x");
    // attempt 2: correct user, wrong pass
    t.testPushRecv("u1");
    t.testPushRecv("wrong");
    // attempt 3: correct
    t.testPushRecv("u1");
    t.testPushRecv("p1");

    p.login("customer.txt");

    bool sawUser = false, sawPass = false, sawCorrect = false;
    while (t.testSentCount() > 0)
    {
        const std::string msg = t.testPopSent();
        if (msg == "Incorrect-User") sawUser = true;
        if (msg == "Incorrect-Pass") sawPass = true;
        if (msg == "Correct") sawCorrect = true;
    }

    EXPECT_TRUE(sawUser);
    EXPECT_TRUE(sawPass);
    EXPECT_TRUE(sawCorrect);

    std::remove("customer.txt");
}

TEST(ComplainFlowTest, CustomerLoginThenComplainWriteReviewUpdateExit) {
    const std::string file = "complaint.dat";
    std::remove(file.c_str());

    writeLoginFile("customer.txt");

    Thread t;
    t.enableTestMode(true);
    TestCustomerRunner c(t);

    // Login:
    t.testPushRecv("u1");
    t.testPushRecv("p1");

    // Customer::home() menu:
    // 2 complain, then 4 exit home
    t.testPushRecv("2");

    // Customer::complain() menu:
    // 1 write -> text
    // 2 review
    // 3 update -> new text
    // 4 exit complain
    t.testPushRecv("1");
    t.testPushRecv("my issue");
    t.testPushRecv("2");
    t.testPushRecv("3");
    t.testPushRecv("updated issue");
    t.testPushRecv("4");

    // Exit home
    t.testPushRecv("4");

    c.login("customer.txt");

    // Should have sent TRUE at least once for successful write/update.
    bool sawTrue = false;
    while (t.testSentCount() > 0)
    {
        if (t.testPopSent() == "TRUE")
        {
            sawTrue = true;
            break;
        }
    }
    EXPECT_TRUE(sawTrue);

    std::remove(file.c_str());
    std::remove("customer.txt");
}

TEST(ComplainFlowTest, EmployeeHomeThenComplainSeeAnswerViewClearExit) {
    const std::string file = "complaint.dat";
    std::remove(file.c_str());

    // Seed a complaint record via customer login->complain flow.
    {
        writeLoginFile("customer.txt");
        Thread t0;
        t0.enableTestMode(true);
        TestCustomerRunner c0(t0);

        t0.testPushRecv("u1");
        t0.testPushRecv("p1");
        t0.testPushRecv("2");
        t0.testPushRecv("1");
        t0.testPushRecv("seed complaint"); // complaint text
        t0.testPushRecv("4");
        t0.testPushRecv("4");
        c0.login("customer.txt");
        std::remove("customer.txt");
    }

    Thread t;
    t.enableTestMode(true);
    TestEmployeeRunner e(t);

    // Employee::home() menu:
    // 2 complain, then 4 exit
    t.testPushRecv("2");

    // Employee::complain() menu:
    // 1 see complaint + mark seen
    // 3 answer
    // 4 view list (unseen/summary)
    // 5 view details
    // 6 clear
    // 7 exit
    t.testPushRecv("1");
    t.testPushRecv("u1");
    t.testPushRecv("3");
    t.testPushRecv("u1");
    t.testPushRecv("answer text");
    t.testPushRecv("4");
    t.testPushRecv("5");
    t.testPushRecv("6");
    t.testPushRecv("7");

    // exit home
    t.testPushRecv("4");

    e.runHomeForTest();

    bool sawTrue = false;
    while (t.testSentCount() > 0)
    {
        if (t.testPopSent() == "TRUE")
        {
            sawTrue = true;
            break;
        }
    }
    EXPECT_TRUE(sawTrue);

    std::remove(file.c_str());
}

