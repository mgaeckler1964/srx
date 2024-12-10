//---------------------------------------------------------------------------

#ifndef srxDesignerFrmH
#define srxDesignerFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

#include <gak/xml.h>
#include <gak/map.h>
#include <gak/vcl_tools.h>

//---------------------------------------------------------------------------
class ParameterDefinition
{
	public:
	typedef gak::STRING key_type;
	
	gak::STRING	name, label, type, defaultValue, query;
	bool		usageFlag;

	const gak::STRING &getKey( void ) const
	{
		return name;
	}
	void setKey( const gak::STRING &newName )
	{
		this->name = newName;
	}
	ParameterDefinition()
	{
		usageFlag = false;
	}
};
typedef gak::Map<ParameterDefinition>	ParameterDefinitions;

//---------------------------------------------------------------------------
class ChartDataDefinition
{
	public:
	gak::STRING	labelField, valueField, title,
				type, subtype,
				color,
				vaxis, haxis,
				explode, others,
				mark;
	bool		legend;

	ChartDataDefinition()
	{
		legend = false;
	}

};
typedef gak::Array<ChartDataDefinition>	ChartDataDefinitions;

//---------------------------------------------------------------------------
class TsrxDesignerForm : public TForm
{
__published:	// Von der IDE verwaltete Komponenten
	TSaveDialog *SaveDialog;
	TOpenDialog *OpenDialog;
	TPanel *Panel1;
	TEdit *EditUserName;
	TEdit *EditPassword;
	TComboBox *ComboBoxDatabase;
	TLabel *Label3;
	TLabel *Label2;
	TLabel *Label1;
	TPanel *Panel2;
	TButton *ButtonLoad;
	TButton *ButtonSaveAs;
	TButton *ButtonTest;
	TPageControl *PageControl;
	TTabSheet *TabSheetSQL;
	TMemo *MemoSql;
	TTabSheet *TabSheetParameter;
	TListBox *ListBoxParameter;
	TLabel *Label4;
	TComboBox *ComboBoxParamType;
	TLabel *Label5;
	TEdit *EditParameterValue;
	TLabel *Label6;
	TEdit *EditLabel;
	TButton *ButtonSave;
	TButton *ButtonNew;
	TTabSheet *TabSheetReport;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TEdit *EditReportTitle;
	TEdit *EditTableTag;
	TEdit *EditRecordTag;
	TTabSheet *TabSheetChart;
	TPageControl *PageControl2;
	TTabSheet *TabSheetChartInner;
	TLabel *Label10;
	TEdit *EditChartTitle;
	TLabel *Label11;
	TCheckBox *CheckBoxLegend;
	TLabel *Label12;
	TComboBox *ComboBoxType;
	TComboBox *ComboBoxSubtype;
	TLabel *Label13;
	TEdit *EditTopAxisTitle;
	TEdit *EditLeftAxisTitle;
	TEdit *EditBottomAxisTitle;
	TEdit *EditRightAxisTitle;
	TLabel *Label14;
	TCheckBox *CheckBoxTopGrid;
	TCheckBox *CheckBoxRightGrid;
	TCheckBox *CheckBoxBottomGrid;
	TCheckBox *CheckBoxLeftGrid;
	TTabSheet *TabSheetChartData;
	TListBox *ListBoxChartData;
	TLabel *Label15;
	TEdit *EditLabelField;
	TLabel *Label16;
	TEdit *EditValueField;
	TLabel *Label17;
	TComboBox *ComboBoxChartDataType;
	TComboBox *ComboBoxChartDataSubtype;
	TColorDialog *ColorDialog;
	TLabel *Label18;
	TLabel *Label19;
	TEdit *EditChartDataColor;
	TButton *ButtonSelectColor;
	TLabel *Label20;
	TEdit *EditDataChartTitle;
	TLabel *Label21;
	TComboBox *ComboBoxVerticalAxis;
	TComboBox *ComboBoxHorizAxis;
	TCheckBox *CheckBoxChartDataLegend;
	TLabel *Label22;
	TLabel *Label23;
	TEdit *EditExplode;
	TLabel *Label24;
	TEdit *EditOther;
	TLabel *Label25;
	TComboBox *ComboBoxMarks;
	TButton *ButtonNewChartData;
	TButton *ButtonDeleteChartData;
	TButton *ButtonUp;
	TButton *ButtonDown;
	TLabel *Label26;
	TMemo *MemoParameterQuery;
	TListBox *ListBoxFields;
	TListBox *ListBoxTables;
	TListBox *ListBoxKeywords;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ButtonTestClick(TObject *Sender);
	void __fastcall ButtonSaveAsClick(TObject *Sender);
	void __fastcall ButtonLoadClick(TObject *Sender);
	void __fastcall TabSheetParameterShow(TObject *Sender);
	void __fastcall ListBoxParameterClick(TObject *Sender);
	void __fastcall TabSheetParameterHide(TObject *Sender);
	void __fastcall editChange(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall ButtonSaveClick(TObject *Sender);
	void __fastcall ButtonNewClick(TObject *Sender);
	void __fastcall ButtonSelectColorClick(TObject *Sender);
	void __fastcall ButtonNewChartDataClick(TObject *Sender);
	void __fastcall ButtonDeleteChartDataClick(TObject *Sender);
	void __fastcall ListBoxChartDataClick(TObject *Sender);
	void __fastcall ChangeDefaultType(TObject *Sender);
	void __fastcall enableDisableChartTypes(TObject *Sender);
	void __fastcall UpdateChartData(TObject *Sender);
	void __fastcall ButtonUpClick(TObject *Sender);
	void __fastcall ButtonDownClick(TObject *Sender);
	void __fastcall DatabaseChane(TObject *Sender);
	void __fastcall ListBoxTablesClick(TObject *Sender);
	void __fastcall ListBoxTablesDblClick(TObject *Sender);
private:	// Anwender-Deklarationen

	gak::vcl::DBconnector		theConnector;
	gak::vcl::DatabaseSchema	theSchema;

	void loadFields( void );
	void loadSchema( void );

	void __fastcall insertToMemo( const AnsiString &text );

	ParameterDefinitions	parameterDefinitions;
	ChartDataDefinitions	chartDataDefinitions;

	AnsiString	myCaption, FileName;
	int			selectedParamIndex;
	bool		changedFlag, select;

	gak::xml::Any *createXML( void );
	void loadFile( AnsiString FileName );
	void loadXML( gak::xml::Element *theRoot );
	void addParameter( const gak::STRING &paramName );
	void saveParameter( void );
	void clearReport( void );
	void clearParameter( void );
	void clearChart( void );
public:		// Anwender-Deklarationen
	__fastcall TsrxDesignerForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TsrxDesignerForm *srxDesignerForm;
//---------------------------------------------------------------------------
#endif
