#include "findVideoPath.hpp"

inputVideoPath::inputVideoPath(const std::string s, const std::string t) : videoDir(s), fileType(t)
{
	checkinPutPath(videoDir);
	if (!getVideoInfo(videoDir))
	{
		std::cout << "Read Video File in: \"" << videoDir << "\" Succeed. And fileType is: \"" << fileType << "\"." << std::endl;
	}else{
		std::cout << "Read Video File in: \"" << videoDir << "\" Failed. And fileType is: \"" << fileType << "\"." << std::endl;
	}
	
}

int getVideoInfo(const std::string &currentDir)
{
	if (currentDir.empty())
	{
		std::cout << "Initial VideoDir Failed, Input Path is empty." << std::endl;
		return 1;
	}else{
		std::cout << "Initial VideoDir, Input Path: \"" << currentDir << "\"." << std::endl;
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
					getVideoInfo(fp.assign(fileInfo.name).append('/'));
				}
			}else{
				fn.assign(fileInfo.name);
				std::size_t sz = fn.size();
				std::size_t pos = fn.rfind('.', std::string::npos);

				if (pos != std::string::npos)
				{
					if (fn.compare(pos, sz - pos, fileType) == 0)
					{
						videoPath.insert(std::make_pair(fn.substr(0, pos), fp.assign(currentDir).append(fn)));
						videoName.push_back(fn.substr(0, pos));
					}
				}
			}
		} while (_findnext(h, &fileInfo) == 0);
	}
	_findclose(h);

	if (videoName.empty() || videoPath.empty())
		return 0;
	else
		return 1;
}

void checkinPutPath(std::string &inputPath)
{
	int pos = -1;
	std::size_t length = inputPath.size();
	for (auto c : inputPath)
	{
		++pos;
		if (c == '\\' && pos < length)
		{
			inputPath[pos] = '/';
		}
	}
}