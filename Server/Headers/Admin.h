#pragma once
#include "Employee.h"

class Admin: public Employee
{
    protected:

    void home() override;
    void add_employee(void);
    void view(const string& fileName);
    void search(const string& fileName );
    void search(const string& str, const string& fileName);
    void Delete(const string& fileName);
    void balance(const string& fileName);
    void accounts(void);

    public:
        void stock(void); // friend of goods
        Admin() : Employee() {}
        Admin(Thread& server) : Employee(server) {}
};