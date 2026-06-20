/*
		Project:		SRX
		Module:			srxViewerFrm.cpp
		Description:	The report viewer
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 2009-2026 Martin Gäckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Linz, Austria ``AS IS''
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
	m_theLabel = new TLabel( scrollBox );

	m_theLabel->Parent = scrollBox;
	m_theLabel->Caption = label.c_str();
	m_theLabel->Top = top;
	m_theLabel->Left = 8;

	if( type == "date" )
	{
		m_theDatePicker = new TDateTimePicker( scrollBox );
		m_theDatePicker->Parent = scrollBox;
		m_theDatePicker->Top = top;
		m_theDatePicker->Kind = dtkDate;
		m_theDatePicker->Anchors.Clear();
		m_theDatePicker->Anchors << akLeft << akTop << akRight;
	}
	else if( type == "time" )
	{
		m_theTimePicker = new TDateTimePicker( scrollBox );
		m_theTimePicker->Parent = scrollBox;
		m_theTimePicker->Top = top;
		m_theTimePicker->Width =  100;
		m_theTimePicker->Kind = dtkTime;
		m_theTimePicker->Anchors.Clear();
		m_theTimePicker->Anchors << akLeft << akTop;
	}
	else if( type == "date_time" )
	{
		m_theDatePicker = new TDateTimePicker( scrollBox );
		m_theDatePicker->Parent = scrollBox;
		m_theDatePicker->Top = top;
		m_theDatePicker->Kind = dtkDate;
		m_theDatePicker->Anchors.Clear();
		m_theDatePicker->Anchors << akLeft << akTop << akRight;

		m_theTimePicker = new TDateTimePicker( scrollBox );
		m_theTimePicker->Parent = scrollBox;
		m_theTimePicker->Top = top;
		m_theTimePicker->Width =  100;
		m_theTimePicker->Kind = dtkTime;
		m_theTimePicker->Anchors.Clear();
		m_theTimePicker->Anchors << akRight << akTop;
	}
	else if( type == "query" )
	{
		ParameterQueryValue		theQueryValue;

		m_theComboBox = new TComboBox( scrollBox );
		m_theComboBox->Parent = scrollBox;
		m_theComboBox->Top = top;
		m_theComboBox->Anchors.Clear();
		m_theComboBox->Anchors << akLeft << akTop << akRight;
		m_theComboBox->Style = Stdctrls::csDropDownList;
		m_theComboBox->Sorted = false;

		srxViewerForm->QueryParam->SQL->Add( query.c_str() );
		for(
			srxViewerForm->QueryParam->Open();
			!srxViewerForm->QueryParam->Eof;
			srxViewerForm->QueryParam->Next()
		)
		{
			if( m_type.isEmpty() )
			{
				switch( srxViewerForm->QueryParam->Fields->Fields[0]->DataType )
				{
					case ftString:
						m_type = "string";
						break;
					case ftSmallint:
					case ftInteger:
					case ftWord:
					case ftAutoInc:
					case ftLargeint:
						m_type = "int";
						break;
					case ftFloat:
					case ftCurrency	:
						m_type = "float";
						break;
					case ftDate:
						m_type = "date";
						break;
					case ftTime:
						m_type = "time";
						break;
					case ftDateTime:
						m_type = "date_time";
						break;
					case ftBoolean:
						m_type = "boolean";
						break;
				}
			}
			theQueryValue.paramValue = srxViewerForm->QueryParam->Fields->Fields[0]->AsString.c_str();
			if( srxViewerForm->QueryParam->FieldCount > 1 )
				theQueryValue.displayValue = srxViewerForm->QueryParam->Fields->Fields[1]->AsString.c_str();
			else
				theQueryValue.displayValue = theQueryValue.paramValue;

			m_theComboBox->Items->Add( theQueryValue.displayValue.c_str() );
			m_paramQueryValues.addElement( theQueryValue );
		}
		m_theComboBox->ItemIndex = 0;
	}
	else
	{
		m_theEditControl = new TEdit( scrollBox );
		m_theEditControl->Parent = scrollBox;
		m_theEditControl->Top = top;
		m_theEditControl->Anchors.Clear();
		m_theEditControl->Anchors << akLeft << akTop << akRight;
	}


	moveControl( scrollBox->Width, 50 );
	setValue( value );
}
//---------------------------------------------------------------------------
void ParameterGui::moveControl( int width, int pos )
{
	if( m_theEditControl )
	{
		m_theEditControl->Left = pos;
		m_theEditControl->Width = width - pos - 8;
	}
	else if( m_theComboBox )
	{
		m_theComboBox->Left = pos;
		m_theComboBox->Width = width - pos - 8;
	}
	else if( m_theDatePicker )
	{
		m_theDatePicker->Left = pos;
		if( m_theTimePicker )
		{
			m_theDatePicker->Width = width - pos - 8- m_theTimePicker->Width;
			m_theTimePicker->Left =  width - m_theTimePicker->Width - 8;
		}
		else
			m_theDatePicker->Width = width - pos - 8;
	}
	else if( m_theTimePicker )
		m_theTimePicker->Left = pos;
}
//---------------------------------------------------------------------------
void ParameterGui::setValue( const STRING &newValue )
{
	if( m_theEditControl )
		m_theEditControl->Text = newValue.c_str();
	else if( m_theDatePicker && m_theTimePicker )
	{
		try
		{
			TDateTime	val( newValue.c_str(), TDateTime::DateTime );

			m_theDatePicker->Date = val;
			m_theTimePicker->Time = val;
		}
		catch( ... )
		{
		}
	}
	else if( m_theDatePicker )
	{
		try
		{
			TDateTime	val( newValue.c_str(), TDateTime::Date );
			m_theDatePicker->Date = val;
		}
		catch( ... )
		{
		}
	}
	else if( m_theTimePicker )
	{
		try
		{
			TDateTime	val( newValue.c_str(), TDateTime::Time );
			m_theTimePicker->Time = val;
		}
		catch( ... )
		{
		}
	}
	else if( m_theComboBox )
	{
		m_theComboBox->ItemIndex = m_theComboBox->Items->IndexOf( newValue.c_str() );
	}
}
//---------------------------------------------------------------------------
STRING ParameterGui::getValue( void )
{
	STRING value;
	if( m_theEditControl )
		value += m_theEditControl->Text.c_str();
	if( m_theComboBox && m_theComboBox->ItemIndex >= 0 )
		value += m_paramQueryValues[m_theComboBox->ItemIndex].paramValue;
	if( m_theDatePicker )
		value += m_theDatePicker->Date.DateString().c_str();
	if( m_theTimePicker )
	{
		if( !value.isEmpty() )
			value += ' ';
		value += m_theTimePicker->Time.TimeString().c_str();
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
	: TForm(Owner), m_theRoot(nullptr)
{
}

//---------------------------------------------------------------------------
void TsrxViewerForm::loadReport2( xml::Element *theRoot, const gak::STRING &reportFile )
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
	m_tableName = theRoot->getAttribute( "table_name" );
	m_recordName = theRoot->getAttribute( "record_name" );
	if( m_tableName.isEmpty() )
		m_tableName = "data";
	if( m_recordName.isEmpty() )
		m_recordName = "entry";

	title = theRoot->getAttribute( "title" );
	if( !title.isEmpty() )
		Caption = Caption + " - " + title.c_str();
	else if( !reportFile.isEmpty() )
		Caption = Caption + " - " + reportFile.c_str();
	m_title = title;

	/*
		setup the chart
		===============
	*/
	m_theChart = static_cast<xml::Any*>( theRoot->getElement( "chart" ) );
	if( m_theChart )
	{
		Chart->RemoveAllSeries();

		defType = m_theChart->getAttribute( "type" );
		defSubtype = m_theChart->getAttribute( "subtype" );
		for( i=0; i<m_theChart->getNumObjects(); i++ )
		{
			theData = static_cast<xml::Any*>( m_theChart->getElement( i ) );
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
					series->Title = title.c_str();

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

		title = m_theChart->getAttribute( "title" );
		if( title.isEmpty() )
			title = m_title;
		if( title.isEmpty() )
			Chart->Title->Visible = false;
		else
		{
			Chart->Title->Text->Clear();
			Chart->Title->Text->Add( title.c_str() );
		}

		legend = m_theChart->getAttribute( "legend" );
		if( legend == "yes" )
			Chart->Legend->Visible = true;
		else if( legend == "no" )
			Chart->Legend->Visible = false;

		axisTitle = m_theChart->getAttribute( "leftAxisTitle" );
		if( !axisTitle.isEmpty() )
			Chart->LeftAxis->Title->Caption = axisTitle.c_str();

		axisTitle = m_theChart->getAttribute( "rightAxisTitle" );
		if( !axisTitle.isEmpty() )
			Chart->RightAxis->Title->Caption = axisTitle.c_str();

		axisTitle = m_theChart->getAttribute( "bottomAxisTitle" );
		if( !axisTitle.isEmpty() )
			Chart->BottomAxis->Title->Caption = axisTitle.c_str();

		axisTitle = m_theChart->getAttribute( "topAxisTitle" );
		if( !axisTitle.isEmpty() )
			Chart->TopAxis->Title->Caption = axisTitle.c_str();

		axisGrid = m_theChart->getAttribute( "leftAxisGrid" );
		if( axisGrid=="yes" )
			Chart->LeftAxis->Grid->Visible = true;
		else if( axisGrid=="no" )
			Chart->LeftAxis->Grid->Visible = false;

		axisGrid = m_theChart->getAttribute( "rightAxisGrid" );
		if( axisGrid=="yes" )
			Chart->RightAxis->Grid->Visible = true;
		else if( axisGrid=="no" )
			Chart->RightAxis->Grid->Visible = false;

		axisGrid = m_theChart->getAttribute( "bottomAxisGrid" );
		if( axisGrid=="yes" )
			Chart->BottomAxis->Grid->Visible = true;
		else if( axisGrid=="no" )
			Chart->BottomAxis->Grid->Visible = false;

		axisGrid = m_theChart->getAttribute( "topAxisGrid" );
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
			QueryMain->SQL->Add( sqlText.c_str() );
			sqlFound = true;
		}

		/*
			the SQL parameters
		*/
		m_params.clear();
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
						m_theParameterEditor.size()
					);

					m_params.addElement( theParameter );

					m_theParameterEditor.addElement( newRow );
				}
				i++;
			}
			m_theParameterEditor.setLabelWidth( ScrollBoxParams, maxLen * 8 );

			if( m_theParameterEditor.size() < NUM_VISIBLE_PARAMS )
				ScrollBoxParams->Height = m_theParameterEditor.size() * PARAM_LINE_HEIGHT + 16;
			else
				ScrollBoxParams->Height = NUM_VISIBLE_PARAMS * PARAM_LINE_HEIGHT + 16;
		} // if( params )

		if( !m_params.size() )
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
	STRING			title;

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

		m_userName = userInfo.getFirstToken( "/" );
		m_password = userInfo.getNextToken();

		xml::Parser		theParser( reportFile );
		xml::Document	*theReportDoc = theParser.readFile( false );

		DatabaseMain->AliasName = dbAlias.c_str();
		if( !m_userName.isEmpty() )
			DatabaseMain->Params->Values["USER NAME"] = m_userName.c_str();
		if( !m_password.isEmpty() )
			DatabaseMain->Params->Values["PASSWORD"] = m_password.c_str();

		if( theReportDoc )
		{
			m_theRoot = theReportDoc->getElement( "sql_report" );
			title = reportFile;
		}
	} // if( theCmdLineParams.size() == 2 )

	if( m_theRoot )
		loadReport2( m_theRoot, title );
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

	m_colWidths.clear();
	for( int i=0; i<DBGridMain->Columns->Count; i++ )
	{
		m_colWidths[i] = DBGridMain->Columns->Items[i]->Width;
	}

	QueryMain->Close();
	for( size_t i=0; i<m_params.size(); i++ )
	{
		xml::Any *theParameter = m_params[i];
		paramName = theParameter->getAttribute( "name" );
		type = theParameter->getAttribute( "type" );
		value = m_theParameterEditor[i].getValue().c_str();

		if( type == "query" )
			type = m_theParameterEditor[i].getType();

		if( type == "int" )
			QueryMain->ParamByName( paramName.c_str() )->AsInteger = value.ToInt();
		else if( type == "float" )
			QueryMain->ParamByName( paramName.c_str() )->AsFloat = value.ToDouble();
		else if( type == "time" )
		{
			TDateTime	newTime( value, TDateTime::Time );
			QueryMain->ParamByName( paramName.c_str() )->AsTime = newTime;
		}
		else if( type == "date" )
		{
			TDateTime	newDate( value, TDateTime::Date );
			QueryMain->ParamByName( paramName.c_str() )->AsDate = newDate;
		}
		else if( type == "date_time" )
		{
			TDateTime	newDateTime( value, TDateTime::DateTime );
			QueryMain->ParamByName( paramName.c_str() )->AsDateTime = newDateTime;
		}
		else if( type == "string" )
			QueryMain->ParamByName( paramName.c_str() )->AsString = value;
		else if( type == "boolean" )
			QueryMain->ParamByName( paramName.c_str() )->AsString = value;

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

	if( !QueryMain->Active || m_params.size() )
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

	if( !QueryMain->Active || m_params.size() )
		ButtonStartClick( Sender );

	EXPORT_XML_THREAD	*theThread = new EXPORT_XML_THREAD(
		QueryMain, destFile, m_tableName, m_recordName
	);
	theThread->StartThread( false );
}
//---------------------------------------------------------------------------



void __fastcall TsrxViewerForm::DatabaseMainLogin(TDatabase *,
	  TStrings *LoginParams)
{
	if( !m_userName.isEmpty() )
		LoginParams->Values["USER NAME"] = m_userName.c_str();
	if( !m_password.isEmpty() )
		LoginParams->Values["PASSWORD"] = m_password.c_str();
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
	m_theRoot = nullptr;

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

	if( int(m_colWidths.size()) == DBGridMain->Columns->Count )
	{
		std::size_t i=0;
		for(
			ArrayOfInts::const_iterator it = m_colWidths.cbegin(),
				endIT = m_colWidths.cend();
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

		m_colWidths.clear();
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
	if( m_theChart && Chart->SeriesCount() )
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
					QueryMain->FieldByName( value.c_str() )->AsFloat,
					QueryMain->FieldByName( label.c_str() )->AsString,
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
	m_userName = userName;
	m_password = password;
	m_theRoot = theRoot;

	DatabaseMain->AliasName = dbAlias.c_str();
	if( !userName.isEmpty() )
		DatabaseMain->Params->Values["USER NAME"] = userName.c_str();
	if( !password.isEmpty() )
		DatabaseMain->Params->Values["PASSWORD"] = password.c_str();
	ShowModal();
}
//---------------------------------------------------------------------------

