#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>

class Account
{
public:
   Account() {}
   ~Account() {}

   QString getName() const;
   void setName(const QString& value);

   QString getId() const;
   void setId(const QString& value);

   QString getBank() const;
   void setBank(const QString& value);

   QString getCardid() const;
   void setCardid(const QString& value);

   QString getPwd() const;
   void setPwd(const QString& value);

   QString getPhone() const;
   void setPhone(const QString& value);

   double getBalance() const;
   void setBalance(double value);

   QString getPwdpay() const;
   void setPwdpay(const QString& value);

private:
   QString name;
   QString id;
   QString bank;
   QString cardid;
   QString pwd;
   QString pwdpay;
   QString phone;
   double balance;
};

#endif // ACCOUNT_H
