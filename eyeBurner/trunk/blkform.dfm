object BlockForm: TBlockForm
  Left = 133
  Top = 121
  BorderStyle = bsNone
  Caption = 'BlockForm'
  ClientHeight = 556
  ClientWidth = 847
  Color = clBlack
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  WindowState = wsMaximized
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnHide = FormHide
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 16
  object InfoLab: TLabel
    Left = 144
    Top = 88
    Width = 663
    Height = 29
    Caption = 'Przerwa! Odejd'#378' od komputera, po'#263'wicz, daj oczom odpocz'#261#263
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindow
    Font.Height = -25
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object InfoTimer: TTimer
    OnTimer = InfoTimerTimer
    Left = 8
    Top = 8
  end
end
