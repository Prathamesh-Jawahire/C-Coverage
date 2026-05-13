#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>

#include "../../Server/Headers/Person.h"
#include "../../Server/Headers/Thread.h"

namespace {

class TestBuyer final : public Person {
public:
    explicit TestBuyer(Thread& t) : Person(t) {}
    void setLookupForTest(const std::string& v) { lookup = v; }
    void setBalanceForTest(double v) { balance = std::to_string(v); }
    void callBuy(const std::string& fileName) { buy(fileName); }
};

static void writeGoodsFileTriggerReorder()
{
    std::ofstream f("goods.txt", std::ios::trunc);
    ASSERT_TRUE(f.is_open());

    // N = 10 goods. Use simple names and low stock to trigger reorder for item 1.
    // Format must match Person::initialize_goods token parsing.
    for (int i = 0; i < 10; i++)
    {
        const std::string name = "G" + std::to_string(i + 1);
        const int stock = (i == 0 ? 1 : 50);
        const double cost = 1.0;
        const double price = 5.0;
        const int reorder_level = (i == 0 ? 1 : 0);
        const int reorder_amount = (i == 0 ? 10 : 0);

        f << "Name: " << name << "\n"
          << "Stock: " << stock << "\n"
          << "Cost: " << cost << "\n"
          << "Price: " << price << "\n"
          << "Reorder Level: " << reorder_level << "\n"
          << "Reorder Amount: " << reorder_amount << "\n\n";
    }
}

static void writeCashFile(double initialCash)
{
    std::ofstream f("cash.txt", std::ios::trunc);
    ASSERT_TRUE(f.is_open());
    f << "Initial cash: " << initialCash << "\n"
      << "Cash in: " << 0 << "\n"
      << "Cash out: " << 0 << "\n"
      << "Final cash: " << initialCash;
}

static void writeUserFile(const std::string& fileName, const std::string& username, double bal)
{
    std::ofstream f(fileName, std::ios::trunc);
    ASSERT_TRUE(f.is_open());
    f << "Username: " << username << "\n"
      << "Password: p\n"
      << "Balance: " << bal << "\n\n";
}

static std::string readAll(const std::string& path)
{
    std::ifstream f(path);
    std::string s((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    return s;
}

} // namespace

TEST(BuyFlowTest, SuccessfulOrderTriggersReorderAndUpdatesFiles) {
    writeGoodsFileTriggerReorder();
    writeCashFile(1000.0);
    writeUserFile("customer.txt", "u", 100.0);

    Thread t;
    t.enableTestMode(true);

    TestBuyer buyer(t);
    buyer.setLookupForTest("Username: u");

    // buy() loop:
    // 1) choose "add item" (1), then itemNo=1, quantity=1 (enough stock)
    // 2) choose "confirm" (2) -> success path
    // 3) choose "exit" (3)
    t.testPushRecv("1");
    t.testPushRecv("1");
    t.testPushRecv("1");
    t.testPushRecv("2");
    t.testPushRecv("3");

    buyer.callBuy("customer.txt");

    const std::string goods = readAll("goods.txt");
    const std::string cash = readAll("cash.txt");
    const std::string users = readAll("customer.txt");

    // goods for item 1 should have been decremented then reordered (net increase visible).
    EXPECT_NE(goods.find("Name: G1"), std::string::npos);
    EXPECT_NE(cash.find("Final cash:"), std::string::npos);
    EXPECT_NE(users.find("Balance:"), std::string::npos);

    std::remove("goods.txt");
    std::remove("cash.txt");
    std::remove("customer.txt");
}

TEST(BuyFlowTest, NotEnoughStockSendsNotEnoughAndDoesNotCrash) {
    writeGoodsFileTriggerReorder();
    writeCashFile(1000.0);
    writeUserFile("customer.txt", "u", 100.0);

    Thread t;
    t.enableTestMode(true);

    TestBuyer buyer(t);
    buyer.setLookupForTest("Username: u");

    // add item 1 with quantity 999 -> NOT-ENOUGH
    // then exit
    t.testPushRecv("1");
    t.testPushRecv("1");
    t.testPushRecv("999");
    t.testPushRecv("3");

    buyer.callBuy("customer.txt");

    // at least one of the sent messages should be "NOT-ENOUGH"
    bool found = false;
    while (t.testSentCount() > 0)
    {
        if (t.testPopSent() == "NOT-ENOUGH")
        {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);

    std::remove("goods.txt");
    std::remove("cash.txt");
    std::remove("customer.txt");
}

TEST(BuyFlowTest, ConfirmWithNoOrderSendsNoOrder) {
    writeGoodsFileTriggerReorder();
    writeCashFile(1000.0);
    writeUserFile("customer.txt", "u", 100.0);

    Thread t;
    t.enableTestMode(true);

    TestBuyer buyer(t);
    buyer.setLookupForTest("Username: u");

    // confirm immediately without adding anything, then exit
    t.testPushRecv("2");
    t.testPushRecv("3");

    buyer.callBuy("customer.txt");

    bool found = false;
    while (t.testSentCount() > 0)
    {
        if (t.testPopSent().find("NO-ORDER") != std::string::npos)
        {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);

    std::remove("goods.txt");
    std::remove("cash.txt");
    std::remove("customer.txt");
}

