#include "InputFilePath.hpp"

int InputFilePath::initial(bool flag = true)
{
	printf("%5s Check %5s in \"%20s\" %5s\n ", std::string(5, '=').c_str(), fileType.c_str(), inputDir.c_str(), std::string(5, '=').c_str());
	if (flag)
		printf("%4s CHECK SUB DIRECTORY FUNCTION IS OPEN!! %4s\n", std::string(4, '*').c_str(), std::string(4, '*').c_str());
	else{
		printf("%4s CHECK SUB DIRECTORY FUNCTION IS CLOSED!! %4s\n", std::string(4, '*').c_str(), std::string(4, '*').c_str());
		CHECKSUBDIR = false;
	}

	checkInPutDir(inputDir);
	if (!getFileInfo(inputDir))
	{
		std::cout << "Check Files in: \"" << inputDir << "\" Succeed. And fileType is: \"" << fileType << "\"." << std::endl;
	}else{
		std::cout << "Check Files in: \"" << inputDir << "\" Failed. And fileType is: \"" << fileType << "\"." << std::endl;
		return 1;
	}
	
	if (fileName.empty() || filePath.empty())
	{
		std::cerr << "No Files Found in \"" << inputDir << "\"." << std::endl;
		return 1;
	}else{
		printf("%4s Search File Complete, Total %5d Found. %4s\n", std::string(4, '=').c_str(), fileName.size(), std::string(4, '=').c_str());
		return 0;
	}
}

int InputFilePath::getFileInfo(const std::string &currentDir)
{
	if (currentDir.empty())
	{
		std::cerr << "Initial inputDir Failed, Input Path is empty." << std::endl;
		return 1;
	}else if (_access(currentDir.c_str(), 4)){
		std::cerr << "Initial inputDir Failed, Input Path can not be accessed." << std::endl;
		return 2;
	}else{
		std::cout << "Initial inputDir, Input Path: \"" << currentDir << "\"." << std::endl;
	}

	struct _finddata_t fileInfo;
	// _finddata_t struct:
	// unsigned		attrib
	// time_t		time_create
	// time_t		time_access
	// time_t		time_write
	// _fsize_t		size;
	// char			name[260]
	// 
	std::string fp, fn;

	intptr_t h;

	if ((h = _findfirst(fp.assign(currentDir).append("*").c_str(), &fileInfo)) != -1)
	{
		do
		{
			if (fileInfo.attrib & _A_SUBDIR)
			{
				if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0)
				{
					if (CHECKSUBDIR)
						getFileInfo(fp.assign(fileInfo.name).append("/"));
				}
			}else{
				fn.assign(fileInfo.name);
				std::string::size_type sz = fn.size();
				std::string::size_type pos = fn.rfind('.', std::string::npos);

				if (pos != std::string::npos)
				{
					if (fn.compare(pos + 1, sz - pos - 1, fileType) == 0)
					{
						filePath.insert(std::make_pair(fn.substr(0, pos), fp.assign(currentDir).append(fn)));
						fileName.push_back(fn.substr(0, pos));
					}
				}
			}
		} while (_findnext(h, &fileInfo) == 0);
		_findclose(h);
	}
	return 0;
}

void checkInPutDir(std::string &filePath)
{
	if (!filePath.empty())
	{
		for (auto &c : filePath)
			if (c == '\\')
				c = '/';\

		if (filePath[filePath.size() - 1] != '/')
			filePath.append("/");
	}	
}