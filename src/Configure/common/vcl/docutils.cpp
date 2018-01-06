//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <ExtActns.hpp>

#include "docutils.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void DocUtils::openDoc(const string &url)
{
    TFileRun *fr = new TFileRun(Application);
    fr->Operation = "open";
    fr->FileName = url.c_str();
    fr->Execute();
    delete fr;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void DocUtils::browsePage(const string &url0)
{
    string url = url0;
    if (url.find_first_of("@") != string::npos) {
        url = "mailto:" + url;
    } else {
        if (url.find("http://") != 0 && url.find("https://")) {
            url = "http://" + url;
        }
    }
    openDoc(url);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void DocUtils::openFile(const string &fileName)
{
    if (FileExists(fileName.c_str())) {
        openDoc(fileName);
    } else {
        //Error file not exists
    }
    /*  pdfFind := FileExists( filename );
    	if (not pdfFind) then begin
    	Application.MessageBox(PChar(TextL10n.tr('doc_1')), PChar(Application.title), MB_OK+MB_ICONINFORMATION);
    	//Getting Started Guide file not found.
    	exit;
      end;

    	rez := ShellExecute(self.Handle, nil, PChar(filename), nil, nil, SW_RESTORE);
      */
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



