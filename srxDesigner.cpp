/*
		Project:		SRX
		Module:			srxDesigner.cpp
		Description:	The report designer
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
