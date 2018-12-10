#include "bank.h"

Bank::Bank()
{
}


QString Bank::getName() const
{
   return name;
}


void Bank::setName(const QString& value)
{
   name = value;
}


QString Bank::getPrefix() const
{
   return prefix;
}


void Bank::setPrefix(const QString& value)
{
   prefix = value;
}


int Bank::getCount() const
{
   return count;
}


void Bank::setCount(int value)
{
   count = value;
}
