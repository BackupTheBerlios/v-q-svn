object SeparatorForm: TSeparatorForm
  Left = 244
  Top = 126
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsDialog
  Caption = 'Przerwa'
  ClientHeight = 127
  ClientWidth = 211
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  PixelsPerInch = 120
  TextHeight = 16
  object Label1: TLabel
    Left = 10
    Top = 10
    Width = 198
    Height = 16
    Caption = 'Min'#261#322' dozwolony czas pracy:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object OkBut: TButton
    Left = 24
    Top = 32
    Width = 161
    Height = 25
    Caption = 'Czas na przerw'#281
    Default = True
    TabOrder = 0
    OnClick = OkButClick
  end
  object DelBut: TButton
    Left = 24
    Top = 64
    Width = 161
    Height = 25
    Caption = 'Przerwa za chwil'#281
    TabOrder = 1
    OnClick = DelButClick
  end
  object NoBut: TButton
    Left = 24
    Top = 96
    Width = 161
    Height = 25
    Caption = 'Tym razem bez przerwy'
    TabOrder = 2
    OnClick = NoButClick
  end
end
