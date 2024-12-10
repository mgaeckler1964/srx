//---------------------------------------------------------------------------

#include <vcl.h>

#include <gak/string.h>

#pragma hdrstop
USERES("srxViewer.res");
USEFORM("srxViewerFrm.cpp", srxViewerForm);
USELIB("..\..\Object\gaklib\gaklib_bcb.lib");
USEFORM("..\GAKLIB\Repository\StatusFrm.cpp", StatusForm);
USELIB("..\..\Object\winlib\winlibBCB.lib");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR cmdLine, int)
{
	if( *cmdLine )
		Application->Tag = (int)cmdLine;
#ifdef _DEBUG
	else
		Application->Tag = (int)"KASSE \"..\\source\\kasse\\reports\\Kontenauszug.srxd\"";
#endif

	if( !Application->Tag )
	{
		Application->MessageBox(
			"There is no SQL-Report defined",
			"Error",
			MB_OK|MB_ICONERROR
		);
		exit( -1 );
	}
	try
	{
		Application->Initialize();
		Application->Title = "SRX Report Viewer";
		Application->CreateForm(__classid(TsrxViewerForm), &srxViewerForm);
		Application->CreateForm(__classid(TStatusForm), &StatusForm);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------




