#include "pbProtection.h"
#include "ofxDirList.h"
#include "pbFiles.h"

pbProtection shared_protection;


//---------------------------------------------------------------------------
string pbProtectionParam::toString() const
{
    vector<string> list;
    if (logo) {
        list.push_back("logo");
    }
    if (date) {
        list.push_back("date");
        list.push_back(dateToString());
    }
    string s = "";
    for (int i = 0; i < list.size(); i++) {
        if (i >= 1) {
            s += " ";
        }
        s += list[i];
    }
    return s;
}

//---------------------------------------------------------------------------
void pbProtectionParam::setDateFromString(const string &s)
{
    date = false;
    year = 0;
    month = 0;
    day = 0;
    vector<string> list = ofSplitString(s, ".");
    if (list.size() >= 3) {
        date = true;
        year = ofToInt(list[0]);
        month = ofToInt(list[1]);
        day = ofToInt(list[2]);
    }
}

//---------------------------------------------------------------------------
void pbProtectionParam::loadFromString(const string &s)
{
    logo = false;
    date = false;
    vector<string> list = ofSplitString(s, " ");
    for (int i = 0; i < list.size(); i++) {
        string s = list[i];
        if (s == "logo") {
            logo = true;
        }
        if (s == "date" && i + 1 < list.size()) {
            setDateFromString(list[ i + 1 ]);
        }
    }

}

//---------------------------------------------------------------------------
pbProtection::pbProtection(void)
{
    _noProtectCode = "-----";
}

//---------------------------------------------------------------------------
pbProtection::~pbProtection(void)
{
}

//---------------------------------------------------------------------------
//рекурсивная функция взятия сумм длин файлов изображений в папке dir
//TODO сейчас int, для огромных файлов может некорректно сработать (>2гб)
int getSumImages(const string dir)
{
    int sum = 0;
    ofxDirList dirList;
    int n = dirList.listDir(dir);
    if (n > 0) {
        for (int i = 0; i < n; i++) {
            string fileName = dirList.getPath(i);
            string ext = pbFiles::getExtension(fileName);
            if (ext == "") {	//это может быть директория
                sum += getSumImages(fileName);
            }
            if (ext == "jpg" || ext == "png" || ext == "bmp") {
                sum += pbFiles::fileSize(fileName);
            }
        }
    }
    return sum;
}

//---------------------------------------------------------------------------
string pbProtection::getCode(const pbProtectionParam &param)		//расчет кода проверки
{
    //сканируем папку data рекурсивно
    int sum = getSumImages("data/");			//TODO не сработает для MacOS
    if (param.date) {
        sum += param.year;
        sum += param.month * 337;
        sum += param.day * 173;
    }
    if (param.logo) {
        sum += 17;
    }
    int code = sum % 99881;
    return ofToString(code);
}

//---------------------------------------------------------------------------
//сгенерировать код проверки и записать его в файл
//код зависит от длин файлов .png и .jpg в data
void pbProtection::writeProtectionCode(const string &fileName, bool protectionEnabled, const pbProtectionParam &param)
{
    string code = (protectionEnabled) ? getCode(param) : _noProtectCode;
    vector<string> list;
    list.push_back(code);
    list.push_back(param.toString());
    pbFiles::writeStrings(list, ofToDataPath(fileName));
}

//---------------------------------------------------------------------------
//проверить код проверки, записанный в файле
bool pbProtection::checkProtectionCode(const string &fileName, string &reason)
{
    bool result = false;
    reason = "";
    vector<string> list = pbFiles::readStrings(ofToDataPath(fileName));
    if (list.size() >= 2) {
        string code = list[0];
        pbProtectionParam param;
        param.loadFromString(list[1]);
        _param = param;

        result = (code == _noProtectCode || code == getCode(param));
        if (!result) {
            reason = "Bad licence number or image files are changed/corrupted";
        } else {
            if (param.date) {
                //проверка даты
                int year = ofGetYear();
                int month = ofGetMonth();
                int day = ofGetDay();
                if (!(year < param.year
                        || year == param.year
                        && (month < param.month || month == param.month && day < param.day)
                     )) {
                    result = false;
                    reason = "Date is out, license stops at " + param.dateToString();
                }
            }
        }
    } else {
        reason = "Bad licence file";
    }

    return result;
}

//---------------------------------------------------------------------------
void pbProtectionSetup(ofxArgs *args)		//считывание и проверка лицензии
{
    //Установка лицензии
    if (args) {
        bool finish = false;

        if (args->contains("protect")) {
            pbProtectionParam param;
            param.logo = args->contains("logo");
            param.setDateFromString(args->getString("date"));
            shared_protection.writeProtectionCode(shared_protection.defaultFile(), true, param);
            finish = true;
            cout << endl << "Licence changed: Protection is on/updated, " << param.toString() << endl;
        }
        if (args->contains("noprotect")) {
            shared_protection.writeProtectionCode(shared_protection.defaultFile(), false, pbProtectionParam());
            finish = true;
            cout << endl << "Licence changed: Protection is off" << endl;
        }
        if (finish) {
            cout << endl << "All is ok, and closing app now... Please restart app. " << endl;
            ofSleepMillis(1000);
            OF_EXIT_APP(0);
        }
    }

    //Проверка лицензии (не должна использовать args)
    string reason;
    bool protectionOk = shared_protection.checkProtectionCode(shared_protection.defaultFile(), reason);
    if (protectionOk)	{
        cout << "Licence: ok" << endl;
    } else {
        cout << endl << "ERROR: Bad licence - " << reason << endl;
    }
    if (!protectionOk) {
        cout << endl << "Closing app now... Please contact technical support on updating licence" << endl;
        ofSleepMillis(5000);
        OF_EXIT_APP(0);
    }
    //Рисовать ли лого
    /*_logo = shared_protection.param().logo;
    if ( _logo ) {
    	_logoImage.loadImage( "images/logo.png" );
    	if ( _logoImage.width == 0 ) {
    		cout << endl << "ERROR: No logo" << endl;
    		cout << endl << "Closing app now... Please contact technical support on updating licence" << endl;
    		ofSleepMillis( 5000 );
    		OF_EXIT_APP( 0 );
    	}
    }*/


}