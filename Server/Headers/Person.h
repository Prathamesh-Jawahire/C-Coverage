#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <conio.h>
#include <cstdio>
#include <cstring>
#include "Server.h"
using namespace std;

// N is the number of goods we sell.
#define N (10)

// Forward declaration of classes
class Person;
class Employee;
class Customer;
class Admin;
class Goods;
class Cash;

const string customerFile = "customer.txt";
const string adminFile = "admin.txt";
const string empFile = "emp.txt";

struct Date
{
    string day, month, year;
};

class Person
{
    protected:
        string name, CNIC, email, phone_num, username, password;
        string sex;
        string age;
        string balance;
        Date DOB;
        string lookup;
        Thread *server;

        bool input(const string& fileName);
        bool transfer_to_file(const string& fileName);
        
        // In production, derived types override this.
        // For unit tests, we keep a default no-op implementation so `Person` is instantiable.
        virtual void home() {} 

        // Methods for initialaizing and updating
        bool initialize_goods(Goods* goods);
        bool initialize_balance(string fileName);
        bool initialize_cash(Cash& cash);
        bool update_goods(Goods* goods);
        bool update_balance(string fileName);
        bool update_cash(Cash& cash);

        // Other methods
        void profile(const string& fileName, const string& Lookup);
        void buy(const string& fileName);

    public:
        Person() : server(nullptr) {}
        Person(const string& n, int a) : server(nullptr) { setName(n); setAge(a); }
        Person(Thread& server) : server(&server) {}

        void setName(const string& n) { name = n; }
        string getName() const { return name; }

        void setAge(int a) { age = to_string(a); }
        int getAge() const { return age.empty() ? 0 : stoi(age); }

        void login(const string& fileName);   
};
