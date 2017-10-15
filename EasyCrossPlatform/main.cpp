#include "XSYDMultiTask.h"
#include <iostream>

EasyCrossPlatform::Thread::ThreadManager MyManager(2);

void MT(unsigned int &UniqueID,void* parameter, bool* isRunning, std::mutex &myMutex) {
	int Num = *((int*) parameter);
	myMutex.lock(); //对输出区域上锁
	std::cout << "subthread started(" << Num << ")[" << UniqueID << "]" << std::endl;
	for (int i = 0;i < 3;i++) {
		std::cout << "Hi, I am the subthread(" << Num << ")[" << UniqueID << "]" << std::endl;
	}
	myMutex.unlock();
	return;
}
void CallBack(unsigned int &UniqueID, std::mutex &myMutex) {
	myMutex.lock();
	std::cout << "thread exit[" << UniqueID << "]" << std::endl;
	myMutex.unlock();
	MyManager.deleteThread(UniqueID);
}
int main(int argc, char** args) {
	int a[]{ 1,2 };
	for (int i = 0;i < 2;i++) {
		unsigned int UID = MyManager.addThread(MT, &a[i], CallBack);
		//std::cout << "UID:" << UID << std::endl;
	}
	system("pause");
	return 0;
}