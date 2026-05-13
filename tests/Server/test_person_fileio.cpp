#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>

#include "../../Server/Headers/Person.h"

namespace {

class TestPerson final : public Person {
public:
    TestPerson() : Person() {}

    void setLookupForTest(const std::string& v) { lookup = v; }
    void setBalanceForTest(double v) { balance = std::to_string(v); }
    void setUsernameForTest(const std::string& v) { username = v; }
    void setPasswordForTest(const std::string& v) { password = v; }

    bool transferToFileForTest(const std::string& fileName) { return transfer_to_file(fileName); }
    bool initializeBalanceForTest(const std::string& fileName) { return initialize_balance(fileName); }
    bool updateBalanceForTest(const std::string& fileName) { return update_balance(fileName); }
};

static std::string readAll(const std::string& path) {
    std::ifstream f(path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

} // namespace

TEST(PersonFileIOTest, TransferToFileWritesBasicRecord) {
    const std::string file = "person_transfer_to_file_test.txt";
    std::remove(file.c_str());

    TestPerson p;
    p.setUsernameForTest("u1");
    p.setPasswordForTest("p1");
    p.setName("Alice");
    p.setAge(30);

    ASSERT_TRUE(p.transferToFileForTest(file));

    const std::string content = readAll(file);
    EXPECT_NE(content.find("Username: u1"), std::string::npos);
    EXPECT_NE(content.find("Password: p1"), std::string::npos);
    EXPECT_NE(content.find("Name: Alice"), std::string::npos);
    EXPECT_NE(content.find("Age: 30"), std::string::npos);

    std::remove(file.c_str());
}

TEST(PersonFileIOTest, InitializeBalanceFindsBalanceForLookup) {
    const std::string file = "person_initialize_balance_test.txt";
    std::remove(file.c_str());

    {
        std::ofstream f(file);
        f << "Username: someone\n"
          << "Balance: 11\n\n"
          << "Username: target\n"
          << "Balance: 42\n\n";
    }

    TestPerson p;
    p.setLookupForTest("Username: target");
    ASSERT_TRUE(p.initializeBalanceForTest(file));

    EXPECT_EQ(p.getAge(), 0); // unrelated but ensures object usable

    std::remove(file.c_str());
}

TEST(PersonFileIOTest, UpdateBalanceRewritesBalanceLineForLookup) {
    const std::string file = "person_update_balance_test.txt";
    std::remove(file.c_str());

    {
        std::ofstream f(file);
        f << "Username: target\n"
          << "Password: x\n"
          << "Balance: 10\n\n"
          << "Username: other\n"
          << "Balance: 99\n\n";
    }

    TestPerson p;
    p.setLookupForTest("Username: target");
    p.setBalanceForTest(123.0);

    ASSERT_TRUE(p.updateBalanceForTest(file));

    const std::string content = readAll(file);
    EXPECT_NE(content.find("Username: target"), std::string::npos);
    EXPECT_NE(content.find("Balance: 123"), std::string::npos);
    EXPECT_NE(content.find("Username: other"), std::string::npos);
    EXPECT_NE(content.find("Balance: 99"), std::string::npos);

    std::remove(file.c_str());
}

