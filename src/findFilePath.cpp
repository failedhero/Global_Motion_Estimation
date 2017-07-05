#include "findFilePath.h"

inputFilePath::inputFilePath(const std::string s, const std::string t) : inputDir(s), fileType(t)
{
	checkInPutDir(inputDir);
	if (!getFileInfo(inputDir))
	{
		std::cout << "Check Files in: \"" << inputDir << "\" Succeed. And fileType is: \"" << fileType << "\"." << std::endl;
	}else{
		std::cout << "Check Files in: \"" << inputDir << "\" Failed. And fileType is: \"" << fileType << "\"." << std::endl;
	}
	
}

int getFileInfo(const std::string &currentDir)
{
	if (currentDir.empty())
	{
		std::cout << "Initial inputDir Failed, Input Path is empty." << std::endl;
		return 1;
	}else{
		std::cout << "Initial inputDir, Input Path: \"" << currentDir << "\"." << std::endl;
	}

	struct _finddata_t fileInfo;
	std::string fp, fn;

	long h = 0;

	if ((h = _findfirst(fp.assign(currentDir).append("*").c_str(), &fileInfo)) != -1)
	{
		do
		{
			if (fileInfo.attrib & _A_SUBDIR)
			{
				if (strcmp(fileInfo.name, '.') != 0 && strcmp(fileInfo.name, '..') != 0)
				{
					getFileInfo(fp.assign(fileInfo.name).append('/'));
				}
			}else{
				fn.assign(fileInfo.name);
				std::size_t sz = fn.size();
				std::size_t pos = fn.rfind('.', std::string::npos);

				if (pos != std::string::npos)
				{
					if (fn.compare(pos, sz - pos, fileType) == 0)
					{
						filePath.insert(std::make_pair(fn.substr(0, pos), fp.assign(currentDir).append(fn)));
						fileName.push_back(fn.substr(0, pos));
					}
				}
			}
		} while (_findnext(h, &fileInfo) == 0);
	}
	_findclose(h);

	if (fileName.empty() || filePath.empty())
		return 0;
	else
		return 1;
}

void checkInPutDir(std::string &filePath)
{
	int pos = -1;
	std::size_t length = filePath.size();
	for (auto c : filePath)
	{
		++pos;
		if (c == '\\' && pos < length)
		{
			filePath[pos] = '/';
		}
	}
}