#include "XSYDFileIO.h"
#if defined(EASYCROSSPLATFORM_PLATFORM_WINDOWS)
FILE * EasyCrossPlatform::File::FileIO::OpenFile(const char * Path, const char * Mode)
{
	FILE *myfile;
	errno_t opennum = fopen_s(&myfile, Path, Mode);
	if (opennum != 0) {
		//文件不存在, 或无权限获取
		return NULL;
	}
	return myfile;
}
#elif defined(EASYCROSSPLATFORM_PLATFORM_LINUX)
FILE * EasyCrossPlatform::File::FileIO::OpenFile(const char * Path, const char * Mode)
{
	FILE *myfile;
	myfile = fopen(&myfile, Path, Mode);
	return myfile;
}
#endif
std::string EasyCrossPlatform::File::FileIO::ReadPartFile(const std::string & Path, long StartPos, const long ReadLength)
{
	return EasyCrossPlatform::File::FileIO::ReadPartFile(Path.c_str(), StartPos, ReadLength);
}
std::string EasyCrossPlatform::File::FileIO::ReadPartFile(const char * Path, long StartPos, const long ReadLength)
{
	std::string myTempRst;
	FILE *myfile;
	myfile = OpenFile(Path, "r");
	if (myfile==NULL) {
		//文件不存在, 或无权限获取
		throw std::runtime_error("File cannot be opened");
		return myTempRst;
	}
	if (StartPos > 0) {
		fseek(myfile, StartPos, SEEK_SET);
	}
	else if(StartPos < 0) {
		fseek(myfile, StartPos, SEEK_END);
	}
	char* MyTemp = new char[ReadLength + 1];
	for (unsigned long i = 0; i < ReadLength; i++) {
		MyTemp[i] = fgetc(myfile);
	}
	MyTemp[ReadLength] = '\0';
	myTempRst = MyTemp;
	delete[] MyTemp;
	fclose(myfile);
	return myTempRst;
}

std::string EasyCrossPlatform::File::FileIO::ReadFile(const std::string & Path)
{
	return EasyCrossPlatform::File::FileIO::ReadFile(Path.c_str());
}

std::string EasyCrossPlatform::File::FileIO::ReadFile(const char * Path)
{
	std::string myTempRst;
	FILE *myfile;

	myfile = OpenFile(Path, "r");
	if (myfile == NULL) {
		//文件不存在, 或无权限获取
		throw std::runtime_error("File cannot be opened");
		return myTempRst;
	}
	fseek(myfile, 0, SEEK_END);//前往最后一个字符
	unsigned long FileLength = ftell(myfile);
	//fseek(myfile, 0, SEEK_SET); //返回文件头
	rewind(myfile); //回到文件头
	char* MyTemp = new char[FileLength+1];
	for (unsigned long i = 0; i < FileLength; i++) {
		MyTemp[i] = fgetc(myfile);
	}
	MyTemp[FileLength] = '\0';
	myTempRst = MyTemp;
	delete[] MyTemp;
	fclose(myfile);
	return myTempRst;
}
bool EasyCrossPlatform::File::FileIO::WriteFile_ReplaceAt(long BytesMoved, const std::string & Path, const std::string & Content)
{
	return EasyCrossPlatform::File::FileIO::WriteFile_ReplaceAt(BytesMoved,Path.c_str(),Content);
}

bool EasyCrossPlatform::File::FileIO::WriteFile_ReplaceAt(long BytesMoved, const char * Path, const std::string & Content)
{
	FILE* myfile;
	myfile = OpenFile(Path, "w");
	if (myfile == NULL) {
		throw std::runtime_error("File cannot be opened");
		return false;
	}
	if (BytesMoved > 0) {
		fseek(myfile, BytesMoved, SEEK_SET);
	}
	else if (BytesMoved < 0) {
		fseek(myfile, BytesMoved, SEEK_END);
	}
	for (auto i = Content.begin(); i != Content.end(); i++) {
		fputc((*i), myfile);
	}
	fclose(myfile);
	return true;
}

bool EasyCrossPlatform::File::FileIO::WriteFile(const std::string & Path, const std::string & Content)
{
	return EasyCrossPlatform::File::FileIO::WriteFile(Path.c_str(),Content);
}


bool EasyCrossPlatform::File::FileIO::WriteFile(const char * Path, const std::string & Content)
{
	return EasyCrossPlatform::File::FileIO::WriteFile_ReplaceAt(0,Path,Content);
}

bool EasyCrossPlatform::File::FileIO::WriteFile_Append(const std::string & Path, const std::string & Content)
{
	return EasyCrossPlatform::File::FileIO::WriteFile_Append(Path.c_str(), Content);
}

bool EasyCrossPlatform::File::FileIO::WriteFile_Append(const char * Path, const std::string & Content)
{
	FILE* myfile;
	myfile = OpenFile(Path, "a");
	if (myfile == NULL) {
		throw std::runtime_error("File cannot be opened");
		return false;
	}
	for (auto i = Content.begin(); i != Content.end(); i++) {
		fputc((*i), myfile);
	}
	fclose(myfile);
	return true;
}



unsigned long EasyCrossPlatform::File::FileIO::GetFileLength(const std::string & Path)
{
	
	return EasyCrossPlatform::File::FileIO::GetFileLength(Path.c_str());
}

unsigned long EasyCrossPlatform::File::FileIO::GetFileLength(const char * Path)
{
	FILE* myfile;
	myfile = OpenFile(Path, "r");
	if (myfile == NULL) {
		throw std::runtime_error("File cannot be opened");
		return false;
	}
	fseek(myfile, 0, SEEK_END);
	long MyLength = ftell(myfile);
	fclose(myfile);
	return MyLength;
}
