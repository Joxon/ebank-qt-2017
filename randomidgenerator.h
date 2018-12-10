#ifndef RANDOMIDGENERATOR_H
#define RANDOMIDGENERATOR_H

#include <QString>

class RandomIDGenerator
{
public:
   RandomIDGenerator();

   QString generateID();
   QString generatePhone();
   int genRandInt(int min, int max);

private:
   QString genTwoDigitCode(int max);
   QString genThreeDigitCode(int max);
   QString genFourDigitCode(int max);

   QString genBirthday(int minage, int maxage);
   QString genOneDigitVeriCode();
};

#endif // RANDOMIDGENERATOR_H
