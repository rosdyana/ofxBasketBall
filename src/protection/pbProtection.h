#pragma once

//������ ���������

#include "ofMain.h"
#include "ofxArgs.h"

struct pbProtectionParam {
    pbProtectionParam()
    {
        logo = false;
        date = false;
    }
    bool logo;	//�������� ����
    bool date;	//������������ ����
    int year, month, day;	//����
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

    //������������� ��� �������� � �������� ��� � ����
    //��� ������� �� ���� ������ .png � .jpg � data
    void writeProtectionCode(const string &fileName, bool protectionEnabled,
                             const pbProtectionParam &param);

    //��������� ��� ��������, ���������� � �����
    bool checkProtectionCode(const string &fileName, string &reason);

    pbProtectionParam param()
    {
        return _param;
    }

private:
    string getCode(const pbProtectionParam &param);		//������ ���� ��������
    string _noProtectCode;	//����, ���������� ��������

    pbProtectionParam _param;
};

extern pbProtection shared_protection;

void pbProtectionSetup(ofxArgs *args = 0);		//���������� � �������� ��������
