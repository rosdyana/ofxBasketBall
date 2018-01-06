object MainForm: TMainForm
  Left = 0
  Top = 0
  Caption = 'Configure'
  ClientHeight = 292
  ClientWidth = 437
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 8
    Top = -1
    Width = 329
    Height = 282
    ActivePage = TabSheet1
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'Screen'
      object Label2: TLabel
        Left = 16
        Top = 9
        Width = 50
        Height = 13
        Caption = 'Resolution'
      end
      object Label3: TLabel
        Left = 16
        Top = 176
        Width = 41
        Height = 13
        Caption = 'Duration'
      end
      object trialLabel: TLabel
        Left = 16
        Top = 221
        Width = 295
        Height = 16
        Caption = 'This is trial version, limited by running time or logo.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGray
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        Visible = False
      end
      object Label4: TLabel
        Left = 16
        Top = 121
        Width = 41
        Height = 13
        Caption = 'Rotation'
      end
      object Label5: TLabel
        Left = 16
        Top = 65
        Width = 80
        Height = 13
        Caption = 'Full screen mode'
      end
      object ScreenResolutionCombo: TComboBox
        Left = 16
        Top = 27
        Width = 128
        Height = 21
        Style = csDropDownList
        DropDownCount = 16
        ItemHeight = 13
        ItemIndex = 0
        TabOrder = 0
        Text = '640 x 360'
        Items.Strings = (
          '640 x 360'
          '640 x 480'
          '800 x 450'
          '800 x 600'
          '1024 x 576'
          '1024 x 600'
          '1024 x 640'
          '1024 x 768'
          '1024 x 819'
          '1093 x 614'
          '1152 x 720'
          '1152 x 864'
          '1280 x 720'
          '1280 x 768'
          '1280 x 800'
          '1280 x 960'
          '1280 x 1024'
          '1344 x 840'
          '1360 x 768'
          '1366 x 768'
          '1400 x 1050'
          '1440 x 900'
          '1600 x 900'
          '1600 x 1200'
          '1680 x 1050'
          '1920 x 1080'
          '1920 x 1200'
          '2560 x 1440'
          '2560 x 1600')
      end
      object DurationCombo: TComboBox
        Left = 16
        Top = 195
        Width = 128
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        ItemIndex = 1
        TabOrder = 4
        Text = '30 Sec'
        Items.Strings = (
          '15 Sec'
          '30 Sec'
          '45 Sec'
          '1 Min'
          '1 Min 30 Sec'
          '2 Min'
          '3 Min'
          '5 Min'
          '10 Min'
          '15 Min'
          'Don'#39't stop')
      end
      object RotationCombo: TComboBox
        Left = 16
        Top = 139
        Width = 128
        Height = 21
        Style = csDropDownList
        DropDownCount = 16
        ItemHeight = 13
        ItemIndex = 0
        TabOrder = 2
        Text = 'No'
        Items.Strings = (
          'No'
          'Right'
          'Down'
          'Left')
      end
      object ScreenMirror: TCheckBox
        Left = 176
        Top = 140
        Width = 97
        Height = 17
        Caption = 'Mirror'
        TabOrder = 3
      end
      object ScreenFullscreenCombo: TComboBox
        Left = 16
        Top = 80
        Width = 128
        Height = 21
        Style = csDropDownList
        DropDownCount = 16
        ItemHeight = 13
        ItemIndex = 0
        TabOrder = 1
        Text = 'No fullscreen'
        Items.Strings = (
          'No fullscreen'
          'Fullscreen'
          'Fullscreen 2')
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Camera'
      ImageIndex = 1
      object Label1: TLabel
        Left = 16
        Top = 8
        Width = 32
        Height = 13
        Caption = 'Device'
      end
      object CameraDeviceCombo: TComboBox
        Left = 16
        Top = 27
        Width = 80
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        ItemIndex = 0
        TabOrder = 0
        Text = '0'
        Items.Strings = (
          '0'
          '1'
          '2'
          '3'
          '4'
          '5'
          '6'
          '7'
          '8'
          '9')
      end
      object CameraTypeUSBRadio: TRadioButton
        Left = 16
        Top = 80
        Width = 113
        Height = 17
        Caption = 'USB camera'
        Checked = True
        TabOrder = 1
        TabStop = True
      end
      object CameraTypePS3Radio: TRadioButton
        Left = 16
        Top = 103
        Width = 113
        Height = 17
        Caption = ' PS3 Eye Camera'
        TabOrder = 2
      end
      object CameraTypeKinectRadio: TRadioButton
        Left = 16
        Top = 126
        Width = 113
        Height = 17
        Caption = 'Kinect'
        TabOrder = 3
      end
    end
  end
  object Button1: TButton
    Left = 351
    Top = 20
    Width = 73
    Height = 33
    Caption = 'Run'
    TabOrder = 1
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 354
    Top = 198
    Width = 73
    Height = 33
    Caption = 'Revert'
    TabOrder = 3
    Visible = False
  end
  object SaveButton: TButton
    Left = 354
    Top = 248
    Width = 73
    Height = 20
    Caption = 'Save'
    TabOrder = 4
    Visible = False
    OnClick = SaveButtonClick
  end
  object BuyButton: TButton
    Left = 351
    Top = 64
    Width = 73
    Height = 33
    Caption = 'Buy'
    TabOrder = 2
    Visible = False
    OnClick = BuyButtonClick
  end
  object MainMenu1: TMainMenu
    Left = 216
    Top = 8
    object N1: TMenuItem
      Caption = 'Help'
      object GettingStartedGuide1: TMenuItem
        Caption = 'Getting Started Guide'
        OnClick = GettingStartedGuide1Click
      end
      object miOnlineHelp: TMenuItem
        Caption = 'Online Help'
        OnClick = miOnlineHelpClick
      end
      object N2: TMenuItem
        Caption = '-'
      end
      object About1: TMenuItem
        Caption = 'About'
        OnClick = About1Click
      end
    end
  end
  object XPManifest1: TXPManifest
    Left = 216
    Top = 48
  end
end
