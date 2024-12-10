object srxViewerForm: TsrxViewerForm
  Left = 626
  Top = 247
  Width = 709
  Height = 442
  Caption = 'SRX Report Viewer'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefaultPosOnly
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 64
    Top = 200
    Width = 273
    Height = 32
    Caption = 
      'Please enter the parameter values and click on "Start" to start ' +
      'the query'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold, fsUnderline]
    ParentFont = False
    WordWrap = True
  end
  object Panel1: TPanel
    Left = 0
    Top = 97
    Width = 693
    Height = 41
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    object Label1: TLabel
      Left = 416
      Top = 8
      Width = 258
      Height = 13
      Anchors = [akTop, akRight]
      Caption = 'SRX Viewer 1.2 (c) 2009-2013 by Martin Gäckler - Linz'
    end
    object ButtonStart: TButton
      Left = 8
      Top = 8
      Width = 75
      Height = 25
      Caption = 'Start'
      TabOrder = 0
      OnClick = ButtonStartClick
    end
    object ButtonCSVexport: TButton
      Left = 88
      Top = 8
      Width = 75
      Height = 25
      Caption = 'CSV Export'
      TabOrder = 1
      OnClick = ButtonCSVexportClick
    end
    object ButtonXMLexport: TButton
      Left = 168
      Top = 8
      Width = 75
      Height = 25
      Caption = 'XML Export'
      TabOrder = 2
      OnClick = ButtonXMLexportClick
    end
  end
  object PageControl: TPageControl
    Left = 0
    Top = 138
    Width = 693
    Height = 265
    ActivePage = TabSheetTable
    Align = alClient
    TabOrder = 1
    Visible = False
    object TabSheetTable: TTabSheet
      Caption = 'Table'
      object DBGridMain: TDBGrid
        Left = 0
        Top = 0
        Width = 685
        Height = 237
        Align = alClient
        DataSource = DataSourceMain
        TabOrder = 0
        TitleFont.Charset = DEFAULT_CHARSET
        TitleFont.Color = clWindowText
        TitleFont.Height = -11
        TitleFont.Name = 'MS Sans Serif'
        TitleFont.Style = []
        OnDblClick = DBGridMainDblClick
      end
    end
    object TabSheetChart: TTabSheet
      Caption = 'Chart'
      ImageIndex = 1
      TabVisible = False
      object Chart: TChart
        Left = 0
        Top = 0
        Width = 582
        Height = 226
        BackWall.Brush.Color = clWhite
        BackWall.Brush.Style = bsClear
        Title.Text.Strings = (
          'TChart')
        Align = alClient
        BevelOuter = bvNone
        PopupMenu = PopupMenu
        TabOrder = 0
      end
    end
  end
  object ScrollBoxParams: TScrollBox
    Left = 0
    Top = 0
    Width = 693
    Height = 97
    Align = alTop
    TabOrder = 2
  end
  object DatabaseMain: TDatabase
    DatabaseName = 'theDatabase'
    SessionName = 'Default'
    OnLogin = DatabaseMainLogin
    Left = 24
    Top = 16
  end
  object DataSourceMain: TDataSource
    DataSet = QueryMain
    Left = 184
    Top = 16
  end
  object QueryMain: TQuery
    AfterOpen = QueryMainAfterOpen
    DatabaseName = 'theDatabase'
    Left = 112
    Top = 16
  end
  object PrintDialog: TPrintDialog
    Left = 188
    Top = 193
  end
  object PopupMenu: TPopupMenu
    Left = 36
    Top = 193
    object Print: TMenuItem
      Caption = 'Print...'
      OnClick = PrintClick
    end
    object Save: TMenuItem
      Caption = 'Save...'
      OnClick = SaveClick
    end
  end
  object SaveDialog: TSaveDialog
    Filter = 'Bitmaps (*.bmp)|*.bmp|Metafile (*.wmf)|*.wmf'
    FilterIndex = 2
    Options = [ofOverwritePrompt, ofHideReadOnly, ofPathMustExist, ofCreatePrompt, ofEnableSizing]
    Title = 'Save Chart'
    Left = 124
    Top = 193
  end
  object QueryParam: TQuery
    DatabaseName = 'theDatabase'
    Left = 282
    Top = 18
  end
end
