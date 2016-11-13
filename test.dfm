object Form1: TForm1
  Left = 309
  Top = 236
  Width = 870
  Height = 640
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 472
    Top = 456
    Width = 32
    Height = 13
    Caption = 'Label1'
  end
  object Label2: TLabel
    Left = 760
    Top = 128
    Width = 32
    Height = 13
    Caption = 'Label2'
  end
  object TrackBar1: TTrackBar
    Left = 272
    Top = 448
    Width = 185
    Height = 33
    Max = 50
    Orientation = trHorizontal
    Frequency = 5
    Position = 0
    SelEnd = 0
    SelStart = 0
    TabOrder = 16
    TickMarks = tmBottomRight
    TickStyle = tsAuto
    OnChange = TrackBar1Change
    OnEnter = TrackBar1Enter
  end
  object Button1: TButton
    Left = 776
    Top = 64
    Width = 75
    Height = 25
    Caption = 'Button1'
    TabOrder = 0
  end
  object Memo1: TMemo
    Left = 24
    Top = 192
    Width = 185
    Height = 249
    Lines.Strings = (
      'Memo1')
    ScrollBars = ssVertical
    TabOrder = 1
  end
  object Button2: TButton
    Left = 784
    Top = 0
    Width = 75
    Height = 25
    Caption = 'Button2'
    TabOrder = 2
  end
  object Button3: TButton
    Left = 72
    Top = 40
    Width = 75
    Height = 25
    Caption = 'Button3'
    TabOrder = 3
    OnClick = Button3Click
  end
  object ProgressBar1: TProgressBar
    Left = 16
    Top = 16
    Width = 201
    Height = 17
    Min = 0
    Max = 100
    Smooth = True
    Step = 1
    TabOrder = 4
  end
  object ProgressBar2: TProgressBar
    Left = 16
    Top = 72
    Width = 201
    Height = 17
    Min = 0
    Max = 100
    Smooth = True
    TabOrder = 5
  end
  object Button4: TButton
    Left = 72
    Top = 96
    Width = 75
    Height = 25
    Caption = 'Button4'
    TabOrder = 6
    OnClick = Button4Click
  end
  object Button5: TButton
    Left = 216
    Top = 312
    Width = 57
    Height = 25
    Caption = 'Button5'
    TabOrder = 7
    OnClick = Button5Click
  end
  object Memo2: TMemo
    Left = 280
    Top = 192
    Width = 185
    Height = 249
    Lines.Strings = (
      'Memo2')
    ScrollBars = ssVertical
    TabOrder = 8
  end
  object Memo3: TMemo
    Left = 544
    Top = 80
    Width = 185
    Height = 241
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clTeal
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Lines.Strings = (
      'Memo3')
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 9
  end
  object Button6: TButton
    Left = 472
    Top = 312
    Width = 57
    Height = 25
    Caption = 'Button6'
    TabOrder = 10
    OnClick = Button6Click
  end
  object Button7: TButton
    Left = 304
    Top = 40
    Width = 75
    Height = 25
    Caption = 'Button7'
    TabOrder = 11
    OnClick = Button7Click
  end
  object Button8: TButton
    Left = 304
    Top = 96
    Width = 75
    Height = 25
    Caption = 'Button8'
    TabOrder = 12
    OnClick = Button8Click
  end
  object ProgressBar3: TProgressBar
    Left = 232
    Top = 16
    Width = 201
    Height = 17
    Min = 0
    Max = 100
    Smooth = True
    TabOrder = 13
  end
  object ProgressBar4: TProgressBar
    Left = 232
    Top = 72
    Width = 201
    Height = 17
    Min = 0
    Max = 100
    TabOrder = 14
  end
  object Memo4: TMemo
    Left = 544
    Top = 328
    Width = 185
    Height = 241
    Lines.Strings = (
      'Memo4')
    ScrollBars = ssVertical
    TabOrder = 15
  end
  object GroupBox1: TGroupBox
    Left = 288
    Top = 488
    Width = 177
    Height = 41
    Caption = 'GroupBox1'
    TabOrder = 17
    object RadioButton1: TRadioButton
      Left = 8
      Top = 16
      Width = 73
      Height = 17
      Caption = 'random'
      TabOrder = 0
    end
    object RadioButton2: TRadioButton
      Left = 96
      Top = 16
      Width = 73
      Height = 17
      Caption = 'sequential'
      Checked = True
      TabOrder = 1
      TabStop = True
    end
  end
  object Button9: TButton
    Left = 536
    Top = 32
    Width = 33
    Height = 25
    Caption = 'Button9'
    TabOrder = 18
    OnClick = Button9Click
  end
  object Button10: TButton
    Left = 216
    Top = 136
    Width = 75
    Height = 25
    Caption = 'Button10'
    TabOrder = 19
    OnClick = Button10Click
  end
  object OpenDialog1: TOpenDialog
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Left = 248
    Top = 40
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = '*.bz2'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofPathMustExist, ofEnableSizing]
    Left = 168
    Top = 40
  end
end
