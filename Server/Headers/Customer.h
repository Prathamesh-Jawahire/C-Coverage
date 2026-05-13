#pragma once
#include "Person.h"

class Customer: public Person
{
    void signup();
    void home() override;
    void complain();

    public:
        Customer() : Person() {}
        Customer(Thread& server) : Person(server) {};
        void start();
};
