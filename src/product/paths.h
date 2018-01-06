//---------------------------------------------------------------------------

#ifndef pathsH
#define pathsH

#include <string>
using namespace std;

//������ � ������ � �������� ������ ���������

class Paths
{
public:
    Paths();
    void init(const string &binPath);

    string binPath();			//���� � ����������� �������
    string settingsPath();		//���� � ����������
    string docPath();			//���� � ����������
    string siteUrl();			//����
    string onlineHelpUrl();
    string gettingStartedFileName();
    string supportUrl();		//������� �� �������� support
    string buyUrl();			//�������� �������

    string defaultSettingsIniFile();
    string userSettingsIniFile();
    string runAppCommand();		//������� ��� ������� �������� ����������

private:
    string _binPath;
    string _settingsPath;
    string _docPath;

};

extern Paths sharedPaths;

//---------------------------------------------------------------------------
#endif
