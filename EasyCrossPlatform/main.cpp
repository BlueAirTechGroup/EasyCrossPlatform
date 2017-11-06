#include "XSYDMultiTask.h"
#include "XSYDFileIO.h"
#include <iostream>
struct TestThreadPara {
	unsigned int WorkID;
};
void TaskJob(std::thread::id &ThreadID, void * Parameters, bool * RunningSign, std::mutex *Mutex) {
	TestThreadPara &MyPara = *((TestThreadPara*)Parameters);
	while (*RunningSign) {
		std::cout << "Hi, Here's Subthread" << MyPara.WorkID << std::endl;
	}
	std::cout << "Thread Exited" << std::endl;
	return;
}
void TaskJob_MTask(std::thread::id &ThreadID, void * Parameters, bool * RunningSign, std::mutex *Mutex) {
	TestThreadPara &MyPara = *((TestThreadPara*)Parameters);
	Mutex->lock();
	std::cout << "Hi, Here's Subthread" << MyPara.WorkID << std::endl;
	std::cout << "Thread Exited" << std::endl;
	Mutex->unlock();
	return;
}



//Sigle Thread Test 
int main_old(int argc, char** args) {
	EasyCrossPlatform::Thread::SingleWork myOwnWork(TaskJob);
	TestThreadPara MyPara;
	MyPara.WorkID = 0;
	myOwnWork.StartJob(NULL, &MyPara);
	std::cout << "Main Thread Here" << std::endl;
	system("pause");
	myOwnWork.StopJob();
	std::cout << "Thread Had been stopped" << std::endl;
	system("pause");
	return 0;
}

//Worker Pool Test
int main_old2(int argc, char** args) {
	EasyCrossPlatform::Thread::SingleWork myOwnWork(TaskJob_MTask);
	EasyCrossPlatform::Thread::WorkPool myWorks(4);

	TestThreadPara MyPara[10];
	for (unsigned int i = 0; i < 10; i++) {
		MyPara[i].WorkID = i;
		myWorks.addWork(myOwnWork, &MyPara[i]);
	}
	system("pause");
	return 0;
}
//FileReadTest
int main(int argc, char** args) {
	//std::string MyFileRst = EasyCrossPlatform::File::FileIO::ReadPartFile("E:\\233.txt", 0, 3);
	//std::string MyFileRst = EasyCrossPlatform::File::FileIO::ReadPartFile("E:\\233.txt", -3, 3);
	std::string MyFileRst = EasyCrossPlatform::File::FileIO::ReadFile("E:\\233.txt");
	std::cout << "¶ÁÈ¡½á¹û:" << MyFileRst << std::endl;
	system("pause");
	return 0;
}