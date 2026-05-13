#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>

#include "../../Server/Headers/Admin.h"
#include "../../Server/Headers/Thread.h"

namespace {

class TestAdminFlows final : public Admin {
public:
    explicit TestAdminFlows(Thread& t) : Admin(t) {}
    using Admin::accounts;
    using Admin::balance;
    using Admin::Delete;
    using Admin::search;
    using Admin::stock;
    using Admin::view;
};

static void writeUsers(const std::string& fileName)
{
    std::ofstream f(fileName, std::ios::trunc);
    ASSERT_TRUE(f.is_open());
    f << "Username: u1\nPassword: p\nBalance: 10\n\n"
      << "Username: u2\nPassword: p\nBalance: 20\n\n";
}

static void writeCash(double initialCash)
{
    std::ofstream f("cash.txt", std::ios::trunc);
    ASSERT_TRUE(f.is_open());
    f << "Initial cash: " << initialCash << "\n"
      << "Cash in: 0\n"
      << "Cash out: 0\n"
      << "Final cash: " << initialCash;
}

static void writeGoods()
{
    std::ofstream f("goods.txt", std::ios::trunc);
    ASSERT_TRUE(f.is_open());
    for (int i = 0; i < 10; i++)
    {
        f << "Name: G" << (i + 1) << "\n"
          << "Stock: 10\n"
          << "Cost: 1\n"
          << "Price: 5\n"
          << "Reorder Level: 1\n"
          << "Reorder Amount: 5\n\n";
    }
}

} // namespace

TEST(AdminFlowTest, ViewAndAccountsSendData) {
    writeUsers("customer.txt");
    writeCash(123.0);

    Thread t;
    t.enableTestMode(true);
    TestAdminFlows a(t);

    a.view("customer.txt");
    a.accounts();

    // We should have sent at least 2 messages (file contents).
    EXPECT_GE(t.testSentCount(), 2u);

    std::remove("customer.txt");
    std::remove("cash.txt");
}

TEST(AdminFlowTest, BalanceUpdatesRecordAndDeleteRemovesRecord) {
    writeUsers("customer.txt");

    Thread t;
    t.enableTestMode(true);
    TestAdminFlows a(t);

    // balance(): Rec(user), Rec(balance)
    t.testPushRecv("u1");
    t.testPushRecv("777");
    a.balance("customer.txt");

    // Delete(): Rec(user)
    t.testPushRecv("u2");
    a.Delete("customer.txt");

    // Ensure it reported success for balance update.
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

    std::ifstream f("customer.txt");
    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    EXPECT_NE(content.find("Username: u1"), std::string::npos);
    EXPECT_NE(content.find("Balance: 777"), std::string::npos);
    EXPECT_EQ(content.find("Username: u2"), std::string::npos);

    std::remove("customer.txt");
}

TEST(AdminFlowTest, SearchByUsernameFindsAndSendsProfile) {
    writeUsers("customer.txt");

    Thread t;
    t.enableTestMode(true);
    TestAdminFlows a(t);

    // Admin::search(file): loop reads menu choice.
    // choose 4 => "Username", then provide username "u1", then exit (6).
    t.testPushRecv("4");
    t.testPushRecv("u1");
    t.testPushRecv("6");
    a.search("customer.txt");

    bool sawProfile = false;
    while (t.testSentCount() > 0)
    {
        const std::string msg = t.testPopSent();
        if (msg.find("--> Username: u1") != std::string::npos)
        {
            sawProfile = true;
            break;
        }
    }
    EXPECT_TRUE(sawProfile);

    std::remove("customer.txt");
}

TEST(AdminFlowTest, StockCase2ModifiesPriceAndLevels) {
    writeGoods();
    writeCash(9999.0);

    Thread t;
    t.enableTestMode(true);
    TestAdminFlows a(t);

    // stock():
    // Rec(ans)
    // case 2 => change price/reorder params
    // Rec(name), Rec(price), Rec(level), Rec(amount)
    t.testPushRecv("2");
    t.testPushRecv("G1");
    t.testPushRecv("9.5");
    t.testPushRecv("2");
    t.testPushRecv("7");

    a.stock();

    std::ifstream f("goods.txt");
    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    EXPECT_NE(content.find("Name: G1"), std::string::npos);
    EXPECT_NE(content.find("Price: 9.5"), std::string::npos);
    EXPECT_NE(content.find("Reorder Level: 2"), std::string::npos);
    EXPECT_NE(content.find("Reorder Amount: 7"), std::string::npos);

    std::remove("goods.txt");
    std::remove("cash.txt");
}

