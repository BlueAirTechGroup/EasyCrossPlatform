#include "EasyCrossPlatform.h"
#include <iostream>
struct TestThreadPara {
	unsigned int WorkID;
};
void TaskJob(std::thread::id ThreadID, void * Parameters, bool * RunningSign, std::mutex *Mutex) {
	TestThreadPara &MyPara = *((TestThreadPara*)Parameters);
	while (*RunningSign) {
		std::cout << "Hi, Here's Subthread" << MyPara.WorkID << std::endl;
	}
	std::cout << "Thread Exited" << std::endl;
	return;
}
void TaskJob_MTask(std::thread::id ThreadID, void * Parameters, bool * RunningSign, std::mutex *Mutex) {
	TestThreadPara &MyPara = *((TestThreadPara*)Parameters);
	Mutex->lock();
	std::cout << "Hi, Here's Subthread" << MyPara.WorkID << std::endl;
	std::cout << "Thread Exited" << std::endl;
	Mutex->unlock();
	return;
}
class MyMultiTaskWork : public EasyCrossPlatform::Thread::SingleWorkCls {
public:
	void ThreadJob(std::thread::id & ThreadID, std::mutex* MyMutex = NULL, void* Parameters = NULL){
		while (this->getRunningStatus()) {
			std::cout << "Hi! Here is Cls for SingleWorkCls" << std::endl;
		}
		return;
	}
};
class MyMultiTaskWork_MTTask : public EasyCrossPlatform::Thread::SingleWorkCls {
public:
	void ThreadJob(std::thread::id & ThreadID, std::mutex* MyMutex = NULL, void* Parameters = NULL) {
		MyMutex->lock();
		std::cout << "Hi! Here is Cls for SingleWorkCls" << std::endl;
		MyMutex->unlock();
		return;
	}
};



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
int main_WorkerPool(int argc, char** args) {
	EasyCrossPlatform::Thread::SingleWork myOwnWork(TaskJob_MTask);
	EasyCrossPlatform::Thread::WorkPool myWorks(4);
	MyMultiTaskWork MyMultiWork;
	myWorks.Start();
	TestThreadPara MyPara[10];
	for (unsigned int i = 0; i < 10; i++) {
		MyPara[i].WorkID = i;
		myWorks.addWork(myOwnWork, &MyPara[i]);
	}
	for (unsigned int i = 0; i < 10; i++) {
		myWorks.addWork(MyMultiWork, NULL);
	}
	system("pause");
	myWorks.Stop();
	return 0;
}
//FileReadTest
int main_filetest(int argc, char** args) {
	//std::string MyFileRst = EasyCrossPlatform::File::FileIO::ReadPartFile("E:\\233.txt", 0, 3);
	//std::string MyFileRst = EasyCrossPlatform::File::FileIO::ReadPartFile("E:\\233.txt", -3, 3);
	std::string MyFileRst = EasyCrossPlatform::File::FileIO::ReadFile("E:\\233.txt");
	std::cout << "读取结果:" << MyFileRst << std::endl;
	system("pause");
	return 0;
}
//multitasktest_extendclass{
int main_MTEXT(int argc, char** args) {
	MyMultiTaskWork MT;
	std::cout << "MultiTaskStarts" << std::endl;
	std::string XSYD;
	MT.StartJob();
	system("pause");
	MT.StopJob();
	system("pause");
	return 0;
}


//Socket Test
int main_SocketTest(int argc, char** args) {
	EasyCrossPlatform::Network::TCPSocket MySock;
	MySock.Create();
	MySock.Bind(700);
	MySock.Listen();

	EasyCrossPlatform::Network::TCPSocket* ClientSock;
	char myBuffer[200] = "";
	int msgState;
	while (true) {
		msgState = -1;
		ClientSock = MySock.Accept();
		std::string MyStr = ClientSock->GetRemoteAddr();
		std::cout << "Connected:" << MyStr << std::endl;
		bool Connected = ClientSock->isConnected();
		while (msgState < 0) { //-1表示没有消息
			msgState = ClientSock->Receive(myBuffer, 200);
		}
		if (msgState == 0) {
			std::cout << "Client Connection Shutdown" << std::endl;
		}
		std::cout << myBuffer << std::endl;
		std::cout << strlen(myBuffer) << std::endl;
		ClientSock->Close();
		delete ClientSock;
	}
}
int main_1(int argc, char** args) {
	return 0;
}

class MyServer : public EasyCrossPlatform::Network::TCPAsyncSocket {
public:
	void ConnectedCallBack(EasyCrossPlatform::Network::TCPSocket &MySocket, std::mutex &ConnectionMutex) {
		std::cout << "Connected:" << MySocket.GetRemoteAddr() << "(" << MySocket.GetRemotePort() << ")" << std::endl;
	}
	void MsgCallBack(EasyCrossPlatform::Network::TCPSocket &MySocket, std::string Msg, std::mutex &MsgMutex) {
		std::cout << "Msg:" << MySocket.GetRemoteAddr() << "(" << MySocket.GetRemotePort() << ")[" << Msg << "]" << std::endl;
	}
	void DisconnectCallBack(EasyCrossPlatform::Network::TCPSocket &MySocket, std::mutex &ConnectionMutex) {
		std::cout << "Disconnected:" << MySocket.GetRemoteAddr() << "(" << MySocket.GetRemotePort() << ")" << std::endl;
	}
};
//AsyncServer Test
int main_AsyncServer(int argc, char** args){
	MyServer mServer;
	mServer.Listen(900);
	char Input[100];
	std::cout << "Press Any Key to Continue" << std::endl;
    std::cin >> Input;
    std::cout << "Bye!" << std::endl;
	mServer.StopListen();
	return 0;
}

//ChronoTest
int main(int argc, char** args) {
	//double nowTime = EasyCrossPlatform::Chrono::systemTime();
	double nowTime = EasyCrossPlatform::Chrono::accurateTime();
	std::cout << "Time:" << nowTime << std::endl;
	system("pause");
	//double newTime = EasyCrossPlatform::Chrono::systemTime();
	double newTime = EasyCrossPlatform::Chrono::accurateTime();
	std::cout << "Duration: " << (newTime - nowTime) << " s" << std::endl;
	std::cout << "Take: " << (2 * sizeof(double)) << "Byte Space" << std::endl;
	system("pause");
}