#pragma once

class Cash
{
    double initial = 0.0, in = 0.0, out = 0.0, finl = 0.0;
    friend class Person;

    public:
        double get_final_cash(void);
        
        // Operator overloading
        void operator +(double i); // Cash in
        void operator -(double i); // Cash out
};

inline void Cash::operator +(double i) // Operator for Cash in
{
    in += i;
    finl += i;
}

inline void Cash::operator -(double i) // Operator for Cash out
{
    out += i;
    finl -= i;
}

inline double Cash::get_final_cash(void) // Returns final cash
{  
    return finl;
}
