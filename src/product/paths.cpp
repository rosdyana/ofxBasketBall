//---------------------------------------------------------------------------

//#include <vcl.h>
#pragma hdrstop
#include <string>
#include <windows.h>
#include <shlobj.h>

//#include "product.h"

#include "paths.h"
#define PRODUCT_MANUFACTURER "Naga Biru"
#define PRODUCT_NAME "OfxBasketBall"
#define PRODUCT_EXE PRODUCT_NAME
//---------------------------------------------------------------------------

Paths sharedPaths;

//---------------------------------------------------------------------------
string getAppDataPath()
{
    string result = "";
    TCHAR szPath[MAX_PATH];

    if (SUCCEEDED(SHGetFolderPath(NULL,
                                  CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE,
                                  //CSIDL_APPDATA
                                  NULL,
                                  0,
                                  szPath))) {
        //result = szPath;
    }

    return result;

}




//---------------------------------------------------------------------------
Paths::Paths()
{

}

void Paths::init(const string &binPath)
{
    _binPath = binPath; //ExtractFilePath( Application->ExeName ).c_str();
    _docPath 		= _binPath;

    string manufDir = getAppDataPath() + "\\" + PRODUCT_MANUFACTURER + "\\";
    _settingsPath 	= manufDir + PRODUCT_NAME + "\\";

    CreateDirectoryA(manufDir.c_str(), 0);
    CreateDirectoryA(_settingsPath.c_str(), 0);
}


//---------------------------------------------------------------------------
string Paths::binPath()			//путь к исполняемым модулям
{
    return _binPath;
}

//---------------------------------------------------------------------------
string Paths::settingsPath()		//путь к настройкам
{
    return _settingsPath;
}

//---------------------------------------------------------------------------
string Paths::docPath()			//путь к документам
{
    return _docPath;
}


//---------------------------------------------------------------------------
string Paths::defaultSettingsIniFile()
{
    return binPath() + "data/settings.ini";
}

//---------------------------------------------------------------------------
string Paths::userSettingsIniFile()
{
    return settingsPath() + "settings-user.ini";
}

//---------------------------------------------------------------------------
string Paths::runAppCommand()		//команда для запуска главного приложения
{
    return binPath() + PRODUCT_EXE + ".exe";
}

//---------------------------------------------------------------------------

