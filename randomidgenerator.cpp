#include "randomidgenerator.h"

#include <QStringList>
#include <QTime>
#include <QDate>

RandomIDGenerator::RandomIDGenerator()
{
}


QString RandomIDGenerator::generateID()
{
   QTime time = QTime::currentTime();

   qsrand((uint)time.msec());

   QStringList provinceCodes;

   provinceCodes << "11" << "12" << "13" << "14" << "15" << "21" << "22" << "23" <<
      "31" << "32" << "33" << "34" << "35" << "36" << "37" << "41" << "42" << "43" <<
      "44" << "45" << "46" << "50" << "51" << "52" << "53" << "54" << "61" << "62" <<
      "63" << "64" << "65" << "71" << "81" << "82";


   QString provinceCode = provinceCodes.at(genRandInt(0, provinceCodes.length() - 1));
   QString cityCode     = genTwoDigitCode(18);
   QString countyCode   = genTwoDigitCode(28);
   QString birthday     = genBirthday(18, 60);
   QString seqCode      = genThreeDigitCode(999);
   QString veriCode     = genOneDigitVeriCode();

   return provinceCode + cityCode + countyCode + birthday + seqCode + veriCode;
}


QString RandomIDGenerator::generatePhone()
{
   QTime time = QTime::currentTime();

   qsrand((uint)time.msec());

   QStringList phonePrefixes;

   phonePrefixes << "130" << "131" << "132" << "133" << "134" << "135" << "136" << "137" << "138" << "139"
                 << "145" << "147"
                 << "150" << "151" << "152" << "153" << "155" << "156" << "157" << "158" << "159"
                 << "176" << "177" << "178"
                 << "180" << "181" << "182" << "183" << "185" << "186" << "187" << "188" << "189";

   QString phonePrefix = phonePrefixes.at(genRandInt(0, phonePrefixes.length() - 1));
   QString cityCode    = genFourDigitCode(9999);
   QString seqCode     = genFourDigitCode(9999);

   return phonePrefix + cityCode + seqCode;
}


QString RandomIDGenerator::genTwoDigitCode(int max)
{
   return QString::number(genRandInt(0, max)).rightJustified(2, '0', true);
}


QString RandomIDGenerator::genBirthday(int minage, int maxage)
{
   int days = 365 * minage + genRandInt(0, 365 * (maxage - minage));

   return QDate::currentDate().addDays(-days).toString("yyyyMMdd");
}


QString RandomIDGenerator::genThreeDigitCode(int max)
{
   return QString::number(genRandInt(0, max)).rightJustified(3, '0', true);
}


QString RandomIDGenerator::genFourDigitCode(int max)
{
   return QString::number(genRandInt(0, max)).rightJustified(4, '0', true);
}


QString RandomIDGenerator::genOneDigitVeriCode()
{
   QStringList vcodes;

   vcodes << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" <<
      "X";

   return vcodes.at(genRandInt(0, vcodes.length() - 1));
}


int RandomIDGenerator::genRandInt(int min, int max)
{
   return qrand() % ((max + 1) - min) + min;
}
