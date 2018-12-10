#ifndef BANK_H
#define BANK_H

#include <QString>

class Bank
{
public:
   Bank();

   QString getName() const;
   void setName(const QString& value);

   QString getPrefix() const;
   void setPrefix(const QString& value);

   int getCount() const;
   void setCount(int value);

private:
   QString name;
   QString prefix;
   int count;
};

#endif // BANK_H
