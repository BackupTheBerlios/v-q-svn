object EyeBurnerForm: TEyeBurnerForm
  Left = 278
  Top = 250
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'EyeBurner'
  ClientHeight = 213
  ClientWidth = 553
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  WindowState = wsMinimized
  OnCreate = FormCreate
  PixelsPerInch = 120
  TextHeight = 16
  object PageCon: TPageControl
    Left = 0
    Top = 0
    Width = 553
    Height = 213
    ActivePage = OptionsTabSheet
    Align = alClient
    TabIndex = 0
    TabOrder = 0
    object OptionsTabSheet: TTabSheet
      Caption = 'Opcje'
      object Label1: TLabel
        Left = 10
        Top = 10
        Width = 219
        Height = 16
        Caption = 'Czas mi'#281'dzy przerwami (w minutach):'
      end
      object Label2: TLabel
        Left = 10
        Top = 37
        Width = 203
        Height = 16
        Caption = 'Czas trwania przerwy (w minutach):'
      end
      object Label3: TLabel
        Left = 10
        Top = 65
        Width = 228
        Height = 16
        Caption = 'Czas odroczenia przerwy (w minutach):'
      end
      object Label4: TLabel
        Left = 10
        Top = 92
        Width = 171
        Height = 16
        Caption = 'Ilo'#347#263' dozwolonych odrocze'#324':'
      end
      object e_time_sep_cur: TLabel
        Left = 456
        Top = 10
        Width = 33
        Height = 16
        Alignment = taRightJustify
        AutoSize = False
        Caption = 'e_time_sep_cur'
      end
      object e_time_brk_cur: TLabel
        Left = 456
        Top = 37
        Width = 33
        Height = 16
        Alignment = taRightJustify
        AutoSize = False
        Caption = 'e_time_brk_cur'
      end
      object e_time_del_cur: TLabel
        Left = 456
        Top = 65
        Width = 33
        Height = 16
        Alignment = taRightJustify
        AutoSize = False
        Caption = 'e_time_del_cur'
      end
      object e_del_cnt_cur: TLabel
        Left = 456
        Top = 92
        Width = 33
        Height = 16
        Alignment = taRightJustify
        AutoSize = False
        Caption = 'e_del_cnt_cur'
      end
      object e_time_sep_max: TLabel
        Left = 488
        Top = 10
        Width = 37
        Height = 16
        AutoSize = False
        Caption = 'e_time_sep_max'
      end
      object e_time_brk_max: TLabel
        Left = 488
        Top = 37
        Width = 37
        Height = 16
        AutoSize = False
        Caption = 'e_time_brk_max'
      end
      object e_time_del_max: TLabel
        Left = 488
        Top = 65
        Width = 37
        Height = 16
        AutoSize = False
        Caption = 'e_time_del_max'
      end
      object e_del_cnt_max: TLabel
        Left = 488
        Top = 92
        Width = 37
        Height = 16
        AutoSize = False
        Caption = 'e_del_cnt_max'
      end
      object e_time_sep_min: TLabel
        Left = 249
        Top = 8
        Width = 40
        Height = 17
        Alignment = taRightJustify
        AutoSize = False
        Caption = 'e_time_sep_min'
      end
      object e_time_brk_min: TLabel
        Left = 249
        Top = 32
        Width = 40
        Height = 17
        Alignment = taRightJustify
        AutoSize = False
        Caption = 'e_time_brk_min'
      end
      object e_time_del_min: TLabel
        Left = 249
        Top = 64
        Width = 40
        Height = 17
        Alignment = taRightJustify
        AutoSize = False
        Caption = 'e_time_del_min'
      end
      object e_del_cnt_min: TLabel
        Left = 249
        Top = 88
        Width = 40
        Height = 17
        Alignment = taRightJustify
        AutoSize = False
        Caption = 'e_del_cnt_min'
      end
      object AutoChkBox: TCheckBox
        Left = 10
        Top = 118
        Width = 237
        Height = 21
        Caption = 'Uruchamiaj w czasie startu systemu'
        TabOrder = 0
        OnClick = AutoChkBoxClick
      end
      object ConfSaveBut: TButton
        Left = 449
        Top = 148
        Width = 93
        Height = 30
        Caption = 'Zapisz'
        TabOrder = 1
        OnClick = ConfSaveButClick
      end
      object RegClnBut: TButton
        Left = 321
        Top = 148
        Width = 122
        Height = 30
        Caption = 'Wyczy'#347#263' rejestr'
        TabOrder = 2
        OnClick = RegClnButClick
      end
      object e_time_sep: TTrackBar
        Left = 296
        Top = 6
        Width = 153
        Height = 25
        Max = 180
        Min = 5
        Orientation = trHorizontal
        Frequency = 5
        Position = 5
        SelEnd = 0
        SelStart = 0
        TabOrder = 3
        ThumbLength = 15
        TickMarks = tmBottomRight
        TickStyle = tsAuto
        OnChange = e_time_sepChange
      end
      object e_time_brk: TTrackBar
        Left = 296
        Top = 33
        Width = 153
        Height = 25
        Max = 30
        Min = 5
        Orientation = trHorizontal
        Frequency = 1
        Position = 5
        SelEnd = 0
        SelStart = 0
        TabOrder = 4
        ThumbLength = 15
        TickMarks = tmBottomRight
        TickStyle = tsAuto
        OnChange = e_time_brkChange
      end
      object e_time_del: TTrackBar
        Left = 296
        Top = 61
        Width = 153
        Height = 25
        Max = 30
        Min = 1
        Orientation = trHorizontal
        Frequency = 1
        Position = 1
        SelEnd = 0
        SelStart = 0
        TabOrder = 5
        ThumbLength = 15
        TickMarks = tmBottomRight
        TickStyle = tsAuto
        OnChange = e_time_delChange
      end
      object e_del_cnt: TTrackBar
        Left = 296
        Top = 88
        Width = 153
        Height = 25
        Max = 5
        Min = 1
        Orientation = trHorizontal
        Frequency = 1
        Position = 1
        SelEnd = 0
        SelStart = 0
        TabOrder = 6
        ThumbLength = 15
        TickMarks = tmBottomRight
        TickStyle = tsAuto
        OnChange = e_del_cntChange
      end
    end
    object AboutTabSheet: TTabSheet
      Caption = 'O programie'
      ImageIndex = 1
      object Label5: TLabel
        Left = 10
        Top = 10
        Width = 278
        Height = 16
        Caption = 'Program wymuszaj'#261'cy przerwy w czasie pracy.'
      end
      object Label6: TLabel
        Left = 10
        Top = 158
        Width = 34
        Height = 16
        Caption = 'Autor:'
      end
      object Label7: TLabel
        Left = 49
        Top = 158
        Width = 125
        Height = 16
        Cursor = crHandPoint
        Caption = 'Pawe'#322' Niewiadomski'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -15
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        OnClick = Label7Click
      end
      object Label8: TLabel
        Left = 10
        Top = 39
        Width = 271
        Height = 16
        Caption = 'Licencja BSD, '#378'r'#243'd'#322'a dost'#281'pne pod adresem:'
      end
      object Label9: TLabel
        Left = 10
        Top = 59
        Width = 231
        Height = 16
        Cursor = crHandPoint
        Caption = 'http://iidea.pl/~paweln/kody/eb.tar.gz'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -15
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        OnClick = Label9Click
      end
      object Label10: TLabel
        Left = 10
        Top = 79
        Width = 374
        Height = 16
        Caption = #379'adnej gwarancji, zw'#322'aszcza (lecz nie tylko) co do u'#380'yteczno'#347'ci,'
      end
      object Label11: TLabel
        Left = 10
        Top = 98
        Width = 280
        Height = 16
        Caption = 'bezpiecze'#324'stwa oraz dopasowania do potrzeb.'
      end
      object Label12: TLabel
        Left = 10
        Top = 118
        Width = 300
        Height = 16
        Caption = 'U'#380'ywaj'#261'c program zgadasz si'#281' na warunki licencji.'
      end
    end
  end
  object LoopTimer: TTimer
    Interval = 20000
    OnTimer = LoopTimerTimer
    Left = 176
  end
  object TrayIcon: TTrayIcon
    Visible = True
    PopupMenu = TrayPopMenu
    Hide = True
    RestoreOn = imDoubleClick
    PopupMenuOn = imRightClickUp
    OnRestore = TrayIconRestore
    Left = 144
  end
  object TrayPopMenu: TPopupMenu
    Left = 208
    object OpenTrayPopMenu: TMenuItem
      Caption = 'Otw'#243'rz'
      OnClick = OpenTrayPopMenuClick
    end
    object EndTrayPopMenu: TMenuItem
      Caption = 'Koniec'
      OnClick = EndTrayPopMenuClick
    end
  end
end
