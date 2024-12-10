object srxDesignerForm: TsrxDesignerForm
  Left = 118
  Top = 281
  Width = 804
  Height = 569
  Caption = 'SRX Report Designer'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefaultPosOnly
  OnCloseQuery = FormCloseQuery
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label18: TLabel
    Left = 368
    Top = 264
    Width = 38
    Height = 13
    Caption = 'Label18'
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 796
    Height = 81
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    object Label3: TLabel
      Left = 8
      Top = 56
      Width = 49
      Height = 13
      Caption = 'Database:'
    end
    object Label2: TLabel
      Left = 8
      Top = 32
      Width = 46
      Height = 13
      Caption = 'Password'
    end
    object Label1: TLabel
      Left = 8
      Top = 8
      Width = 51
      Height = 13
      Caption = 'Username:'
    end
    object EditUserName: TEdit
      Left = 64
      Top = 8
      Width = 729
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
      OnChange = editChange
    end
    object EditPassword: TEdit
      Left = 64
      Top = 32
      Width = 729
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      PasswordChar = '*'
      TabOrder = 1
      OnChange = editChange
    end
    object ComboBoxDatabase: TComboBox
      Left = 64
      Top = 56
      Width = 729
      Height = 21
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      ItemHeight = 13
      Sorted = True
      TabOrder = 2
      OnChange = DatabaseChane
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 501
    Width = 796
    Height = 41
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object ButtonLoad: TButton
      Left = 88
      Top = 8
      Width = 75
      Height = 25
      Caption = 'Load...'
      TabOrder = 0
      OnClick = ButtonLoadClick
    end
    object ButtonSaveAs: TButton
      Left = 248
      Top = 8
      Width = 75
      Height = 25
      Caption = 'Save As...'
      TabOrder = 1
      OnClick = ButtonSaveAsClick
    end
    object ButtonTest: TButton
      Left = 712
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Test...'
      TabOrder = 2
      OnClick = ButtonTestClick
    end
    object ButtonSave: TButton
      Left = 168
      Top = 8
      Width = 75
      Height = 25
      Caption = 'Save'
      TabOrder = 3
      OnClick = ButtonSaveClick
    end
    object ButtonNew: TButton
      Left = 8
      Top = 8
      Width = 75
      Height = 25
      Caption = 'New'
      TabOrder = 4
      OnClick = ButtonNewClick
    end
  end
  object PageControl: TPageControl
    Left = 0
    Top = 81
    Width = 796
    Height = 420
    ActivePage = TabSheetSQL
    Align = alClient
    TabOrder = 2
    object TabSheetReport: TTabSheet
      Caption = 'Report'
      object Label7: TLabel
        Left = 8
        Top = 8
        Width = 58
        Height = 13
        Caption = 'Report Title:'
      end
      object Label8: TLabel
        Left = 8
        Top = 40
        Width = 52
        Height = 13
        Caption = 'Table Tag:'
      end
      object Label9: TLabel
        Left = 8
        Top = 72
        Width = 60
        Height = 13
        Caption = 'Record Tag:'
      end
      object EditReportTitle: TEdit
        Left = 80
        Top = 8
        Width = 698
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 0
        OnChange = editChange
      end
      object EditTableTag: TEdit
        Left = 80
        Top = 40
        Width = 698
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 1
        OnChange = editChange
      end
      object EditRecordTag: TEdit
        Left = 80
        Top = 72
        Width = 698
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 2
        OnChange = editChange
      end
    end
    object TabSheetSQL: TTabSheet
      Caption = 'SQL Command'
      object MemoSql: TMemo
        Left = 0
        Top = 0
        Width = 355
        Height = 392
        Align = alClient
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'System'
        Font.Pitch = fpFixed
        Font.Style = []
        ParentFont = False
        ScrollBars = ssBoth
        TabOrder = 0
        WantTabs = True
        WordWrap = False
        OnChange = editChange
      end
      object ListBoxFields: TListBox
        Left = 632
        Top = 0
        Width = 156
        Height = 392
        Align = alRight
        ItemHeight = 13
        Sorted = True
        TabOrder = 3
        OnDblClick = ListBoxTablesDblClick
      end
      object ListBoxTables: TListBox
        Left = 476
        Top = 0
        Width = 156
        Height = 392
        Align = alRight
        ItemHeight = 13
        Sorted = True
        TabOrder = 2
        OnClick = ListBoxTablesClick
        OnDblClick = ListBoxTablesDblClick
      end
      object ListBoxKeywords: TListBox
        Left = 355
        Top = 0
        Width = 121
        Height = 392
        Align = alRight
        ItemHeight = 13
        Items.Strings = (
          'select '
          'distinct '
          'from '
          'left join '
          'right join '
          'inner join '
          'full outer join '
          'on '
          'where '
          'group by '
          'having '
          'order by '
          'desc ')
        TabOrder = 1
        OnDblClick = ListBoxTablesDblClick
      end
    end
    object TabSheetParameter: TTabSheet
      Caption = 'Parameter'
      ImageIndex = 1
      OnHide = TabSheetParameterHide
      OnShow = TabSheetParameterShow
      object Label4: TLabel
        Left = 184
        Top = 40
        Width = 27
        Height = 13
        Caption = 'Type:'
      end
      object Label5: TLabel
        Left = 184
        Top = 72
        Width = 67
        Height = 13
        Caption = 'Default Value:'
      end
      object Label6: TLabel
        Left = 184
        Top = 8
        Width = 29
        Height = 13
        Caption = 'Label:'
      end
      object Label26: TLabel
        Left = 184
        Top = 112
        Width = 31
        Height = 13
        Caption = 'Query:'
      end
      object ListBoxParameter: TListBox
        Left = 0
        Top = 0
        Width = 169
        Height = 392
        Align = alLeft
        ItemHeight = 13
        TabOrder = 0
        OnClick = ListBoxParameterClick
      end
      object ComboBoxParamType: TComboBox
        Left = 280
        Top = 40
        Width = 501
        Height = 21
        Style = csDropDownList
        Anchors = [akLeft, akTop, akRight]
        Enabled = False
        ItemHeight = 13
        TabOrder = 2
        Items.Strings = (
          'int'
          'float'
          'time'
          'date'
          'date_time'
          'string'
          'query')
      end
      object EditParameterValue: TEdit
        Left = 280
        Top = 72
        Width = 501
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        Enabled = False
        TabOrder = 3
      end
      object EditLabel: TEdit
        Left = 279
        Top = 8
        Width = 501
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        Enabled = False
        TabOrder = 1
      end
      object MemoParameterQuery: TMemo
        Left = 280
        Top = 112
        Width = 501
        Height = 273
        Anchors = [akLeft, akTop, akRight, akBottom]
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'System'
        Font.Pitch = fpFixed
        Font.Style = []
        ParentFont = False
        TabOrder = 4
        WordWrap = False
      end
    end
    object TabSheetChart: TTabSheet
      Caption = 'Chart'
      ImageIndex = 3
      object PageControl2: TPageControl
        Left = 0
        Top = 0
        Width = 788
        Height = 392
        ActivePage = TabSheetChartInner
        Align = alClient
        TabOrder = 0
        object TabSheetChartInner: TTabSheet
          Caption = 'Chart'
          object Label10: TLabel
            Left = 8
            Top = 8
            Width = 23
            Height = 13
            Caption = 'Title:'
          end
          object Label11: TLabel
            Left = 8
            Top = 32
            Width = 39
            Height = 13
            Caption = 'Legend:'
          end
          object Label12: TLabel
            Left = 8
            Top = 56
            Width = 64
            Height = 13
            Caption = 'Default Type:'
          end
          object Label13: TLabel
            Left = 8
            Top = 88
            Width = 50
            Height = 13
            Caption = 'Axis Titles:'
          end
          object Label14: TLabel
            Left = 8
            Top = 184
            Width = 22
            Height = 13
            Caption = 'Grid:'
          end
          object EditChartTitle: TEdit
            Left = 88
            Top = 8
            Width = 297
            Height = 21
            TabOrder = 0
            OnChange = editChange
          end
          object CheckBoxLegend: TCheckBox
            Left = 88
            Top = 32
            Width = 97
            Height = 17
            Caption = 'Show Legend'
            TabOrder = 1
            OnClick = editChange
          end
          object ComboBoxType: TComboBox
            Left = 88
            Top = 56
            Width = 145
            Height = 21
            Style = csDropDownList
            ItemHeight = 13
            TabOrder = 2
            OnChange = ChangeDefaultType
            Items.Strings = (
              ''
              'line'
              'area'
              'pie'
              'bar')
          end
          object ComboBoxSubtype: TComboBox
            Left = 240
            Top = 56
            Width = 145
            Height = 21
            Style = csDropDownList
            ItemHeight = 13
            TabOrder = 3
            OnChange = editChange
            Items.Strings = (
              ''
              'normal'
              'side'
              'stacked'
              'stacked100')
          end
          object EditTopAxisTitle: TEdit
            Left = 136
            Top = 104
            Width = 121
            Height = 21
            TabOrder = 4
            OnChange = editChange
          end
          object EditLeftAxisTitle: TEdit
            Left = 8
            Top = 128
            Width = 121
            Height = 21
            TabOrder = 5
            OnChange = editChange
          end
          object EditBottomAxisTitle: TEdit
            Left = 136
            Top = 152
            Width = 121
            Height = 21
            TabOrder = 6
            OnChange = editChange
          end
          object EditRightAxisTitle: TEdit
            Left = 264
            Top = 128
            Width = 121
            Height = 21
            TabOrder = 7
            OnChange = editChange
          end
          object CheckBoxTopGrid: TCheckBox
            Left = 136
            Top = 200
            Width = 97
            Height = 17
            Caption = 'Top'
            TabOrder = 8
            OnClick = editChange
          end
          object CheckBoxRightGrid: TCheckBox
            Left = 264
            Top = 224
            Width = 97
            Height = 17
            Caption = 'Right'
            TabOrder = 9
            OnClick = editChange
          end
          object CheckBoxBottomGrid: TCheckBox
            Left = 136
            Top = 248
            Width = 97
            Height = 17
            Caption = 'Bottom'
            TabOrder = 10
            OnClick = editChange
          end
          object CheckBoxLeftGrid: TCheckBox
            Left = 8
            Top = 224
            Width = 97
            Height = 17
            Caption = 'Left'
            TabOrder = 11
            OnClick = editChange
          end
        end
        object TabSheetChartData: TTabSheet
          Caption = 'Data'
          ImageIndex = 1
          object Label15: TLabel
            Left = 288
            Top = 64
            Width = 54
            Height = 13
            Caption = 'Label Field:'
          end
          object Label16: TLabel
            Left = 288
            Top = 40
            Width = 55
            Height = 13
            Caption = 'Value Field:'
          end
          object Label17: TLabel
            Left = 288
            Top = 112
            Width = 27
            Height = 13
            Caption = 'Type:'
          end
          object Label19: TLabel
            Left = 288
            Top = 136
            Width = 27
            Height = 13
            Caption = 'Color:'
          end
          object Label20: TLabel
            Left = 288
            Top = 88
            Width = 23
            Height = 13
            Caption = 'Title:'
          end
          object Label21: TLabel
            Left = 288
            Top = 160
            Width = 92
            Height = 13
            Caption = 'Vertical/Horiz. Axis:'
          end
          object Label22: TLabel
            Left = 288
            Top = 184
            Width = 39
            Height = 13
            Caption = 'Legend:'
          end
          object Label23: TLabel
            Left = 288
            Top = 208
            Width = 41
            Height = 13
            Caption = 'Explode:'
          end
          object Label24: TLabel
            Left = 288
            Top = 232
            Width = 34
            Height = 13
            Caption = 'Others:'
          end
          object Label25: TLabel
            Left = 288
            Top = 256
            Width = 27
            Height = 13
            Caption = 'Mark:'
          end
          object ListBoxChartData: TListBox
            Left = 0
            Top = 0
            Width = 273
            Height = 364
            Align = alLeft
            ItemHeight = 13
            TabOrder = 0
            OnClick = ListBoxChartDataClick
          end
          object EditLabelField: TEdit
            Left = 392
            Top = 64
            Width = 145
            Height = 21
            TabOrder = 4
            OnChange = UpdateChartData
          end
          object EditValueField: TEdit
            Left = 392
            Top = 40
            Width = 145
            Height = 21
            TabOrder = 3
            OnChange = UpdateChartData
          end
          object ComboBoxChartDataType: TComboBox
            Left = 392
            Top = 112
            Width = 145
            Height = 21
            Style = csDropDownList
            ItemHeight = 13
            TabOrder = 6
            OnChange = UpdateChartData
            Items.Strings = (
              ''
              'line'
              'area'
              'pie'
              'bar')
          end
          object ComboBoxChartDataSubtype: TComboBox
            Left = 544
            Top = 112
            Width = 145
            Height = 21
            Style = csDropDownList
            ItemHeight = 13
            TabOrder = 7
            OnChange = UpdateChartData
            Items.Strings = (
              ''
              'normal'
              'side'
              'stacked'
              'stacked100')
          end
          object EditChartDataColor: TEdit
            Left = 392
            Top = 136
            Width = 145
            Height = 21
            TabOrder = 8
            OnChange = UpdateChartData
          end
          object ButtonSelectColor: TButton
            Left = 544
            Top = 136
            Width = 75
            Height = 21
            Caption = 'Select...'
            TabOrder = 9
            OnClick = ButtonSelectColorClick
          end
          object EditDataChartTitle: TEdit
            Left = 392
            Top = 88
            Width = 145
            Height = 21
            TabOrder = 5
            OnChange = UpdateChartData
          end
          object ComboBoxVerticalAxis: TComboBox
            Left = 392
            Top = 160
            Width = 145
            Height = 21
            Style = csDropDownList
            ItemHeight = 13
            TabOrder = 10
            OnChange = UpdateChartData
            Items.Strings = (
              ''
              'left'
              'right'
              'both')
          end
          object ComboBoxHorizAxis: TComboBox
            Left = 544
            Top = 160
            Width = 145
            Height = 21
            Style = csDropDownList
            ItemHeight = 13
            TabOrder = 11
            OnChange = UpdateChartData
            Items.Strings = (
              ''
              'top'
              'bottom'
              'both')
          end
          object CheckBoxChartDataLegend: TCheckBox
            Left = 392
            Top = 184
            Width = 97
            Height = 17
            Caption = 'Show Legend'
            TabOrder = 12
            OnClick = UpdateChartData
          end
          object EditExplode: TEdit
            Left = 392
            Top = 208
            Width = 145
            Height = 21
            TabOrder = 13
            OnChange = UpdateChartData
          end
          object EditOther: TEdit
            Left = 392
            Top = 232
            Width = 145
            Height = 21
            TabOrder = 14
            OnChange = UpdateChartData
          end
          object ComboBoxMarks: TComboBox
            Left = 392
            Top = 256
            Width = 145
            Height = 21
            Style = csDropDownList
            ItemHeight = 13
            TabOrder = 15
            OnChange = UpdateChartData
            Items.Strings = (
              ''
              'hidden'
              'value'
              'percent'
              'label'
              'labelPercent'
              'labelValue'
              'percentTotal'
              'labelPercentTotal')
          end
          object ButtonNewChartData: TButton
            Left = 288
            Top = 8
            Width = 75
            Height = 25
            Caption = 'New'
            TabOrder = 1
            OnClick = ButtonNewChartDataClick
          end
          object ButtonDeleteChartData: TButton
            Left = 368
            Top = 8
            Width = 75
            Height = 25
            Caption = 'Delete'
            TabOrder = 2
            OnClick = ButtonDeleteChartDataClick
          end
          object ButtonUp: TButton
            Left = 448
            Top = 8
            Width = 75
            Height = 25
            Caption = 'Up'
            TabOrder = 16
            OnClick = ButtonUpClick
          end
          object ButtonDown: TButton
            Left = 528
            Top = 8
            Width = 75
            Height = 25
            Caption = 'Down'
            TabOrder = 17
            OnClick = ButtonDownClick
          end
        end
      end
    end
  end
  object SaveDialog: TSaveDialog
    DefaultExt = 'SRXD'
    Filter = 'SRX Designer (*.srxd)|*.srxd|SRX Viewer (*.srx)|*.srx'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Title = 'Save SRX Report'
    Left = 80
    Top = 456
  end
  object OpenDialog: TOpenDialog
    Filter = 'SRX Reports (*.srxd, *.srx)|*.srxd;*.srx'
    Title = 'Load SRX Report'
    Left = 16
    Top = 456
  end
  object ColorDialog: TColorDialog
    Ctl3D = True
    Options = [cdFullOpen, cdAnyColor]
    Left = 152
    Top = 457
  end
end
