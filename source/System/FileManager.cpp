#include "FileManager.h"

/*
	Returns the number of folders in the selected directory

	@param path example: "C:\Users\user_name\Documents\"
*/
int System::FileManager::countFolders(const std::string path)
{
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((path + "\*").c_str(), &findFileData); // Add "\*" that to read all files in path
	int dirCount = 0; // Count of finded files

	if (hFind == INVALID_HANDLE_VALUE)
	{
		std::cerr << "[FileManager][Error] Filed to open directory!" << std::endl;
		return -1;
	}

	do 
	{
		// Check, is the item a folder
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{
			// Ignore the current and parent directories
			if (std::string(findFileData.cFileName) != "." && std::string(findFileData.cFileName) != "..") 
				dirCount++;
		}
	} 
	while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);
	return dirCount;
}

/* Returns the number of folders in the selected directory */
int System::FileManager::countFolders()
{
	return FileManager::countFolders(this->currentPath);
}