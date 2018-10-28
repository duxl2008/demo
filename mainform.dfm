object MForm: TMForm
  Left = 361
  Top = 138
  BorderStyle = bsDialog
  Caption = 'SM1&SM4'#30828#20214#21152#23494#27979#35797#24179#21488#65288#29256#26435#65306#40657#40857#27743#22823#23398#30005#23376#24037#31243#23398#38498#65292'Email:1996012@hlju.edu.cn'#65289
  ClientHeight = 445
  ClientWidth = 571
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  ShowHint = True
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 15
    Width = 161
    Height = 13
    AutoSize = False
    Caption = #21629#20196
  end
  object Label2: TLabel
    Left = 16
    Top = 59
    Width = 177
    Height = 13
    AutoSize = False
    Caption = #21457#36865#25968#25454
  end
  object Label3: TLabel
    Left = 16
    Top = 212
    Width = 90
    Height = 13
    AutoSize = False
    Caption = #25509#25910#25968#25454'              '
  end
  object SpeedButton1: TSpeedButton
    Left = 248
    Top = 208
    Width = 23
    Height = 22
    Hint = #22797#21046#25968#25454
    Caption = 'C'
    OnClick = SpeedButton1Click
  end
  object Label4: TLabel
    Left = 136
    Top = 60
    Width = 72
    Height = 13
    Caption = #21333#27425#25968#25454#38271#24230
  end
  object Button1: TButton
    Left = 480
    Top = 28
    Width = 75
    Height = 25
    Caption = #25171#24320'USB'
    TabOrder = 1
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 480
    Top = 108
    Width = 75
    Height = 25
    Caption = #21457#36865'&&'#35835#21462
    TabOrder = 2
    OnClick = Button2Click
  end
  object Edit1: TEdit
    Left = 16
    Top = 30
    Width = 441
    Height = 21
    TabOrder = 3
    Text = 'FD 80 00 00 00 00'
  end
  object Button3: TButton
    Left = 480
    Top = 148
    Width = 75
    Height = 25
    Caption = #21152#23494#25991#20214
    TabOrder = 4
    OnClick = Button3Click
  end
  object Button4: TButton
    Left = 480
    Top = 356
    Width = 75
    Height = 25
    Caption = #36864#20986
    Default = True
    TabOrder = 0
    OnClick = Button4Click
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 426
    Width = 571
    Height = 19
    Panels = <
      item
        Width = 50
      end>
    SimplePanel = False
  end
  object Button5: TButton
    Left = 480
    Top = 311
    Width = 75
    Height = 25
    Caption = #25830#38500#31243#24207
    TabOrder = 6
    OnClick = Button5Click
  end
  object Edit2: TRichEdit
    Left = 16
    Top = 80
    Width = 441
    Height = 121
    Font.Charset = GB2312_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = #23435#20307
    Font.Style = []
    Lines.Strings = (
      '11 22 33 44 55 66 77 88 11 22 33 44 55 66 77 88')
    ParentFont = False
    TabOrder = 7
  end
  object Edit3: TRichEdit
    Left = 16
    Top = 232
    Width = 441
    Height = 185
    Font.Charset = GB2312_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = #23435#20307
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 8
  end
  object CheckBox1: TCheckBox
    Left = 126
    Top = 212
    Width = 97
    Height = 17
    Caption = #33258#21160#35835#25968#25454
    Checked = True
    State = cbChecked
    TabOrder = 9
  end
  object Button6: TButton
    Left = 480
    Top = 227
    Width = 75
    Height = 25
    Hint = #35831#23558#36339#32447#30701#36335#65292#37325#26032#36830#25509'USB'
    Caption = #32534#31243
    TabOrder = 10
    OnClick = Button6Click
  end
  object Button7: TButton
    Left = 480
    Top = 68
    Width = 75
    Height = 25
    Caption = #35835#25991#20214
    TabOrder = 11
    OnClick = Button7Click
  end
  object Button8: TButton
    Left = 480
    Top = 188
    Width = 75
    Height = 25
    Caption = #36830#32493#27979#35797
    TabOrder = 12
    OnClick = Button8Click
  end
  object Edit4: TEdit
    Left = 215
    Top = 56
    Width = 37
    Height = 21
    TabOrder = 13
    Text = '4'
  end
  object Button9: TButton
    Left = 411
    Top = 55
    Width = 43
    Height = 21
    Caption = #20851#20110
    TabOrder = 14
    OnClick = Button9Click
  end
  object Button10: TButton
    Left = 288
    Top = 207
    Width = 75
    Height = 25
    Caption = #35835#21462#23494#38053
    TabOrder = 15
    OnClick = Button10Click
  end
  object Button11: TButton
    Left = 480
    Top = 272
    Width = 75
    Height = 22
    Caption = #20462#25913#23494#38053
    TabOrder = 16
    OnClick = Button11Click
  end
  object OpenDialog1: TOpenDialog
    Left = 65528
    Top = 328
  end
end
