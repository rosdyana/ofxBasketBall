#pragma once

//Защита программы

#include "ofMain.h"
#include "ofxArgs.h"

struct pbProtectionParam {
    pbProtectionParam()
    {
        logo = false;
        date = false;
    }
    bool logo;	//рисовать лого
    bool date;	//использовать дату
    int year, month, day;	//дата
    string toString() const;
    void loadFromString(const string &s);
    void setDateFromString(const string &s);
    string dateToString() const
    {
        return ofToString(year) + "." + ofToString(month) + "." + ofToString(day);
    }
};

class pbProtection
{
public:
    pbProtection(void);
    ~pbProtection(void);

    string defaultFile()
    {
        return "licence.ini";
    }

    //сгенерировать код проверки и записать его в файл
    //код зависит от длин файлов .png и .jpg в data
    void writeProtectionCode(const string &fileName, bool protectionEnabled,
                             const pbProtectionParam &param);

    //проверить код проверки, записанный в файле
    bool checkProtectionCode(const string &fileName, string &reason);

    pbProtectionParam param()
    {
        return _param;
    }

private:
    string getCode(const pbProtectionParam &param);		//расчет кода проверки
    string _noProtectCode;	//кода, отменяющий проверку

    pbProtectionParam _param;
};

extern pbProtection shared_protection;

void pbProtectionSetup(ofxArgs *args = 0);		//считывание и проверка лицензии
