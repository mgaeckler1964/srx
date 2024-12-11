/*
		Project:		SRX
		Module:			srxViewerFrm.cpp
		Description:	The report viewer
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2024 Martin Gäckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Austria, Linz ``AS IS''
		AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
		TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
		PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR
		CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
		SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
		LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
		USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
		ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
		OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
		OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
		SUCH DAMAGE.
*/

//---------------------------------------------------------------------------
#include <vcl.h>
#include <Series.hpp>

#include <gak/xmlParser.h>
#include <gak/vcl_tools.h>
#include <gak/array.h>
#include <gak/fmtNumber.h>

#pragma hdrstop

#define NUM_VISIBLE_PARAMS	5
#define PARAM_LINE_HEIGHT	22

#include "srxViewerFrm.h"
#include "StatusFrm.h"

//---------------------------------------------------------------------------
using namespace gak;
using namespace vcl;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TsrxViewerForm *srxViewerForm;
//---------------------------------------------------------------------------
#pragma option -RT-
//---------------------------------------------------------------------------
class EXPORT_CSV_THREAD : public StatusThread
{
	TDataSet	*QueryMain;
	STRING		destFile;
	private:
	void perform( void );
	const char *getTitle( void ) const;

	public:
	EXPORT_CSV_THREAD( TDataSet *QueryMain, const STRING &dest ) :
		destFile( dest )
	{
		this->QueryMain = QueryMain;
	}

};

//---------------------------------------------------------------------------
class EXPORT_XML_THREAD : public StatusThread
{
	TDataSet	*QueryMain;
	STRING		destFile, tableName, recordName;
	private:
	void perform( void );
	const char *getTitle( void ) const;

	public:
	EXPORT_XML_THREAD( TDataSet *QueryMain, const STRING &dest, const STRING &tabName, const STRING &recName ) :
		destFile( dest ), tableName( tabName ), recordName( recName )
	{
		this->QueryMain = QueryMain;
	}

};

//---------------------------------------------------------------------------
static bool ShowExportStatus( TDataSet *theQuery )
{
	static TDataSet *lastQuery = NULL;
	static int		recNo = 0;

	if( theQuery != lastQuery )
	{
		lastQuery = theQuery;
		recNo = 0;
	}

	STRING	recNoStr = formatNumber( ++recNo );

	return StatusForm->setStatus( "Export", recNoStr );
}

//---------------------------------------------------------------------------
void EXPORT_CSV_THREAD::perform( void )
{
	exportTable2CSV( QueryMain, destFile, ShowExportStatus );

	ShellExecute( NULL, "open", destFile, "", "", SW_SHOW );
}

//---------------------------------------------------------------------------
void EXPORT_XML_THREAD::perform( void )
{
	std::ofstream	fStream( destFile );
	if( fStream.is_open() )
	{
		xml::Any *theData = exportTable2XML( QueryMain, tableName, recordName, 2 );
		STRING	xmlCode = theData->generateDoc( tableName );
		fStream << xmlCode;
		delete theData;

		ShellExecute( NULL, "open", destFile, "", "", SW_SHOW );
	}

}

//---------------------------------------------------------------------------
const char *EXPORT_CSV_THREAD::getTitle( void ) const
{
	return "Export CSV";
}

//---------------------------------------------------------------------------
const char *EXPORT_XML_THREAD::getTitle( void ) const
{
	return "Export XML";
}

#pragma option -RT.

//---------------------------------------------------------------------------
void ParameterGui::createGui(
	TScrollBox *scrollBox,
	const STRING &label, const STRING &value,
	const STRING &type, const STRING &query,
	int row
)
{
	int	top = row*PARAM_LINE_HEIGHT+8;
	theLabel = new TLabel( scrollBox );

	theLabel->Parent = scrollBox;
	theLabel->Caption = (const char *)label;
	theLabel->Top = top;
	theLabel->Left = 8;

	if( type == "date" )
	{
		theDatePicker = new TDateTimePicker( scrollBox );
		theDatePicker->Parent = scrollBox;
		theDatePicker->Top = top;
		theDatePicker->Kind = dtkDate;
		theDatePicker->Anchors.Clear();
		theDatePicker->Anchors << akLeft << akTop << akRight;
	}
	else if( type == "time" )
	{
		theTimePicker = new TDateTimePicker( scrollBox );
		theTimePicker->Parent = scrollBox;
		theTimePicker->Top = top;
		theTimePicker->Width =  100;
		theTimePicker->Kind = dtkTime;
		theTimePicker->Anchors.Clear();
		theTimePicker->Anchors << akLeft << akTop;
	}
	else if( type == "date_time" )
	{
		theDatePicker = new TDateTimePicker( scrollBox );
		theDatePicker->Parent = scrollBox;
		theDatePicker->Top = top;
		theDatePicker->Kind = dtkDate;
		theDatePicker->Anchors.Clear();
		theDatePicker->Anchors << akLeft << akTop << akRight;

		theTimePicker = new TDateTimePicker( scrollBox );
		theTimePicker->Parent = scrollBox;
		theTimePicker->Top = top;
		theTimePicker->Width =  100;
		theTimePicker->Kind = dtkTime;
		theTimePicker->Anchors.Clear();
		theTimePicker->Anchors << akRight << akTop;
	}
	else if( type == "query" )
	{
		ParameterQueryValue		theQueryValue;

		theComboBox = new TComboBox( scrollBox );
		theComboBox->Parent = scrollBox;
		theComboBox->Top = top;
		theComboBox->Anchors.Clear();
		theComboBox->Anchors << akLeft << akTop << akRight;
		theComboBox->Style = Stdctrls::csDropDownList;
		theComboBox->Sorted = false;

		srxViewerForm->QueryParam->SQL->Add( (const char *)query );
		for(
			srxViewerForm->QueryParam->Open();
			!srxViewerForm->QueryParam->Eof;
			srxViewerForm->QueryParam->Next()
		)
		{
			if( this->type.isEmpty() )
			{
				switch( srxViewerForm->QueryParam->Fields->Fields[0]->DataType )
				{
					case ftString:
						this->type = "string";
						break;
					case ftSmallint:
					case ftInteger:
					case ftWord:
					case ftAutoInc:
					case ftLargeint:
						this->type = "int";
						break;
					case ftFloat:
					case ftCurrency	:
						this->type = "float";
						break;
					case ftDate:
						this->type = "date";
						break;
					case ftTime:
						this->type = "time";
						break;
					case ftDateTime:
						this->type = "date_time";
						break;
					case ftBoolean:
						this->type = "boolean";
						break;
				}
			}
			theQueryValue.paramValue = srxViewerForm->QueryParam->Fields->Fields[0]->AsString.c_str();
			if( srxViewerForm->QueryParam->FieldCount > 1 )
				theQueryValue.displayValue = srxViewerForm->QueryParam->Fields->Fields[1]->AsString.c_str();
			else
				theQueryValue.displayValue = theQueryValue.paramValue;

			theComboBox->Items->Add( (const char *)theQueryValue.displayValue );
			paramQueryValues.addElement( theQueryValue );
		}
		theComboBox->ItemIndex = 0;
	}
	else
	{
		theEditControl = new TEdit( scrollBox );
		theEditControl->Parent = scrollBox;
		theEditControl->Top = top;
		theEditControl->Anchors.Clear();
		theEditControl->Anchors << akLeft << akTop << akRight;
	}


	moveControl( scrollBox->Width, 50 );
	setValue( value );
}
//---------------------------------------------------------------------------
void ParameterGui::moveControl( int width, int pos )
{
	if( theEditControl )
	{
		theEditControl->Left = pos;
		theEditControl->Width = width - pos - 8;
	}
	else if( theComboBox )
	{
		theComboBox->Left = pos;
		theComboBox->Width = width - pos - 8;
	}
	else if( theDatePicker )
	{
		theDatePicker->Left = pos;
		if( theTimePicker )
		{
			theDatePicker->Width = width - pos - 8- theTimePicker->Width;
			theTimePicker->Left =  width - theTimePicker->Width - 8;
		}
		else
			theDatePicker->Width = width - pos - 8;
	}
	else if( theTimePicker )
		theTimePicker->Left = pos;
}
//---------------------------------------------------------------------------
void ParameterGui::setValue( const STRING &newValue )
{
	if( theEditControl )
		theEditControl->Text = (const char *)newValue;
	else if( theDatePicker && theTimePicker )
	{
		try
		{
			TDateTime	val( (const char *)newValue, TDateTime::DateTime );

			theDatePicker->Date = val;
			theTimePicker->Time = val;
		}
		catch( ... )
		{
		}
	}
	else if( theDatePicker )
	{
		try
		{
			TDateTime	val( (const char *)newValue, TDateTime::Date );
			theDatePicker->Date = val;
		}
		catch( ... )
		{
		}
	}
	else if( theTimePicker )
	{
		try
		{
			TDateTime	val( (const char *)newValue, TDateTime::Time );
			theTimePicker->Time = val;
		}
		catch( ... )
		{
		}
	}
	else if( theComboBox )
	{
		theComboBox->ItemIndex = theComboBox->Items->IndexOf( (const char *)newValue );
	}
}
//---------------------------------------------------------------------------
STRING ParameterGui::getValue( void )
{
	STRING value;
	if( theEditControl )
		value += theEditControl->Text.c_str();
	if( theComboBox && theComboBox->ItemIndex >= 0 )
		value += paramQueryValues[theComboBox->ItemIndex].paramValue;
	if( theDatePicker )
		value += theDatePicker->Date.DateString().c_str();
	if( theTimePicker )
	{
		if( !value.isEmpty() )
			value += ' ';
		value += theTimePicker->Time.TimeString().c_str();
	}

	return value;
}
//---------------------------------------------------------------------------
void ParamEditor::setLabelWidth( TScrollBox *scrollBox, int labelWidth )
{
	std::size_t	numElements = size();
	int			scrollWidth = scrollBox->Width;

	if( numElements > NUM_VISIBLE_PARAMS )
	{
		scrollWidth -= 20;
	}

	for( iterator it=begin(), endIT = end(); it != endIT; ++it )
	{
		it->moveControl( scrollWidth, labelWidth );
	}
}
//---------------------------------------------------------------------------
__fastcall TsrxViewerForm::TsrxViewerForm(TComponent* Owner)
	: TForm(Owner)
{
	theRoot = NULL;
}
//---------------------------------------------------------------------------
void TsrxViewerForm::loadReport2( xml::Element *theRoot )
{
	int					theColor;
	TChartSeries    	*series;
	TBarSeries	    	*barSeries;
	TAreaSeries			*areaSeries;
	TPieSeries			*pieSeries;
	xml::Any			*theData;
	STRING				title,  defType, type, defSubtype, subtype, legend,
						color, label, value, query,
						mark, haxis, vaxis, axisTitle, axisGrid,
						explode, others;
	size_t				i, maxLen = 16;
	bool				sqlFound = false;
	bool				chartFound = false;

	/*
		setup the report
		================
	*/
	tableName = theRoot->getAttribute( "table_name" );
	recordName = theRoot->getAttribute( "record_name" );
	if( tableName.isEmpty() )
		tableName = "data";
	if( recordName.isEmpty() )
		recordName = "entry";

	title = theRoot->getAttribute( "title" );
	if( !title.isEmpty() )
		Caption = Caption + " - " + (const char*)title;
	this->title = title;

	/*
		setup the chart
		===============
	*/
	theChart = static_cast<xml::Any*>( theRoot->getElement( "chart" ) );
	if( theChart )
	{
		Chart->RemoveAllSeries();

		defType = theChart->getAttribute( "type" );
		defSubtype = theChart->getAttribute( "subtype" );
		for( i=0; i<theChart->getNumObjects(); i++ )
		{
			theData = static_cast<xml::Any*>( theChart->getElement( i ) );
			if( theData && theData->getTag() == "data" )
			{
				type = theData->getAttribute( "type" );
				if( type.isEmpty() )
					type = defType;
				subtype = theData->getAttribute( "subtype" );
				if( subtype.isEmpty() )
					subtype = defSubtype;

				if( type == "line" )
					series = new TLineSeries( this );
				else if( type == "bar" )
				{
					series = barSeries = new TBarSeries( this );
					if( subtype == "normal" )
						barSeries->MultiBar = Series::mbNone;
					else if( subtype == "side" )
						barSeries->MultiBar = Series::mbSide;
					else if( subtype == "stacked" )
						barSeries->MultiBar = Series::mbStacked;
					else if( subtype == "stacked100" )
						barSeries->MultiBar = Series::mbStacked100;
				}
				else if( type == "area" )
				{
					series = areaSeries = new TAreaSeries( this );
					if( subtype == "normal" )
						areaSeries->MultiArea = Series::maNone;
					else if( subtype == "stacked" )
						areaSeries->MultiArea = Series::maStacked;
					else if( subtype == "stacked100" )
						areaSeries->MultiArea = Series::maStacked100;
				}
				else if( type == "pie" )
				{
					series = pieSeries = new TPieSeries( this );
					explode = theData->getAttribute( "explode" );
					pieSeries->ExplodeBiggest = atoi( explode );
					others = theData->getAttribute( "others" );
					if( !others.isEmpty() )
					{
						if( others[others.strlen()-1] == '%' )
							pieSeries->OtherSlice->Style = poBelowPercent;
						else
							pieSeries->OtherSlice->Style = poBelowValue;
						pieSeries->OtherSlice->Value = atoi( others );
					}
				}
				else
					series = NULL;

				if( series )
				{
					title = theData->getAttribute( "title" );
					series->Title = (const char *)title;

					color = theData->getAttribute( "color" );
					sscanf( color, "%x", &theColor );
					series->SeriesColor = (TColor)theColor;

					mark = theData->getAttribute( "mark" );
					if( mark == "hidden" )
						series->Marks->Visible = false;
					else if( mark == "value" )
						series->Marks->Style = smsValue;
					else if( mark == "percent" )
						series->Marks->Style = smsPercent;
					else if( mark == "label" )
						series->Marks->Style = smsLabel;
					else if( mark == "labelPercent" )
						series->Marks->Style = smsLabelPercent;
					else if( mark == "labelValue" )
						series->Marks->Style = smsLabelValue;
					else if( mark == "percentTotal" )
						series->Marks->Style = smsPercentTotal;
					else if( mark == "labelPercentTotal" )
						series->Marks->Style = smsLabelPercentTotal;

					vaxis = theData->getAttribute( "vaxis" );
					if( vaxis == "left" )
						series->VertAxis = aLeftAxis;
					else if( vaxis == "right" )
						series->VertAxis = aRightAxis;
					else if( vaxis == "both" )
						series->VertAxis = aBothVertAxis;

					haxis = theData->getAttribute( "haxis" );
					if( haxis == "top" )
						series->HorizAxis = aTopAxis;
					else if( haxis == "bottom" )
						series->HorizAxis = aBottomAxis;
					else if( haxis == "both" )
						series->HorizAxis = aBothHorizAxis;

					legend = theData->getAttribute( "legend" );
					if( legend == "yes" )
						series->ShowInLegend = true;
					else if( legend == "no" )
						series->ShowInLegend = false;

					series->ParentChart = Chart;
					series->Tag = (int)theData;
					chartFound = true;
				} // if( series )
			} // if( theData && theData->getTag() == "data" )
		} // for( i=0; i<theChart->getNumObjects(); i++ )

		title = theChart->getAttribute( "title" );
		if( title.isEmpty() )
			title = this->title;
		if( title.isEmpty() )
			Chart->Title->Visible = false;
		else
		{
			Chart->Title->Text->Clear();
			Chart->Title->Text->Add( (const char *)title );
		}

		legend = theChart->getAttribute( "legend" );
		if( legend == "yes" )
			Chart->Legend->Visible = true;
		else if( legend == "no" )
			Chart->Legend->Visible = false;

		axisTitle = theChart->getAttribute( "leftAxisTitle" );
		if( !axisTitle.isEmpty() )
			Chart->LeftAxis->Title->Caption = (const char *)axisTitle;

		axisTitle = theChart->getAttribute( "rightAxisTitle" );
		if( !axisTitle.isEmpty() )
			Chart->RightAxis->Title->Caption = (const char *)axisTitle;

		axisTitle = theChart->getAttribute( "bottomAxisTitle" );
		if( !axisTitle.isEmpty() )
			Chart->BottomAxis->Title->Caption = (const char *)axisTitle;

		axisTitle = theChart->getAttribute( "topAxisTitle" );
		if( !axisTitle.isEmpty() )
			Chart->TopAxis->Title->Caption = (const char *)axisTitle;

		axisGrid = theChart->getAttribute( "leftAxisGrid" );
		if( axisGrid=="yes" )
			Chart->LeftAxis->Grid->Visible = true;
		else if( axisGrid=="no" )
			Chart->LeftAxis->Grid->Visible = false;

		axisGrid = theChart->getAttribute( "rightAxisGrid" );
		if( axisGrid=="yes" )
			Chart->RightAxis->Grid->Visible = true;
		else if( axisGrid=="no" )
			Chart->RightAxis->Grid->Visible = false;

		axisGrid = theChart->getAttribute( "bottomAxisGrid" );
		if( axisGrid=="yes" )
			Chart->BottomAxis->Grid->Visible = true;
		else if( axisGrid=="no" )
			Chart->BottomAxis->Grid->Visible = false;

		axisGrid = theChart->getAttribute( "topAxisGrid" );
		if( axisGrid=="yes" )
			Chart->TopAxis->Grid->Visible = true;
		else if( axisGrid=="no" )
			Chart->TopAxis->Grid->Visible = false;
	} // if( theChart )

	/*
		setup the query
		===============
	*/
	xml::Element *theQuery = theRoot->getElement( "query" );
	if( theQuery )
	{
		/*
			the SQL command
		*/
		xml::Element *theSql = theQuery->getElement( "sql" );
		if( theSql )
		{
			STRING	sqlText = theSql->getValue( xml::PLAIN_MODE );
			QueryMain->SQL->Add( (const char *)sqlText );
			sqlFound = true;
		}

		/*
			the SQL parameters
		*/
		this->params.clear();
		xml::Element	*params = theQuery->getElement( "params" );
		if( params )
		{
			i = 0;

			while( i<params->getNumObjects() )
			{
				xml::Any *theParameter = static_cast<xml::Any*>(
					params->getElement( i )
				);

				if( theParameter && theParameter->getTag() == "param" )
				{
					ParameterGui	newRow;

					label = theParameter->getAttribute( "label" );
					if( label.strlen() > maxLen )
						maxLen = label.strlen();
					value = theParameter->getAttribute( "value" );
					type = theParameter->getAttribute( "type" );
					xml::Element	*queryElement = theParameter->getElement( "query" );
					if( queryElement )
						query = queryElement->getValue( xml::PLAIN_MODE );
					else
						query = "";

					newRow.createGui(
						ScrollBoxParams, label, value, type, query,
						theParameterEditor.size()
					);

					this->params.addElement( theParameter );

					theParameterEditor.addElement( newRow );
				}
				i++;
			}
			theParameterEditor.setLabelWidth( ScrollBoxParams, maxLen * 8 );

			if( theParameterEditor.size() < NUM_VISIBLE_PARAMS )
				ScrollBoxParams->Height = theParameterEditor.size() * PARAM_LINE_HEIGHT + 16;
			else
				ScrollBoxParams->Height = NUM_VISIBLE_PARAMS * PARAM_LINE_HEIGHT + 16;
		} // if( params )

		if( !this->params.size() )
		{
			ScrollBoxParams->Visible = false;
			QueryMain->Open();
		}
		else
			ScrollBoxParams->Visible = true;
	} // if( theQuery )

	if( !sqlFound )
	{
		Application->MessageBox(
			"There is no SQL-Statement defined",
			"Error",
			MB_OK|MB_ICONERROR
		);
	}
	else
	{
		ButtonStart->Enabled = true;
		ButtonCSVexport->Enabled = true;
		ButtonXMLexport->Enabled = true;

		if( chartFound )
			TabSheetChart->TabVisible = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TsrxViewerForm::FormShow(TObject *)
{
	ArrayOfStrings	theCmdLineParams;
	STRING		  	commandLine = (const char *)Application->Tag;

	ButtonStart->Enabled = false;
	ButtonCSVexport->Enabled = false;
	ButtonXMLexport->Enabled = false;
	TabSheetChart->TabVisible = false;

	theCmdLineParams.createElements( commandLine );
	if( theCmdLineParams.size() == 2 )
	{
		T_STRING	dbInfo = theCmdLineParams[0];
		STRING		reportFile = theCmdLineParams[1];

		T_STRING	userInfo = dbInfo.getFirstToken( "@" );
		STRING		dbAlias = dbInfo.getNextToken();
		if( dbAlias.isEmpty() )
		{
			dbAlias = userInfo;
			userInfo = "";
		}

		userName = userInfo.getFirstToken( "/" );
		password = userInfo.getNextToken();

		xml::Parser		theParser( reportFile );
		xml::Document	*theReportDoc = theParser.readFile( false );

		DatabaseMain->AliasName = (const char *)dbAlias;
		if( !userName.isEmpty() )
			DatabaseMain->Params->Values["USER NAME"] = (const char *)userName;
		if( !password.isEmpty() )
			DatabaseMain->Params->Values["PASSWORD"] = (const char *)password;

		if( theReportDoc )
			theRoot = theReportDoc->getElement( "sql_report" );
	} // if( theCmdLineParams.size() == 2 )

	if( theRoot )
		loadReport2( theRoot );
	else if( Application->Tag )
	{
		Application->MessageBox(
			"There is bad SQL-Report defined",
			"Error",
			MB_OK|MB_ICONERROR
		);
		exit( -1 );
	}
}
//---------------------------------------------------------------------------
void __fastcall TsrxViewerForm::ButtonStartClick(TObject *)
{
	doEnterFunction( "TsrxViewerForm::ButtonStartClick" );

	STRING		paramName, type;
	AnsiString	value;
	TDateTime	dateValue;

	colWidths.clear();
	for( int i=0; i<DBGridMain->Columns->Count; i++ )
	{
		colWidths[i] = DBGridMain->Columns->Items[i]->Width;
	}

	QueryMain->Close();
	for( size_t i=0; i<params.size(); i++ )
	{
		xml::Any *theParameter = params[i];
		paramName = theParameter->getAttribute( "name" );
		type = theParameter->getAttribute( "type" );
		value = (const char *)theParameterEditor[i].getValue();

		if( type == "query" )
			type = theParameterEditor[i].getType();

		if( type == "int" )
			QueryMain->ParamByName( (const char *)paramName )->AsInteger = value.ToInt();
		else if( type == "float" )
			QueryMain->ParamByName( (const char *)paramName )->AsFloat = value.ToDouble();
		else if( type == "time" )
		{
			TDateTime	newTime( value, TDateTime::Time );
			QueryMain->ParamByName( (const char *)paramName )->AsTime = newTime;
		}
		else if( type == "date" )
		{
			TDateTime	newDate( value, TDateTime::Date );
			QueryMain->ParamByName( (const char *)paramName )->AsDate = newDate;
		}
		else if( type == "date_time" )
		{
			TDateTime	newDateTime( value, TDateTime::DateTime );
			QueryMain->ParamByName( (const char *)paramName )->AsDateTime = newDateTime;
		}
		else if( type == "string" )
			QueryMain->ParamByName( (const char *)paramName )->AsString = value;
		else if( type == "boolean" )
			QueryMain->ParamByName( (const char *)paramName )->AsString = value;

	} // for( size_t i=0; i<params.size(); i++ )

	QueryMain->Open();
}
//---------------------------------------------------------------------------

void __fastcall TsrxViewerForm::ButtonCSVexportClick(TObject *Sender)
{
	doEnterFunction("TsrxViewerForm::ButtonCSVexportClick");

	STRING	destFile;

	destFile = Session->PrivateDir.c_str();
	if( !destFile.strlen() )
	{
		char	*tmp = getenv( "TEMP" );
		if( tmp )
			destFile = tmp;
		else
			destFile = ".";
	}

	destFile += "\\export.csv";

	if( !QueryMain->Active || params.size() )
		ButtonStartClick( Sender );

	EXPORT_CSV_THREAD	*theThread = new EXPORT_CSV_THREAD( QueryMain, destFile );
	theThread->StartThread( false );
}

//---------------------------------------------------------------------------

void __fastcall TsrxViewerForm::ButtonXMLexportClick(TObject *Sender)
{
	doEnterFunction("TsrxViewerForm::ButtonXMLexportClick");

	STRING	destFile;

	destFile = Session->PrivateDir.c_str();
	if( !destFile.strlen() )
	{
		char	*tmp = getenv( "TEMP" );
		if( tmp )
			destFile = tmp;
		else
			destFile = ".";
	}

	destFile += "\\export.xml";

	if( !QueryMain->Active || params.size() )
		ButtonStartClick( Sender );

	EXPORT_XML_THREAD	*theThread = new EXPORT_XML_THREAD(
		QueryMain, destFile, tableName, recordName
	);
	theThread->StartThread( false );
}
//---------------------------------------------------------------------------



void __fastcall TsrxViewerForm::DatabaseMainLogin(TDatabase *,
	  TStrings *LoginParams)
{
	if( !userName.isEmpty() )
		LoginParams->Values["USER NAME"] = (const char *)userName;
	if( !password.isEmpty() )
		LoginParams->Values["PASSWORD"] = (const char *)password;
}
//---------------------------------------------------------------------------


void __fastcall TsrxViewerForm::FormCreate(TObject *)
{
	char		buffer[128];
	const char	*temp;
	AnsiString	privateDir = "C:\\";

	if( (temp=getenv("TEMP")) != NULL )
		privateDir = temp;
	if( privateDir[privateDir.Length()] != '\\' )
		privateDir += '\\';

	privateDir += "SRX_VIEWER";
	sprintf( buffer, "%d", (int)GetCurrentProcessId() );
	privateDir += buffer;

	mkdir( privateDir.c_str() );
	Session->PrivateDir = privateDir;
}
//---------------------------------------------------------------------------

void __fastcall TsrxViewerForm::FormClose(TObject *,
	  TCloseAction &Action)
{
	STRING	error;
	try
	{
		QueryMain->Close();
	}
	catch( ... )
	{
		error += "Close Query Failed\n";
	}

	try
	{
		DatabaseMain->Close();
	}
	catch( ... )
	{
		error += "Close Database Failed\n";
	}
	try
	{
		Session->Close();
	}
	catch( ... )
	{
		error += "Close Session Failed\n";
	}

	if( !error.isEmpty() )
		Application->MessageBox(
			error,
			"Error",
			MB_OK|MB_ICONWARNING
		);
	rmdir(Session->PrivateDir.c_str());
	theRoot = NULL;

	Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TsrxViewerForm::QueryMainAfterOpen(TDataSet *)
{
	doEnterFunction("TsrxViewerForm::QueryMainAfterOpen");

	TColor colors[]=
	{
		clRed,
		clYellow,
		clGreen,
		clBlue,
		clFuchsia,
		clLime,
		clAqua,
		clPurple,
		clNavy,
		clMaroon,
		clTeal,
		clOlive,
		clWhite,
		clSilver,
		clGray,
		clBlack,
	};

	STRING			label, value, color, title;
	TChartSeries	*series;
	xml::Any		*theData;
	int				theColor;

#if 0
	for( size_t i=0; i<QueryMain->FieldCount; i++ )
	{
		if( QueryMain->Fields->Fields[i]->DisplayWidth > 32 )
			QueryMain->Fields->Fields[i]->DisplayWidth = 32;
	}
#endif

	if( int(colWidths.size()) == DBGridMain->Columns->Count )
	{
		std::size_t i=0;
		for(
			ArrayOfInts::const_iterator it = colWidths.cbegin(),
				endIT = colWidths.cend();
			it != endIT;
			++it, ++i
		)
		{
			DBGridMain->Columns->Items[i]->Width = *it;
		}
	}
	else
	{
		int		gridWidth = DBGridMain->Width - 50;
		int     sumWidthOK = 0;
		int		numFields2Big = 0;

		colWidths.clear();
		for( int i=0; i<DBGridMain->Columns->Count; i++ )
		{
			if( DBGridMain->Columns->Items[i]->Width >= gridWidth )
				numFields2Big++;
			else
				sumWidthOK += DBGridMain->Columns->Items[i]->Width;
		}
		if( numFields2Big )
		{
			int	newSize = (gridWidth - sumWidthOK)/numFields2Big;
			if( newSize < gridWidth/2 )
				newSize = gridWidth/2;

			for( int i=0; i<DBGridMain->Columns->Count; i++ )
			{
				if( DBGridMain->Columns->Items[i]->Width >= gridWidth )
				{
					DBGridMain->Columns->Items[i]->Width = newSize;
				}
			}
		}
	}

	PageControl->Visible = true;
	if( theChart && Chart->SeriesCount() )
	{
		for( std::size_t i=0; i<(size_t)Chart->SeriesCount(); i++ )
		{
			series = Chart->Series[i];
			series->Clear();
		}
		std::size_t recNo;
		for( recNo = 0, QueryMain->First(); !QueryMain->Eof; QueryMain->Next(), recNo++ )
		{
			for( std::size_t i=0; i<std::size_t(Chart->SeriesCount()); i++ )
			{
				series = Chart->Series[i];
				theData = reinterpret_cast<xml::Any*>( series->Tag );
				label = theData->getAttribute( "label" );
				value = theData->getAttribute( "value" );
				color = theData->getAttribute( "color" );

				if( !color.isEmpty() )
					theColor = series->SeriesColor;
				else
					theColor = colors[recNo%16];

				series->Add(
					QueryMain->FieldByName( (const char *)value )->AsFloat,
					QueryMain->FieldByName( (const char *)label )->AsString,
					(TColor)theColor
				);
			}
		}
		QueryMain->First();
	}
}
//---------------------------------------------------------------------------

void __fastcall TsrxViewerForm::PrintClick(TObject *)
{
	if( PrintDialog->Execute() )
		Chart->Print();
}
//---------------------------------------------------------------------------

void __fastcall TsrxViewerForm::SaveClick(TObject *)
{
	if( SaveDialog->Execute() )
		if( SaveDialog->FilterIndex == 1 )
			Chart->SaveToBitmapFile( SaveDialog->FileName );
		else
			Chart->SaveToMetafileEnh( SaveDialog->FileName );
}
//---------------------------------------------------------------------------

void __fastcall TsrxViewerForm::DBGridMainDblClick(TObject *)
{
	TField *curField = DBGridMain->SelectedField;
	Application->MessageBox(
		curField->AsString.c_str(),
		curField->DisplayLabel.c_str(),
		MB_OK
	);
}
//---------------------------------------------------------------------------
void TsrxViewerForm::loadReport(
	const STRING &userName, const STRING &password,
	const STRING &dbAlias,
	xml::Element *theRoot
)
{
	this->userName = userName;
	this->password = password;
	this->theRoot = theRoot;

	DatabaseMain->AliasName = (const char *)dbAlias;
	if( !userName.isEmpty() )
		DatabaseMain->Params->Values["USER NAME"] = (const char *)userName;
	if( !password.isEmpty() )
		DatabaseMain->Params->Values["PASSWORD"] = (const char *)password;
	ShowModal();
}
//---------------------------------------------------------------------------

