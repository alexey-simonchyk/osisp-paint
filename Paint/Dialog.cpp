#include <windows.h>

class Dialog
{
private:
	OPENFILENAME fileNameStructure;
public:

	void init(char *szFileName)
	{
		ZeroMemory(&fileNameStructure, sizeof(fileNameStructure));
		fileNameStructure.lStructSize = sizeof(fileNameStructure);
		fileNameStructure.hwndOwner = NULL;
		fileNameStructure.lpstrFilter = (LPCWSTR)L"EMF files (*.emf)\0*.emf";
		fileNameStructure.lpstrFile = (LPWSTR)szFileName;
		fileNameStructure.nMaxFile = MAX_PATH;
		fileNameStructure.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		fileNameStructure.lpstrDefExt = (LPCWSTR)L"emf";
	}

	LPWSTR OpenFileDialog(char* szFileName)
	{
		init(szFileName);
		if (GetOpenFileName(&fileNameStructure) == TRUE)
		{
		fileNameStructure.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			return fileNameStructure.lpstrFile;
		}
		else
		{
			return NULL;
		}
	}

	LPWSTR OpenSaveDialog(char* szFileName)
	{
		init(szFileName);
		if (GetSaveFileName(&fileNameStructure) == TRUE)
		{
			return fileNameStructure.lpstrFile;

		}
		else
		{
			return NULL;
		}
	}
};