#pragma once
#include "Person.h"

class Employee: public Person
{

    protected:
        void home() override;
        void complain();
    public:
        Employee() : Person() {}
        Employee(Thread& server) : Person(server) {}
};
