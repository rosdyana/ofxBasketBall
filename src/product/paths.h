//---------------------------------------------------------------------------

#ifndef pathsH
#define pathsH

#include <string>
using namespace std;

//работа с пут€ми к основным файлам программы

class Paths
{
public:
    Paths();
    void init(const string &binPath);

    string binPath();			//путь к исполн€емым модул€м
    string settingsPath();		//путь к настройкам
    string docPath();			//путь к документам
    string siteUrl();			//сайт
    string onlineHelpUrl();
    string gettingStartedFileName();
    string supportUrl();		//переход на страницу support
    string buyUrl();			//страница покупки

    string defaultSettingsIniFile();
    string userSettingsIniFile();
    string runAppCommand();		//команда дл€ запуска главного приложени€

private:
    string _binPath;
    string _settingsPath;
    string _docPath;

};

extern Paths sharedPaths;

//---------------------------------------------------------------------------
#endif
