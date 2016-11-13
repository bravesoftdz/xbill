object Form3: TForm3
  Left = 511
  Top = 183
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'TM eBill Encoder v1.0.0'
  ClientHeight = 369
  ClientWidth = 501
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 352
    Top = 14
    Width = 32
    Height = 13
    Caption = 'Label1'
  end
  object Label3: TLabel
    Left = 240
    Top = 56
    Width = 100
    Height = 13
    Caption = 'Files to be processed'
  end
  object Label5: TLabel
    Left = 296
    Top = 24
    Width = 128
    Height = 13
    Caption = 'List of files to be processed'
  end
  object Label7: TLabel
    Left = 96
    Top = 336
    Width = 32
    Height = 13
    Caption = 'Label7'
  end
  object Label8: TLabel
    Left = 16
    Top = 316
    Width = 75
    Height = 13
    Caption = 'Company Name'
  end
  object Button1: TButton
    Left = 416
    Top = 310
    Width = 75
    Height = 25
    Caption = '&Encode'
    TabOrder = 0
    OnClick = Button1Click
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 350
    Width = 501
    Height = 19
    Panels = <
      item
        Width = 340
      end
      item
        Style = psOwnerDraw
        Width = 75
      end
      item
        Style = psOwnerDraw
        Width = 75
      end>
    SimplePanel = False
    OnDrawPanel = StatusBar1DrawPanel
  end
  object ProgressBar1: TProgressBar
    Left = 136
    Top = 40
    Width = 209
    Height = 17
    Min = 0
    Max = 100
    Smooth = True
    TabOrder = 2
  end
  object ProgressBar2: TProgressBar
    Left = 224
    Top = 16
    Width = 150
    Height = 17
    Min = 0
    Max = 100
    Smooth = True
    TabOrder = 3
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 489
    Height = 297
    TabOrder = 4
    object Label2: TLabel
      Left = 8
      Top = 270
      Width = 76
      Height = 13
      Caption = 'Target Directory'
    end
    object Label4: TLabel
      Left = 16
      Top = 16
      Width = 128
      Height = 13
      Caption = 'List of files to be processed'
    end
    object Label6: TLabel
      Left = 256
      Top = 16
      Width = 96
      Height = 13
      Caption = 'List of processed file'
    end
    object ListBox1: TListBox
      Left = 8
      Top = 32
      Width = 235
      Height = 225
      ItemHeight = 13
      Items.Strings = (
        'E:\testzone\tmbilldb\shorty2.file'
        'E:\testzone\tmbilldb\ujibakat.file')
      PopupMenu = PopupMenu1
      TabOrder = 0
    end
    object Edit2: TEdit
      Left = 88
      Top = 266
      Width = 361
      Height = 21
      TabOrder = 1
      Text = 'e:\testzone'
    end
    object Button3: TButton
      Left = 456
      Top = 264
      Width = 25
      Height = 25
      Caption = 'Button2'
      TabOrder = 2
      OnClick = Button3Click
    end
  end
  object ListBox2: TListBox
    Left = 256
    Top = 40
    Width = 235
    Height = 225
    ItemHeight = 13
    TabOrder = 5
  end
  object Edit1: TEdit
    Left = 96
    Top = 312
    Width = 313
    Height = 21
    TabOrder = 6
    Text = 'Dell'
  end
  object OpenDialog1: TOpenDialog
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Left = 256
    Top = 104
  end
  object MainMenu1: TMainMenu
    Left = 176
    Top = 144
    object File1: TMenuItem
      Caption = '&File'
      object Add1: TMenuItem
        Caption = '&Add'
        OnClick = Add3Click
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object Exit1: TMenuItem
        Caption = '&Exit'
        OnClick = Exit1Click
      end
    end
    object About1: TMenuItem
      Caption = '&About'
      OnClick = About1Click
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 344
    Top = 136
    object Add3: TMenuItem
      Caption = '&Add'
      OnClick = Add3Click
    end
    object Add2: TMenuItem
      Caption = '&Remove'
      OnClick = Add2Click
    end
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 88
    Top = 208
  end
end
