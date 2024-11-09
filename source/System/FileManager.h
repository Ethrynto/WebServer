#pragma once
#include <iostream>
#include <vector>
#include <map>
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
			Returns the vector of folder names in the selected directory.

			@param path example: "C:\Users\user_name\Documents\"
		*/
		std::vector<std::string> static getFolders(const std::string& path);
		std::vector<std::string> getFolders();


		/**
			Returns the number of folders in the selected directory.
			@param path the directory path
			@return the folders number
		*/
		int static countFolders(const std::string& path);
		int countFolders();

		/**
			Return the number of (.fileType) in the selected directory.
			@param path the directory path
			@param fileType the type of file
			@return the files number
		*/
		int static countFiles(const std::string& path, std::string fileType);
		int countFiles(std::string fileType);

		/**
			Return the number all files and folders in the selected directory.
			@param path the directory path
		*/
		int static countAll(const std::string& path);
		int countAll();

	private:
		std::string currentPath;
		std::vector<std::string> currentFolders;
		unsigned int currentFoldersNumber;
		unsigned int currentAllFilesNumber;
	};

}
#endif

