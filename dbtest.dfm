object Form4: TForm4
  Left = 220
  Top = 218
  Width = 890
  Height = 661
  Caption = 'Form4'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 408
    Top = 568
    Width = 75
    Height = 25
    Caption = 'Button1'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Memo1: TMemo
    Left = 8
    Top = 8
    Width = 841
    Height = 537
    Lines.Strings = (
      'Memo1')
    ScrollBars = ssBoth
    TabOrder = 1
    WordWrap = False
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 608
    Width = 882
    Height = 19
    Panels = <>
    SimplePanel = False
  end
  object BDEClientDataSet1: TBDEClientDataSet
    Aggregates = <>
    Options = [poAllowCommandText]
    Params = <>
    Left = 152
    Top = 96
  end
  object Table1: TTable
    DatabaseName = 'TMBILLDB'
    FilterOptions = [foCaseInsensitive]
    SessionName = 'Default'
    FieldDefs = <
      item
        Name = 'line_type'
        Attributes = [faRequired]
        DataType = ftString
        Size = 30
      end
      item
        Name = 'line_num'
        Attributes = [faRequired]
        DataType = ftString
        Size = 30
      end
      item
        Name = 'call_type'
        Attributes = [faRequired]
        DataType = ftString
        Size = 30
      end
      item
        Name = 'call_date'
        DataType = ftDate
      end
      item
        Name = 'call_time'
        DataType = ftTime
      end
      item
        Name = 'call_num'
        Attributes = [faRequired]
        DataType = ftString
        Size = 30
      end
      item
        Name = 'call_area'
        Attributes = [faRequired]
        Size = 30
      end
      item
        Name = 'call_duration'
        Attributes = [faRequired]
        DataType = ftInteger
      end
      item
        Name = 'call_charge'
        Attributes = [faRequired]
        DataType = ftCurrency
      end>
    IndexFiles.Strings = (
      'call_detail')
    StoreDefs = True
    TableName = 'call_detail'
    TableType = ttDBase
    Left = 272
    Top = 552
  end
  object Query1: TQuery
    DatabaseName = 'dBASE Files'
    Left = 48
    Top = 552
  end
  object OpenDialog1: TOpenDialog
    Left = 648
    Top = 296
  end
  object Database1: TDatabase
    SessionName = 'Default'
    Left = 240
    Top = 408
  end
end
