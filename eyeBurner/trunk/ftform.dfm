object FirstTimeForm: TFirstTimeForm
  Left = 327
  Top = 165
  BorderStyle = bsDialog
  Caption = 'EyeBurner'
  ClientHeight = 238
  ClientWidth = 394
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 120
  TextHeight = 16
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 290
    Height = 16
    Caption = 'Program ma za zadanie wymusi'#263' robienie przerw'
  end
  object Label2: TLabel
    Left = 8
    Top = 32
    Width = 361
    Height = 16
    Caption = 'w czasie pracy przy komputerze. D'#322'ugo'#347#263' pracy, przerw i inne'
  end
  object Label3: TLabel
    Left = 8
    Top = 56
    Width = 315
    Height = 16
    Caption = 'opcje mo'#380'na ustawi'#263' w panelu konfiguracyjnym, kt'#243'ry'
  end
  object Label4: TLabel
    Left = 8
    Top = 80
    Width = 274
    Height = 16
    Caption = 'przywo'#322'uje si'#281' 2-klikni'#281'ciem niebieskiej ikonki.'
  end
  object Label5: TLabel
    Left = 8
    Top = 112
    Width = 380
    Height = 16
    Caption = 'Zaleca si'#281' robienie co najmniej 5 minutowej przerwy po godzinie'
  end
  object Label6: TLabel
    Left = 8
    Top = 136
    Width = 357
    Height = 16
    Caption = 'pracy przed komputerem. W czasie przerwy najlepiej odej'#347#263
  end
  object Label7: TLabel
    Left = 8
    Top = 160
    Width = 350
    Height = 16
    Caption = 'od komputera. Wykona'#263' r'#243#380'ne '#263'wiczenia: np. nadgarstk'#243'w,'
  end
  object Label8: TLabel
    Left = 8
    Top = 184
    Width = 375
    Height = 16
    Caption = 'oczu (patrzenie na przemian w rzeczy po'#322'o'#380'one blisko i daleko).'
  end
  object CloseBut: TButton
    Left = 312
    Top = 208
    Width = 75
    Height = 25
    Caption = 'Zamknij'
    TabOrder = 0
    OnClick = CloseButClick
  end
end
