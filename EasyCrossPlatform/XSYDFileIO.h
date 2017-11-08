#ifndef __XSYDFileIOFile__
	#define __XSYDFileIOFile__
	#include "EasyCP_Common.h"
	#include <stdio.h>
	namespace EasyCrossPlatform {
		namespace File {
			class FileIO {
			private:
				static FILE* OpenFile(const char* Path, const char * Mode);
			public:
				static std::string ReadPartFile(const std::string &Path, const long StartPos, const unsigned long ReadLength);
				static std::string ReadPartFile(const char* Path, const long StartPos, const unsigned long ReadLength);
				static std::string ReadFile(const std::string &Path);
				static std::string ReadFile(const char* Path);
				static bool WriteFile_ReplaceAt(long BytesMoved, const std::string &Path, const std::string &Content);
				static bool WriteFile_ReplaceAt(long BytesMoved, const char * Path, const std::string &Content);
				static bool WriteFile(const std::string &Path, const std::string &Content);
				static bool WriteFile(const char* Path, const std::string &Content);
				static bool WriteFile_Append(const std::string &Path, const std::string &Content);
				static bool WriteFile_Append(const char* Path, const std::string &Content);
				static unsigned long GetFileLength(const std::string &Path);
				static unsigned long GetFileLength(const char* Path);
			};
		}
	}
#endif