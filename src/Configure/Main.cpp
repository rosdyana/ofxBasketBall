//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "IniFiles.hpp"

#include "Main.h"

#include "docutils.h"
#include "paths.h"
#include "../_product/product.h"
#include "About.h"
#include <vector>
#include "converter.h"
#include "pbFiles.h"
#include "parsing.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;


//Список разрешений экранов:
//http://www.screenresolution.org/

std::vector<int> resolX;
std::vector<int> resolY;

std::vector<int> duration;

void addResol(int x, int y)
{
    resolX.push_back(x);
    resolY.push_back(y);
}

void fillResolutions()
{
    resolX.clear();
    resolY.clear();

    addResol(640, 360);
    addResol(640, 480);
    addResol(800, 450);
    addResol(800, 600);
    addResol(1024, 576);
    addResol(1024, 600);
    addResol(1024, 640);
    addResol(1024, 768);
    addResol(1024, 819);
    addResol(1066, 600);
    addResol(1093, 614);
    addResol(1152, 720);
    addResol(1152, 864);
    addResol(1280, 720);
    addResol(1280, 768);
    addResol(1280, 800);
    addResol(1280, 960);
    addResol(1280, 1024);
    addResol(1344, 840);
    addResol(1360, 768);
    addResol(1366, 768);
    addResol(1400, 1050);
    addResol(1440, 900);
    addResol(1600, 900);
    addResol(1600, 1200);
    addResol(1680, 1050);
    addResol(1920, 1080);
    addResol(1920, 1200);
    addResol(2560, 1440);
    addResol(2560, 1600);
}

void loadResolutions(const string &fileName)
{
    if (!pbFiles::fileExists(fileName)) {
        fillResolutions();
    } else {
        resolX.clear();
        resolY.clear();
        vector<string> list = pbFiles::readStrings(fileName);
        for (int i = 0; i < list.size(); i++) {
            vector<string> pair = Parsing::split(list[i], " ");
            if (pair.size() >= 2) {
                int x = Converter::strToInt(pair[0]);
                int y = Converter::strToInt(pair[1]);
                if (x > 0 && y > 0) {
                    addResol(x, y);
                }
            }
        }

    }

}

void fillDuration()
{
    duration.clear();
    duration.push_back(15);
    duration.push_back(30);
    duration.push_back(45);
    duration.push_back(60);
    duration.push_back(90);
    duration.push_back(120);
    duration.push_back(180);
    duration.push_back(300);
    duration.push_back(600);
    duration.push_back(900);
    duration.push_back(0);
}

//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent *Owner)
    : TForm(Owner)
{

    sharedPaths.init(ExtractFilePath(Application->ExeName).c_str());

    Caption = Caption + " " + PRODUCT_TITLE;
#ifdef PRODUCT_TYPE_TRIAL
    DurationCombo->Enabled = !PRODUCT_TRIAL_TIMELIMIT_ENABLED;
    Caption = Caption + " (TRIAL)";
    BuyButton->Visible = PRODUCT_BUY_ENABLED;
    trialLabel->Visible = true;
#endif

    CameraTypeKinectRadio->Enabled = PRODUCT_KINECT_ENABLED;
    miOnlineHelp->Visible = PRODUCT_ONLINEHELP_ENABLED;


    //Заполнение вариантов
    //fillResolutions();
    loadResolutions(sharedPaths.binPath() + "data/resolutions.ini");
    fillDuration();

    //Заполнение элементов
    ScreenResolutionCombo->Clear();
    for (int i = 0; i < resolX.size(); i++) {
        string title = Converter::intToStr(resolX[i])
                       + " x " + Converter::intToStr(resolY[i]);
        ScreenResolutionCombo->Items->Add(title.c_str());
    }

    DurationCombo->Clear();
    for (int i = 0; i < duration.size(); i++) {
        int sec = duration[i];
        int minute = sec / 60;
        sec %= 60;
        string title;
        if (minute > 0) {
            title += Converter::intToStr(minute) + " ";
            if (minute == 1) {
                title += "Minute";
            } else {
                title += "Minutes";
            }
        }
        if (sec > 0) {
            if (title != "") {
                title += " ";
            }
            title += Converter::intToStr(sec) + " Sec";
        }
        if (sec == 0 && minute == 0) {
            title = "Don't stop";
        }
        DurationCombo->Items->Add(title.c_str());
    }


    //Считывание ini
    readIni();

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::BuyButtonClick(TObject *Sender)
{
    DocUtils::browsePage(sharedPaths.buyUrl());
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::About1Click(TObject *Sender)
{
    AboutForm->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::miOnlineHelpClick(TObject *Sender)
{
    DocUtils::browsePage(sharedPaths.onlineHelpUrl());

}


//---------------------------------------------------------------------------
String readParam(String section, String key, String defaultValue,
                 TIniFile *defaultIni, TIniFile *userIni)
{
    String result = defaultValue;
    result = defaultIni->ReadString(section, key, result);
    result = userIni->ReadString(section, key, result);

    return result;

}

//---------------------------------------------------------------------------
void TMainForm::readIni()
{
    TIniFile *defaultIni = new TIniFile(sharedPaths.defaultSettingsIniFile().c_str());
    TIniFile *userIni = new TIniFile(sharedPaths.userSettingsIniFile().c_str());

    String screenW = readParam("Screen", "w", "320", defaultIni, userIni);
    String screenH = readParam("Screen", "h", "240", defaultIni, userIni);
    String fullScreen = readParam("Screen", "fullScreen", "0", defaultIni, userIni);
    String screenRotate = readParam("Screen", "rotate", "0", defaultIni, userIni);
    String screenMirror = readParam("Screen", "mirror", "0", defaultIni, userIni);
    String durationSec = readParam("Run", "durationSec", "0", defaultIni, userIni);
    String cameraDevice = readParam("Camera", "device", "0", defaultIni, userIni);
    String cameraUsePS3 = readParam("Camera", "PS3Eye", "0", defaultIni, userIni);
    String cameraUseKinect = readParam("Camera", "Kinect", "0", defaultIni, userIni);


    //Разрешение экрана
    {
        int w = Converter::strToDouble(screenW.c_str());
        int h = Converter::strToDouble(screenH.c_str());
        int index = -1;
        for (int i = 0; i < resolX.size(); i++) {
            if (w == resolX[i] && h == resolY[i]) {
                index = i;
                break;
            }
        }
        ScreenResolutionCombo->ItemIndex = index;
    }
    //Полный экран
    {
        int index = 0;
        if (fullScreen == "1") {
            index = 1;
        }
        if (fullScreen == "2") {
            index = 2;
        }
        ScreenFullscreenCombo->ItemIndex = index;
    }

    //Вращение экрана
    {
        ScreenMirror->Checked = (screenMirror == "1");

        int rotate =  Converter::strToInt(screenRotate.c_str());
        if (rotate >= 0 && rotate < 4) {
            RotationCombo->ItemIndex = rotate;
        }
    }

    //Длительность
#ifdef PRODUCT_TYPE_TRIAL
    if (PRODUCT_TRIAL_TIMELIMIT_ENABLED) {
        durationSec = Converter::doubleToStr(PRODUCT_TRIAL_PLAYING_SEC).c_str();
    }
#endif
    {
        int index = -1;
        int dur = Converter::strToDouble(durationSec.c_str());
        for (int i = 0; i < duration.size(); i++) {
            if (dur == duration[i]) {
                index = i;
                break;
            }
        }
        DurationCombo->ItemIndex = index;
    }

    //Камера
    {
        int index = -1;
        int camera = Converter::strToDouble(cameraDevice.c_str());
        if (camera >= 0 && camera < CameraDeviceCombo->Items->Count) {
            index = camera;
        }
        CameraDeviceCombo->ItemIndex = index;

        CameraTypeUSBRadio->Checked = true;
        if (cameraUsePS3 == "1") {
            CameraTypePS3Radio->Checked = true;
        }
        if (cameraUseKinect == "1") {
            CameraTypeKinectRadio->Checked = 2;
        }
    }



    delete userIni;
    delete defaultIni;
}

//---------------------------------------------------------------------------
void TMainForm::writeIni()
{
    String fileName = sharedPaths.userSettingsIniFile().c_str();
    TIniFile *userIni = new TIniFile(fileName);

    //Разрешение экрана
    {
        int index = ScreenResolutionCombo->ItemIndex;
        if (index >= 0) {
            int screenW = resolX[ index ];
            int screenH = resolY[ index ];
            userIni->WriteInteger("Screen", "w", screenW);
            userIni->WriteInteger("Screen", "h", screenH);
        }
        int fullScreen = ScreenFullscreenCombo->ItemIndex;//(ScreenFullScreen->Checked) ? 1 : 0;
        userIni->WriteInteger("Screen", "fullScreen", fullScreen);
        int screenMirror = (ScreenMirror->Checked) ? 1 : 0;
        userIni->WriteInteger("Screen", "mirror", screenMirror);
    }

    //Вращение экрана
    {
        int rotate = RotationCombo->ItemIndex;
        if (rotate >= 0 && rotate < 4) {
            userIni->WriteInteger("Screen", "rotate", rotate);
        }
    }

    //Длительность
    {
        int index = DurationCombo->ItemIndex;
        if (index >= 0) {
            userIni->WriteInteger("Run", "durationSec", duration[ index ]);
        }
    }

    //Камера
    {
        int index = CameraDeviceCombo->ItemIndex;
        if (index >= 0) {
            userIni->WriteInteger("Camera", "device", index);
        }
        int usePS3 = CameraTypePS3Radio->Checked ? 1 : 0;
        int useKinect = CameraTypeKinectRadio->Checked ? 1 : 0;
        userIni->WriteInteger("Camera", "PS3Eye", usePS3);
        userIni->WriteInteger("Camera", "Kinect", useKinect);
    }

    delete userIni;

}

//---------------------------------------------------------------------------

void __fastcall TMainForm::SaveButtonClick(TObject *Sender)
{
    writeIni();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    writeIni();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button1Click(TObject *Sender)
{
    writeIni();
    DocUtils::openFile(sharedPaths.runAppCommand());
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::GettingStartedGuide1Click(TObject *Sender)
{
    DocUtils::openFile(sharedPaths.gettingStartedFileName());
}
//---------------------------------------------------------------------------


