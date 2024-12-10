//---------------------------------------------------------------------------

#include <windows.h>

#include <vcl.h>

#include <winlib/f_type.h>

#pragma hdrstop
USERES("srxDesigner.res");
USEFORM("srxDesignerFrm.cpp", srxDesignerForm);
USEFORM("srxViewerFrm.cpp", srxViewerForm);
USELIB("..\..\Object\gaklib\gaklib_bcb.lib");
USEFORM("..\GAKLIB\Repository\StatusFrm.cpp", StatusForm);
USELIB("..\..\Object\winlib\winlibBCB.lib");
//---------------------------------------------------------------------------
static void registerFiles( void )
{
	winlib::FileTypeRegistry	fType;
	char						exeFile[10240];

	GetModuleFileName( NULL, exeFile, sizeof(exeFile) );

	fType.extension = ".SRXD";
	fType.type = "SRXD_file";
	fType.type_description = "SRX Report Designer File";
	fType.cmd = "open",
	fType.cmd_description = "&Open";
	fType.commandLine = exeFile;
	fType.commandLine += " %1";
	addFileType( &fType );

	fType.extension = ".SRX";
	fType.type = "SRX_file";
	fType.type_description = "SRX Report Viewer File";
	addFileType( &fType );
}
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR cmdLine, int)
{
	registerFiles();

	if( *cmdLine )
		Application->Tag = (int)cmdLine;

	try
	{
		Application->Initialize();
		Application->Title = "SRX Report Designer";
		Application->CreateForm(__classid(TsrxDesignerForm), &srxDesignerForm);
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
