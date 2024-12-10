//---------------------------------------------------------------------------

#include <vcl.h>
#include <DBTables.hpp>

#include <fstream.h>

#include <gak/xmlParser.h>
#include <gak/fmtNumber.h>

#pragma hdrstop

#include "srxDesignerFrm.h"
#include "srxViewerFrm.h"
//---------------------------------------------------------------------------
using namespace gak;
using namespace vcl;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TsrxDesignerForm *srxDesignerForm;
//---------------------------------------------------------------------------
__fastcall TsrxDesignerForm::TsrxDesignerForm(TComponent* Owner)
	: TForm(Owner)
{
	select = changedFlag = false;
	selectedParamIndex = -1;
}
//---------------------------------------------------------------------------
void TsrxDesignerForm::loadFields( void )
{
	ListBoxFields->Items->Clear();
	TableSchema &theTable = theSchema.getTableSchema(
		ListBoxTables->Items->Strings[ListBoxTables->ItemIndex].c_str()
	);

	for( size_t i=0; i<theTable.getNumFields(); i++ )
	{
		ListBoxFields->Items->Add(
			theTable.getField( i )->szName
		);
	}
}

//---------------------------------------------------------------------------
void TsrxDesignerForm::loadSchema( void )
{
	theConnector.m_aliasName = ComboBoxDatabase->Items->Strings[ComboBoxDatabase->ItemIndex].c_str();
	theConnector.m_userName = EditUserName->Text.c_str();
	theConnector.m_password = EditPassword->Text.c_str();

	theSchema.readSchema( theConnector );

	ListBoxTables->Items->Clear();
	for( size_t i=0; i<theSchema.getNumTables(); i++ )
	{
		const TableSchema	&theTable = theSchema.getTableSchema( i );
		ListBoxTables->Items->Add( (const char *)theTable.getKey() );
	}

	theSchema.closeDatabase();
}
//---------------------------------------------------------------------------


void __fastcall TsrxDesignerForm::FormShow(TObject *Sender )
{
	myCaption = Caption;
	TStringList *MyStringList = new TStringList();

	try
	{
		Session->GetAliasNames(MyStringList);
		// fill a list box with alias names for the user to select from
		for( int I = 0; I < MyStringList->Count; I++ )
			ComboBoxDatabase->Items->Add(MyStringList->Strings[I]);

		ComboBoxDatabase->ItemIndex = 0;
		loadSchema();
	}
	__finally
	{
		delete MyStringList;
	}

	if( Application->Tag )
	{
		const char *cmdLine = (const char *)Application->Tag;
		Application->Tag = 0;	// otherwise viewer will load the file, too
		loadFile( cmdLine );
	}
	else
		enableDisableChartTypes( Sender );
}
//---------------------------------------------------------------------------
xml::Any *TsrxDesignerForm::createXML( void )
{
	STRING	sqlText = MemoSql->Text.c_str();

	sqlText = sqlText.stripBlanks();

	xml::Any	*theRoot = new xml::Any( "sql_report" );
	theRoot->setStringAttribute( "title", EditReportTitle->Text.c_str() );
	theRoot->setStringAttribute( "table_name", EditTableTag->Text.c_str() );
	theRoot->setStringAttribute( "record_name", EditRecordTag->Text.c_str() );

	xml::Any	*theQuery = static_cast<xml::Any*>(
		theRoot->addObject( new xml::Any( "query" ) )
	);
	xml::Any	*theSql = static_cast<xml::Any*>(
		theQuery->addObject( new xml::Any( "sql" ) )
	);
	theSql->addObject( new xml::PCData( sqlText ) );

	xml::Any	*params = static_cast<xml::Any*>(
		theQuery->addObject( new xml::Any( "params" ) )
	);

	for(
		ParameterDefinitions::const_iterator it = parameterDefinitions.cbegin(),
			endIT = parameterDefinitions.cend();
		it != endIT;
		++it
	)
	{
		const ParameterDefinition	&paramDef = *it;

		xml::Any *theParameter = static_cast<xml::Any*>(
			params->addObject( new xml::Any( "param" ) )
		);

		theParameter->setStringAttribute( "name", paramDef.name );
		theParameter->setStringAttribute( "label", paramDef.label );
		theParameter->setStringAttribute( "value", paramDef.defaultValue );
		theParameter->setStringAttribute( "type", paramDef.type );
		if( !paramDef.query.isEmpty() )
		{
			xml::Any	*theParamQuery = static_cast<xml::Any*>(
				theParameter->addObject( new xml::Any( "query" ) )
			);
			theParamQuery->addObject( new xml::PCData( paramDef.query ) );
		}
	}

	if( EditChartTitle->Text.Length()
	|| CheckBoxLegend->Checked
	|| ComboBoxType->ItemIndex >= 0
	|| ComboBoxSubtype->ItemIndex >= 0
	|| EditTopAxisTitle->Text.Length()
	|| EditLeftAxisTitle->Text.Length()
	|| EditRightAxisTitle->Text.Length()
	|| EditBottomAxisTitle->Text.Length()
	|| CheckBoxTopGrid->Checked
	|| CheckBoxLeftGrid->Checked
	|| CheckBoxRightGrid->Checked
	|| CheckBoxBottomGrid->Checked
	|| chartDataDefinitions.size() )
	{
		xml::Element *theChart = theRoot->addObject( new xml::Any( "chart" ) );
		theChart->setStringAttribute( "title", EditChartTitle->Text.c_str() );
		theChart->setStringAttribute(
			"legend",
			CheckBoxLegend->Checked ? "yes" : "no"
		);
		if( ComboBoxType->ItemIndex >= 0 )
			theChart->setStringAttribute(
				"type",
				ComboBoxType->Items->Strings[ComboBoxType->ItemIndex].c_str()
			);
		if( ComboBoxSubtype->ItemIndex >= 0 )
			theChart->setStringAttribute(
				"subtype",
				ComboBoxSubtype->Items->Strings[
					ComboBoxSubtype->ItemIndex
				].c_str()
			);
		theChart->setStringAttribute(
			"topAxisTitle",
			EditTopAxisTitle->Text.c_str()
		);
		theChart->setStringAttribute(
			"leftAxisTitle",
			EditLeftAxisTitle->Text.c_str()

		);
		theChart->setStringAttribute(
			"rightAxisTitle",
			EditRightAxisTitle->Text.c_str()
		);
		theChart->setStringAttribute(
			"bottomAxisTitle",
			EditBottomAxisTitle->Text.c_str()
		);
		theChart->setStringAttribute(
			"topAxisGrid",
			CheckBoxTopGrid->Checked ? "yes" : "no"
		);
		theChart->setStringAttribute(
			"leftAxisGrid",
			CheckBoxLeftGrid->Checked ? "yes" : "no"
		);
		theChart->setStringAttribute(
			"rightAxisGrid",
			CheckBoxRightGrid->Checked ? "yes" : "no"
		);
		theChart->setStringAttribute(
			"bottomAxisGrid",
			CheckBoxBottomGrid->Checked ? "yes" : "no"
		);

		for(
			ChartDataDefinitions::const_iterator it = chartDataDefinitions.cbegin(),
				endIT = chartDataDefinitions.cend();
			it != endIT;
			++it
		)
		{
			const ChartDataDefinition &chartData = *it;

			xml::Element *theData = theChart->addObject( new xml::Any( "data" ) );
			theData->setStringAttribute( "label", chartData.labelField );
			theData->setStringAttribute( "value", chartData.valueField );
			theData->setStringAttribute( "title", chartData.title );
			theData->setStringAttribute( "type", chartData.type );
			theData->setStringAttribute( "subtype", chartData.subtype );
			theData->setStringAttribute( "color", chartData.color );
			theData->setStringAttribute( "vaxis", chartData.vaxis );
			theData->setStringAttribute( "haxis", chartData.haxis );
			theData->setStringAttribute( "explode", chartData.explode );
			theData->setStringAttribute( "others", chartData.others );
			theData->setStringAttribute( "mark", chartData.mark );
			theData->setStringAttribute( "legend", chartData.legend ? "yes" : "no" );
		}
	}

	return theRoot;
}
//---------------------------------------------------------------------------
void TsrxDesignerForm::loadXML( xml::Element *theRoot )
{
	ParameterDefinition	newParam;
	ChartDataDefinition	chartData;
	STRING			   	listLabel;

	PageControl->ActivePage = TabSheetReport;

	clearReport();
	clearParameter();
	clearChart();

	STRING	userName = theRoot->getAttribute( "USER_NAME" );
	STRING	password = theRoot->getAttribute( "PASSWORD" );
	STRING	dbAlias = theRoot->getAttribute( "DB_ALIAS" );

	STRING	title = theRoot->getAttribute( "title" );
	STRING	table_name = theRoot->getAttribute( "table_name" );
	STRING	record_name = theRoot->getAttribute( "record_name" );

	EditReportTitle->Text = (const char *)title;
	EditTableTag->Text = (const char *)table_name;
	EditRecordTag->Text = (const char *)record_name;

	EditUserName->Text = (const char *)userName;
	EditPassword->Text = (const char *)password;
	for( int i=0; i<ComboBoxDatabase->Items->Count; i++ )
	{
		if( dbAlias == ComboBoxDatabase->Items->Strings[i].c_str() )
		{
			ComboBoxDatabase->ItemIndex = i;
			loadSchema();
		}
	}
	xml::Element *theQuery = theRoot->getElement( "query" );
	if( theQuery )
	{
		xml::Element *theSql = theQuery->getElement( "sql" );
		if( theSql )
		{
			STRING	sqlText = theSql->getValue( xml::PLAIN_MODE ).stripBlanks();
			setMemoText( MemoSql, sqlText );
		}

		xml::Element	*params = theQuery->getElement( "params" );
		if( params )
		{
			size_t i = 0;
			while( i<params->getNumObjects() )
			{
				xml::Any *theParameter = static_cast<xml::Any*>(
					params->getElement( i )
				);

				if( theParameter && theParameter->getTag() == "param" )
				{
					newParam.label = theParameter->getAttribute( "label" );
					newParam.defaultValue = theParameter->getAttribute( "value" );
					newParam.type = theParameter->getAttribute( "type" );
					newParam.name = theParameter->getAttribute( "name" );

					xml::Element	*theParameterQuery = theParameter->getElement( "query" );
					if( theParameterQuery )
						newParam.query = theParameterQuery->getValue( xml::PLAIN_MODE ).stripBlanks();
					else
						newParam.query = "";

					parameterDefinitions[newParam.name] = newParam;
					ListBoxParameter->Items->Add( (const char*)newParam.name );
				}
				i++;
			}
		}
	}

	xml::Element *theChart = theRoot->getElement( "chart" );
	if( theChart )
	{
		title = theChart->getAttribute( "title" );
		EditChartTitle->Text = (const char*)title;
		STRING	checkBox = theChart->getAttribute( "legend" );
		CheckBoxLegend->Checked = (checkBox == "yes");

		STRING type = theChart->getAttribute( "type" );
		ComboBoxType->ItemIndex = -1;
		for( int i=0; i<ComboBoxType->Items->Count; i++ )
		{
			if( type == ComboBoxType->Items->Strings[i].c_str() )
				ComboBoxType->ItemIndex = i;
		}
		type = theChart->getAttribute( "subtype" );
		ComboBoxSubtype->ItemIndex = -1;
		for( int i=0; i<ComboBoxSubtype->Items->Count; i++ )
		{
			if( type == ComboBoxSubtype->Items->Strings[i].c_str() )
				ComboBoxSubtype->ItemIndex = i;
		}
		title = theChart->getAttribute( "topAxisTitle" );
		EditTopAxisTitle->Text = (const char*)title;
		title = theChart->getAttribute( "leftAxisTitle" );
		EditLeftAxisTitle->Text = (const char*)title;
		title = theChart->getAttribute( "rightAxisTitle" );
		EditRightAxisTitle->Text = (const char*)title;
		title = theChart->getAttribute( "bottomAxisTitle" );
		EditBottomAxisTitle->Text = (const char*)title;

		checkBox = theChart->getAttribute( "topAxisGrid" );
		CheckBoxTopGrid->Checked = (checkBox == "yes");
		checkBox = theChart->getAttribute( "leftAxisGrid" );
		CheckBoxLeftGrid->Checked = (checkBox == "yes");
		checkBox = theChart->getAttribute( "rightAxisGrid" );
		CheckBoxRightGrid->Checked = (checkBox == "yes");
		checkBox = theChart->getAttribute( "bottomAxisGrid" );
		CheckBoxBottomGrid->Checked = (checkBox == "yes");

		for( size_t i=0; i<theChart->getNumObjects(); i++ )
		{
			xml::Element *theData = theChart->getElement(i);
			if( theData && theData->getTag() == "data" )
			{
				chartData.labelField = theData->getAttribute( "label" );
				chartData.valueField = theData->getAttribute( "value" );
				chartData.title = theData->getAttribute( "title" );
				chartData.type = theData->getAttribute( "type" );
				chartData.subtype = theData->getAttribute( "subtype" );
				chartData.color = theData->getAttribute( "color" );
				chartData.vaxis = theData->getAttribute( "vaxis" );
				chartData.haxis = theData->getAttribute( "haxis" );
				chartData.explode = theData->getAttribute( "explode" );
				chartData.others = theData->getAttribute( "others" );
				chartData.mark = theData->getAttribute( "mark" );
				chartData.legend = theData->getAttribute( "legend" ) == "yes";

				chartDataDefinitions.addElement( chartData );

				listLabel = chartData.valueField;
				listLabel += ':';
				listLabel += chartData.labelField;
				listLabel += ':';
				listLabel += chartData.title;
				ListBoxChartData->Items->Add( (const char*)listLabel );
			}
		}
	}

	changedFlag = false;
	selectedParamIndex = -1;
}

//---------------------------------------------------------------------------
void __fastcall TsrxDesignerForm::ButtonTestClick(TObject *)
{
	AnsiString		db = ComboBoxDatabase->Items->Strings[ComboBoxDatabase->ItemIndex].c_str();

	saveParameter();

	STRING	userName = EditUserName->Text.c_str();
	STRING	password = EditPassword->Text.c_str();
	STRING	dbAlias( db.c_str() );

	xml::Any *theRoot = createXML();

	Application->CreateForm(__classid(TsrxViewerForm), &srxViewerForm);
	srxViewerForm->loadReport( userName, password, dbAlias, theRoot );

	delete theRoot;
}
//---------------------------------------------------------------------------
void TsrxDesignerForm::loadFile( AnsiString FileName )
{
	this->FileName = FileName;
	Caption = myCaption + " " + FileName;
	Application->Title = Caption;

	xml::Parser		theParser( FileName.c_str() );
	xml::Document	*theReportDoc = theParser.readFile( false );

	if( theReportDoc )
	{
		xml::Element *theRoot = theReportDoc->getElement( "sql_report" );
		if( theRoot )
			loadXML( theRoot );

		delete theReportDoc;
	}
}
//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::ButtonLoadClick(TObject *Sender)
{
	if( changedFlag )
	{
		int button = Application->MessageBox(
			"Report changed. Do you want to save?",
			"SRX Designer",
			MB_YESNOCANCEL|MB_ICONQUESTION
		);
		if( button == IDYES )
			ButtonSaveClick( Sender );

		if( button == IDCANCEL )
/*@*/		return;
	}

	OpenDialog->FileName = FileName;
	if( OpenDialog->Execute() )
		loadFile( OpenDialog->FileName );

	enableDisableChartTypes( Sender );
}
//---------------------------------------------------------------------------
void TsrxDesignerForm::addParameter( const STRING &paramName )
{
	if( parameterDefinitions.getElementIndex( paramName ) == (size_t)-1 )
	{
		ListBoxParameter->Items->Add( (const char*)paramName );
		ParameterDefinition	paramDef;

		paramDef.usageFlag = true;
		paramDef.name = paramName;
		parameterDefinitions[paramName] = paramDef;
	}
	else
		parameterDefinitions[paramName].usageFlag = true;
}

//---------------------------------------------------------------------------
void __fastcall TsrxDesignerForm::TabSheetParameterShow(TObject *)
{
	bool	inString, inChar, paramFound, eofParam;
	char	c;
	STRING paramName;
	STRING	sqlCommand = MemoSql->Text.c_str();

	EditLabel->Enabled = false;
	ComboBoxParamType->Enabled = false;
	EditParameterValue->Enabled = false;
	MemoParameterQuery->Enabled = false;

	for(
		ParameterDefinitions::iterator it = parameterDefinitions.begin(),
			endIT = parameterDefinitions.end();
		it != endIT;
		++it
	)
	{
		it->usageFlag = false;
	}

	eofParam = paramFound = inString = inChar = false;
	for( std::size_t i=0; (c=sqlCommand[i]) != 0; i++ )
	{
		if( c == '\"' && !inChar )
		{
			inString = !inString;
			eofParam = true;
		}
		else if( c == '\'' && !inString )
		{
			inChar = !inChar;
			eofParam = true;
		}
		else if( isalnum( c ) && paramFound )
			paramName += c;
		else if( c == ':' && !inString && !inChar )
		{
			paramFound = true;
			eofParam = false;
		}
		else
			eofParam = true;

		if( eofParam && !paramName.isEmpty() )
		{
			addParameter( paramName );
			paramName = "";
			paramFound = false;
			eofParam = false;
		}
	}
	if( !paramName.isEmpty() )
		addParameter( paramName );

	ListBoxParameter->ItemIndex = -1;
	selectedParamIndex = -1;

	for( std::size_t i=0; i<parameterDefinitions.size(); )
	{
		if( !parameterDefinitions[i].usageFlag )
		{
			parameterDefinitions.removeElementAt( i );
			ListBoxParameter->Items->Delete( i );
		}
		else
			i++;
	}

}
//---------------------------------------------------------------------------
void TsrxDesignerForm::saveParameter( void )
{
	if( selectedParamIndex >= 0 )
	{
		ParameterDefinition	&paramDef =
			parameterDefinitions.getElementAt(selectedParamIndex);

		STRING newValue = EditLabel->Text.c_str();
		if( paramDef.label != newValue )
		{
			paramDef.label = newValue;
			changedFlag = true;
		}
		newValue =
			ComboBoxParamType->Items->Strings[
				ComboBoxParamType->ItemIndex
			].c_str()
		;
		if( paramDef.type != newValue )
		{
			paramDef.type = newValue;
			changedFlag = true;
		}
		newValue = EditParameterValue->Text.c_str();
		if( paramDef.defaultValue != newValue )
		{
			paramDef.defaultValue = newValue;
			changedFlag = true;
		}

		newValue = MemoParameterQuery->Text.c_str();
		if( paramDef.query != newValue )
		{
			paramDef.query = newValue.stripBlanks();
			changedFlag = true;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TsrxDesignerForm::ListBoxParameterClick(TObject *)
{
	saveParameter();
	if( ListBoxParameter->ItemIndex >= 0 )
	{
		EditLabel->Enabled = true;
		ComboBoxParamType->Enabled = true;
		EditParameterValue->Enabled = true;
		MemoParameterQuery->Enabled = true;

		const ParameterDefinition	&paramDef = parameterDefinitions.getElementAt(
			ListBoxParameter->ItemIndex
		);

		ComboBoxParamType->ItemIndex = 0;
		for( int i=0; i<ComboBoxParamType->Items->Count; i++ )
		{
			if( paramDef.type == ComboBoxParamType->Items->Strings[i].c_str() )
			{
				ComboBoxParamType->ItemIndex = i;
				break;
			}
		}
		EditLabel->Text = (const char *)paramDef.label;
		EditParameterValue->Text = (const char *)paramDef.defaultValue;

		STRING	sqlText = (const char *)paramDef.query;
		setMemoText( MemoParameterQuery, sqlText );
		selectedParamIndex = ListBoxParameter->ItemIndex;
	}
	else
	{
		EditLabel->Enabled = false;
		ComboBoxParamType->Enabled = false;
		EditParameterValue->Enabled = false;
		MemoParameterQuery->Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::TabSheetParameterHide(TObject *)
{
	saveParameter();
}
//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::editChange(TObject *)
{
	changedFlag = true;
}
//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	if( changedFlag )
	{
		int button = Application->MessageBox(
			"Report changed. Do you want to save?",
			"SRX Designer",
			MB_YESNOCANCEL|MB_ICONQUESTION
		);
		if( button == IDYES )
			ButtonSaveClick( Sender );

		if( button == IDCANCEL )
			CanClose = false;
		else
			CanClose = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::ButtonSaveAsClick(TObject *Sender )
{
	saveParameter();

	SaveDialog->FileName = FileName;
	if( SaveDialog->Execute() )
	{
		FileName = SaveDialog->FileName;
		Caption = myCaption + " " + FileName;
		Application->Title = Caption;
		ButtonSaveClick( Sender );
	}

}
//---------------------------------------------------------------------------


void __fastcall TsrxDesignerForm::ButtonSaveClick(TObject *Sender )
{
	if( FileName.Length() == 0 )
	{
		ButtonSaveAsClick( Sender );
	}
	else
	{
		saveParameter();

		std::auto_ptr<xml::Any> theRoot( createXML() );

		AnsiString	extension = FileName.SubString(FileName.Length()-4, 5 );
		if( !extension.AnsiCompareIC( ".SRXD" ) )
		{
			STRING	userName = EditUserName->Text.c_str();
			STRING	password = EditPassword->Text.c_str();
			STRING	dbAlias = ComboBoxDatabase->Text.c_str();

			theRoot->setStringAttribute( "USER_NAME", userName );
			theRoot->setStringAttribute( "PASSWORD", password );
			theRoot->setStringAttribute( "DB_ALIAS", dbAlias );
		}

		STRING			xmlCode = theRoot->generateDoc();
		std::ofstream	fStream( FileName.c_str() );
		if( fStream.is_open() )
		{
			fStream << xmlCode;
		}
		changedFlag = false;
	}
}

//---------------------------------------------------------------------------
void TsrxDesignerForm::clearReport( void )
{
	EditUserName->Text = "";
	EditPassword->Text = "";
	ComboBoxDatabase->ItemIndex = -1;
	EditReportTitle->Text = "";
	EditTableTag->Text = "";
	EditRecordTag->Text = "";
	MemoSql->Text = "";
}

//---------------------------------------------------------------------------
void TsrxDesignerForm::clearParameter( void )
{
	parameterDefinitions.clear();

	ListBoxParameter->Items->Clear();
	selectedParamIndex = -1;
	EditLabel->Text = "";
	ComboBoxParamType->ItemIndex = -1;
	EditParameterValue->Text = "";
}
//---------------------------------------------------------------------------
void TsrxDesignerForm::clearChart( void )
{
	EditChartTitle->Text = "";
	CheckBoxLegend->Checked = false;
	ComboBoxType->ItemIndex = -1;
	ComboBoxSubtype->ItemIndex = -1;
	EditTopAxisTitle->Text = "";
	EditLeftAxisTitle->Text = "";
	EditRightAxisTitle->Text = "";
	EditBottomAxisTitle->Text = "";
	CheckBoxTopGrid->Checked = false;
	CheckBoxLeftGrid->Checked = false;
	CheckBoxRightGrid->Checked = false;
	CheckBoxBottomGrid->Checked = false;

	chartDataDefinitions.clear();
	ListBoxChartData->Items->Clear();

	EditValueField->Text = "";
	EditLabelField->Text = "";
	EditDataChartTitle->Text = "";
	ComboBoxChartDataType->ItemIndex = -1;
	ComboBoxChartDataSubtype->ItemIndex = -1;
	EditChartDataColor->Text = "";
	ComboBoxVerticalAxis->ItemIndex = -1;
	ComboBoxHorizAxis->ItemIndex = -1;
	CheckBoxChartDataLegend->Checked = false;
	EditExplode->Text = "";
	EditOther->Text = "";
	ComboBoxMarks->ItemIndex = -1;

	enableDisableChartTypes( NULL );
}

//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::ButtonNewClick(TObject *Sender)
{
	if( changedFlag )
	{
		int button = Application->MessageBox(
			"Report changed. Do you want to save?",
			"SRX Designer",
			MB_YESNOCANCEL|MB_ICONQUESTION
		);
		if( button == IDYES )
			ButtonSaveClick( Sender );

		if( button == IDCANCEL )
/*@*/		return;
	}

	PageControl->ActivePage = TabSheetReport;

	clearReport();
	clearParameter();
	clearChart();

	changedFlag = false;
	FileName = "";
	Caption = myCaption;
	Application->Title = Caption;
}
//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::ButtonSelectColorClick(TObject *)
{
	char	buffer[16];
	long	color;

	strncpy( buffer, EditChartDataColor->Text.c_str(), 6 );
	buffer[6] = 0;
	sscanf(  buffer, "%lX", &color );
	ColorDialog->Color = (TColor)color;

	if( ColorDialog->Execute() )
	{
		sprintf( buffer, "%06lX", (long)ColorDialog->Color );
		EditChartDataColor->Text = buffer;
	}
}
//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::ButtonNewChartDataClick(TObject *Sender)
{
	ChartDataDefinition	chartData;

	ListBoxChartData->ItemIndex = -1;
	chartData.valueField = "new";
	ListBoxChartData->Items->Add( "new" );
	chartDataDefinitions.addElement( chartData );

	ListBoxChartData->ItemIndex = chartDataDefinitions.size()-1;
	ListBoxChartDataClick( Sender );
	changedFlag = true;
}
//---------------------------------------------------------------------------


void __fastcall TsrxDesignerForm::ButtonDeleteChartDataClick(
	  TObject *)
{
	int	idx = ListBoxChartData->ItemIndex;
	if( idx >= 0 )
	{
		ListBoxChartData->Items->Delete( idx );
		chartDataDefinitions.removeElementAt( idx );

		changedFlag = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::ListBoxChartDataClick(TObject *Sender )
{
	int	i, idx = ListBoxChartData->ItemIndex;
	if( idx >= 0 )
	{
		select = true;
		const ChartDataDefinition	&chartData = chartDataDefinitions[idx];

		EditLabelField->Text = (const char *)chartData.labelField;
		EditValueField->Text = (const char *)chartData.valueField;
		EditDataChartTitle->Text = (const char *)chartData.title;
		ComboBoxChartDataType->ItemIndex = -1;
		for( i=0; i<ComboBoxChartDataType->Items->Count; i++ )
			if( chartData.type == ComboBoxChartDataType->Items->Strings[
				i
			].c_str() )
			{
				ComboBoxChartDataType->ItemIndex = i;
				break;
			}
		ComboBoxChartDataSubtype->ItemIndex = -1;
		for( i=0; i<ComboBoxChartDataSubtype->Items->Count; i++ )
			if( chartData.subtype == ComboBoxChartDataSubtype->Items->Strings[
				i
			].c_str() )
			{
				ComboBoxChartDataSubtype->ItemIndex = i;
				break;
			}
		EditChartDataColor->Text = (const char *)chartData.color;
		ComboBoxVerticalAxis->ItemIndex = -1;
		for( i=0; i<ComboBoxVerticalAxis->Items->Count; i++ )
			if( chartData.vaxis == ComboBoxVerticalAxis->Items->Strings[
				i
			].c_str() )
			{
				ComboBoxVerticalAxis->ItemIndex = i;
				break;
			}
		ComboBoxHorizAxis->ItemIndex = -1;
		for( i=0; i<ComboBoxHorizAxis->Items->Count; i++ )
			if( chartData.haxis == ComboBoxHorizAxis->Items->Strings[
				i
			].c_str() )
			{
				ComboBoxHorizAxis->ItemIndex = i;
				break;
			}
		EditExplode->Text = (const char *)chartData.explode;
		EditOther->Text = (const char *)chartData.others;
		ComboBoxMarks->ItemIndex = -1;
		for( i=0; i<ComboBoxMarks->Items->Count; i++ )
			if( chartData.mark == ComboBoxMarks->Items->Strings[
				i
			].c_str() )
			{
				ComboBoxMarks->ItemIndex = i;
				break;
			}
		CheckBoxChartDataLegend->Checked = chartData.legend;

		enableDisableChartTypes( Sender );

		select = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::ChangeDefaultType(TObject *Sender)
{
	editChange( Sender );
	enableDisableChartTypes( Sender );
}
//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::enableDisableChartTypes(TObject *)
{
	STRING	type = ComboBoxType->ItemIndex >= 0
		? ComboBoxType->Items->Strings[ComboBoxType->ItemIndex].c_str()
		: ""
	;

	ComboBoxSubtype->Enabled = ( type=="area" || type=="bar" );

	if( ComboBoxChartDataType->ItemIndex > 0 )
		type = ComboBoxChartDataType->Items->Strings[ComboBoxChartDataType->ItemIndex].c_str();

	ComboBoxChartDataSubtype->Enabled = ( type=="area" || type=="bar" );

	if( type == "pie" )
	{
		EditExplode->Enabled = true;
		EditOther->Enabled = true;
	}
	else
	{
		EditExplode->Enabled = false;
		EditOther->Enabled = false;
	}
	ComboBoxMarks->Enabled = (type=="bar" || type=="pie");
}
//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::UpdateChartData(TObject *Sender)
{
	if( select )	// this is a response to a user selection of the list box
/***/	return;

	int	idx = ListBoxChartData->ItemIndex;
	if( idx >= 0 )
	{
		STRING			   	listLabel;
		ChartDataDefinition	&chartData = chartDataDefinitions[idx];

		chartData.labelField = EditLabelField->Text.c_str();
		if( EditValueField->Text.Length() )
			chartData.valueField = EditValueField->Text.c_str();
		chartData.title = EditDataChartTitle->Text.c_str();
		if( ComboBoxChartDataType->ItemIndex > 0 )
			chartData.type = ComboBoxChartDataType->Items->Strings[
				ComboBoxChartDataType->ItemIndex
			].c_str();
		if( ComboBoxChartDataSubtype->ItemIndex > 0 )
			chartData.subtype = ComboBoxChartDataSubtype->Items->Strings[
				ComboBoxChartDataSubtype->ItemIndex
			].c_str();
		chartData.color = EditChartDataColor->Text.c_str();
		if( ComboBoxVerticalAxis->ItemIndex > 0 )
			chartData.vaxis = ComboBoxVerticalAxis->Items->Strings[
				ComboBoxVerticalAxis->ItemIndex
			].c_str();
		if( ComboBoxHorizAxis->ItemIndex > 0 )
			chartData.haxis = ComboBoxHorizAxis->Items->Strings[
				ComboBoxHorizAxis->ItemIndex
			].c_str();
		chartData.explode = EditExplode->Text.c_str();
		chartData.others = EditOther->Text.c_str();
		if( ComboBoxMarks->ItemIndex > 0 )
			chartData.mark = ComboBoxMarks->Items->Strings[
				ComboBoxMarks->ItemIndex
			].c_str();
		chartData.legend = CheckBoxChartDataLegend->Checked;

		listLabel  = chartData.valueField;
		listLabel += ':';
		listLabel += chartData.labelField;
		listLabel += ':';
		listLabel += chartData.title;
		ListBoxChartData->Items->Strings[idx] = (const char *)listLabel;

		enableDisableChartTypes( Sender );
		changedFlag = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::ButtonUpClick(TObject *)
{
	int	idx = ListBoxChartData->ItemIndex;
	if( idx >= 1 )
	{
		chartDataDefinitions.moveElement( idx, idx-1 );
		AnsiString	tmp = ListBoxChartData->Items->Strings[idx];
		ListBoxChartData->Items->Strings[idx] = ListBoxChartData->Items->Strings[idx-1];
		ListBoxChartData->Items->Strings[idx-1] = tmp;

		ListBoxChartData->ItemIndex = idx-1;
		changedFlag = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::ButtonDownClick(TObject *)
{
	int	idx = ListBoxChartData->ItemIndex;
	if( idx >= 0 && idx<(int)chartDataDefinitions.size()-1 )
	{
		chartDataDefinitions.moveElement( idx, idx+1 );
		AnsiString	tmp = ListBoxChartData->Items->Strings[idx];
		ListBoxChartData->Items->Strings[idx] = ListBoxChartData->Items->Strings[idx+1];
		ListBoxChartData->Items->Strings[idx+1] = tmp;

		ListBoxChartData->ItemIndex = idx+1;
		changedFlag = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::DatabaseChane(TObject *Sender)
{
	editChange( Sender );
	loadSchema();
}
//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::ListBoxTablesClick(TObject *)
{
	loadFields();	
}
//---------------------------------------------------------------------------
void __fastcall TsrxDesignerForm::insertToMemo( const AnsiString &text )
{
	int			cPos = MemoSql->SelStart;
	AnsiString	memoText = MemoSql->Text;
	AnsiString	newText = memoText.SubString( 1, cPos );

	newText += text;

	newText += memoText.SubString(
		1+cPos+MemoSql->SelLength,
		memoText.Length() - cPos - MemoSql->SelLength
	);

	cPos += text.Length();
	MemoSql->Text = newText;
	MemoSql->SelStart = cPos;
	MemoSql->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TsrxDesignerForm::ListBoxTablesDblClick(TObject *Sender)
{
	TListBox *lb = dynamic_cast<TListBox *>( Sender );
	if( lb )
		insertToMemo( lb->Items->Strings[lb->ItemIndex] );
}
//---------------------------------------------------------------------------

