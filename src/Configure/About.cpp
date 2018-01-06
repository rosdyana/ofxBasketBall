//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "About.h"
#include "../_product/product.h"
#include "docutils.h"
#include "paths.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAboutForm *AboutForm;
//---------------------------------------------------------------------------
__fastcall TAboutForm::TAboutForm(TComponent* Owner)
	: TForm(Owner)
{

	productLabel->Caption = PRODUCT_TITLE;
	versionLabel->Caption = String("Version ") + PRODUCT_VERSION;
	descriptionLabel->Caption = PRODUCT_DESCRIPTION;
	siteLabel->Caption = "www.playbat.net";

#ifdef PRODUCT_TYPE_TRIAL
	trialLabel->Visible = true;
#endif
}
//---------------------------------------------------------------------------
void __fastcall TAboutForm::siteLabelClick(TObject *Sender)
{
	DocUtils::browsePage( sharedPaths.siteUrl() );
}
//---------------------------------------------------------------------------

