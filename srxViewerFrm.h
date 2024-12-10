//---------------------------------------------------------------------------

#ifndef srxViewerFrmH
#define srxViewerFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Db.hpp>
#include <DBGrids.hpp>
#include <DBTables.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
#include <Chart.hpp>
#include <ComCtrls.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <ExtDlgs.hpp>
#include <ADODB.hpp>

#include <gak/string.h>
#include <gak/array.h>
#include <gak/xml.h>

//---------------------------------------------------------------------------
struct ParameterQueryValue
{
	gak::STRING		paramValue;
	gak::STRING		displayValue;
};
class ParameterGui
{
	TLabel							*theLabel;
	TEdit							*theEditControl;
	TDateTimePicker 				*theDatePicker, *theTimePicker;

	TComboBox						*theComboBox;
	gak::Array<ParameterQueryValue>	paramQueryValues;
	gak::STRING						type;

	public:
	ParameterGui()
	{
		theLabel = NULL;
		theEditControl = NULL;
		theDatePicker = theTimePicker = NULL;
		theComboBox = NULL;
	}
	void createGui(
		TScrollBox *scrollBox,
		const gak::STRING &label, const gak::STRING &value,
		const gak::STRING &type, const gak::STRING &query,
		int row
	);
	void moveControl( int width, int pos );
	void setValue( const gak::STRING &newValue );
	gak::STRING getValue( void );
	gak::STRING getType( void ) const
	{
		return type;
	}
};

class ParamEditor : public gak::Array<ParameterGui>
{
	public:
	void setLabelWidth( TScrollBox *scrollBox, int labelWidth );
};

//---------------------------------------------------------------------------
class TsrxViewerForm : public TForm
{
__published:	// Von der IDE verwaltete Komponenten
	TQuery *QueryMain;
	TDataSource *DataSourceMain;
	TDatabase *DatabaseMain;
	TPanel *Panel1;
	TButton *ButtonStart;
	TButton *ButtonCSVexport;
	TButton *ButtonXMLexport;
	TLabel *Label1;
	TLabel *Label2;
	TPageControl *PageControl;
	TTabSheet *TabSheetTable;
	TTabSheet *TabSheetChart;
	TChart *Chart;
	TDBGrid *DBGridMain;
	TPrintDialog *PrintDialog;
	TPopupMenu *PopupMenu;
	TMenuItem *Print;
	TMenuItem *Save;
	TSaveDialog *SaveDialog;
	TScrollBox *ScrollBoxParams;
	TQuery *QueryParam;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ButtonStartClick(TObject *Sender);
	void __fastcall ButtonCSVexportClick(TObject *Sender);
	void __fastcall ButtonXMLexportClick(TObject *Sender);
	void __fastcall DatabaseMainLogin(TDatabase *Database,
		  TStrings *LoginParams);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall QueryMainAfterOpen(TDataSet *DataSet);
	void __fastcall PrintClick(TObject *Sender);
	void __fastcall SaveClick(TObject *Sender);
	void __fastcall DBGridMainDblClick(TObject *Sender);
private:	// Anwender-Deklarationen
	ParamEditor			theParameterEditor;

	void loadReport2( gak::xml::Element *theRoot );
	gak::Array<gak::xml::Any*>	params;
	gak::xml::Any				*theChart;
	gak::STRING					userName, password,
								title, tableName, recordName;
	gak::xml::Element			*theRoot;
	gak::ArrayOfInts			colWidths;
public:		// Anwender-Deklarationen
	__fastcall TsrxViewerForm(TComponent* Owner);
	void loadReport(
		const gak::STRING &userName, const gak::STRING &password,
		const gak::STRING &dbAlias,
		gak::xml::Element *theRoot
	);
};
//---------------------------------------------------------------------------
extern PACKAGE TsrxViewerForm *srxViewerForm;
//---------------------------------------------------------------------------
#endif
