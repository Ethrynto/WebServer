#pragma once
#include <iostream>
#include <windows.h>
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

namespace System
{
	class FileManager
	{
	public:

		FileManager()
		{	}
		FileManager(std::string path) : currentPath(path)
		{	}
		~FileManager()
		{	}

		/*
			Returns the number of folders in the selected directory.

			@param path example: "C:\Users\user_name\Documents\"
		*/
		int static countFolders(const std::string path);
		int countFolders();

		/*
			Return the number of (.fileType) in the selected directory.

			@param path example: "C:\Users\user_name\Documents\"
			@param fileType example: ".txt"
		*/
		int static countFiles(const std::string path, std::string fileType);
		int countFiles(std::string fileType);

		/*
			Return the number all files and folders in the selected directory.

			@param path example: "C:\Users\user_name\Documents\"
		*/
		int static countAll(const std::string path);
		int countAll();
	private:
		std::string currentPath;
	};

}
#endif

