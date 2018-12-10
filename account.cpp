#include "account.h"

QString Account::getName() const
{
   return name;
}


void Account::setName(const QString& value)
{
   name = value;
}


QString Account::getId() const
{
   return id;
}


void Account::setId(const QString& value)
{
   id = value;
}


QString Account::getBank() const
{
   return bank;
}


void Account::setBank(const QString& value)
{
   bank = value;
}


QString Account::getCardid() const
{
   return cardid;
}


void Account::setCardid(const QString& value)
{
   cardid = value;
}


QString Account::getPwd() const
{
   return pwd;
}


void Account::setPwd(const QString& value)
{
   pwd = value;
}


QString Account::getPhone() const
{
   return phone;
}


void Account::setPhone(const QString& value)
{
   phone = value;
}


double Account::getBalance() const
{
   return balance;
}


void Account::setBalance(double value)
{
   balance = value;
}


QString Account::getPwdpay() const
{
   return pwdpay;
}


void Account::setPwdpay(const QString& value)
{
   pwdpay = value;
}
