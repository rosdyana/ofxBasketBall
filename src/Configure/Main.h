//---------------------------------------------------------------------------

#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <XPMan.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu1;
	TMenuItem *N1;
	TMenuItem *About1;
	TXPManifest *XPManifest1;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TButton *Button1;
	TComboBox *CameraDeviceCombo;
	TLabel *Label1;
	TLabel *Label2;
	TComboBox *ScreenResolutionCombo;
	TLabel *Label3;
	TComboBox *DurationCombo;
	TButton *Button2;
	TButton *SaveButton;
	TButton *BuyButton;
	TMenuItem *N2;
	TMenuItem *miOnlineHelp;
	TMenuItem *GettingStartedGuide1;
	TLabel *trialLabel;
	TRadioButton *CameraTypeUSBRadio;
	TRadioButton *CameraTypePS3Radio;
	TRadioButton *CameraTypeKinectRadio;
	TLabel *Label4;
	TComboBox *RotationCombo;
	TCheckBox *ScreenMirror;
	TComboBox *ScreenFullscreenCombo;
	TLabel *Label5;
	void __fastcall BuyButtonClick(TObject *Sender);
	void __fastcall About1Click(TObject *Sender);
	void __fastcall miOnlineHelpClick(TObject *Sender);
	void __fastcall SaveButtonClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall GettingStartedGuide1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TMainForm(TComponent* Owner);

	void readIni();
	void writeIni();

};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
