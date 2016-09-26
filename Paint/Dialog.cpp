#include <windows.h>

class Dialog
{
private:
	OPENFILENAME fileNameStructure;
public:
	Dialog()
	{
		char szFileName[MAX_PATH] = "";
		ZeroMemory(&fileNameStructure, sizeof(fileNameStructure));
		fileNameStructure.lStructSize = sizeof(fileNameStructure);
		fileNameStructure.hwndOwner = NULL;
		fileNameStructure.lpstrFilter = (LPCWSTR)L"EMF files (*.emf)\0*.emf";
		fileNameStructure.lpstrFile = (LPWSTR)szFileName;
		fileNameStructure.nMaxFile = MAX_PATH;
		fileNameStructure.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		fileNameStructure.lpstrDefExt = (LPCWSTR)L"emf";
	}

	LPWSTR OpenFileDialog(bool *isSelected)
	{
		if (GetOpenFileName(&fileNameStructure) == TRUE)
		{
			*isSelected = true;
			return fileNameStructure.lpstrFile;
		}
		else
		{
			*isSelected = false;
		}
	}

	LPWSTR OpenSaveDialog(bool *isSelected)
	{
		if (GetSaveFileName(&fileNameStructure) == TRUE)
		{
			*isSelected = true;
			return fileNameStructure.lpstrFile;

		}
		else
		{
			*isSelected = false;
		}
	}
};