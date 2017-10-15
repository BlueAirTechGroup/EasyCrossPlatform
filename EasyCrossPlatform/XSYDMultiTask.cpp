#include "XSYDMultiTask.h"
#include <iostream>
using namespace EasyCrossPlatform::Thread;
int EasyCrossPlatform::Thread::DoMultiThread(pendingThreadInfo PendingInfo, bool *isRunningSign, void* myMutex) {
	PendingInfo.TasktoExec(PendingInfo.Unique_ID,PendingInfo.Parameter,isRunningSign,*((std::mutex*) myMutex));
	PendingInfo.CallBack(PendingInfo.Unique_ID,*((std::mutex*) myMutex));
	return 0;
}
int EasyCrossPlatform::Thread::SuperviseThreads(void* Infos, void* pendingInfos, bool* isSupervising, unsigned int MaxWorkerNum, bool* isRunning, void* myMutex) {
	std::mutex* customMutex = (std::mutex*) myMutex;
	std::deque<executingThreadInfo>* executingInfo = (std::deque<executingThreadInfo>*) (Infos);
	std::deque<pendingThreadInfo>* pendingInfo = (std::deque<pendingThreadInfo>*) (pendingInfos);

	std::thread newThread;
	executingThreadInfo newThreadExecInfo;
	pendingThreadInfo newTheadPendingInfo;
	while ((*isSupervising)) {
		if (!(executingInfo->empty())) {
			for (auto i = executingInfo->begin();i != executingInfo->end();++i) {
				if (i->exited){
					i->MyThread->~thread();
					delete i->MyThread;
					i = executingInfo->erase(i);
					if (i == executingInfo->end()) {
						break;
					}
				}
			}
		}
		if (executingInfo->size() < MaxWorkerNum && !(pendingInfo->empty())) {
			newTheadPendingInfo = pendingInfo->front();
			newThreadExecInfo.MyThread = new std::thread(DoMultiThread, newTheadPendingInfo, isRunning, customMutex);
			newThreadExecInfo.MyThread->detach();
			newThreadExecInfo.Unique_ID = newTheadPendingInfo.Unique_ID;
			newThreadExecInfo.exited = false;
			executingInfo->push_back(newThreadExecInfo);
			pendingInfo->pop_front();
		}
	}
	return 0;
}
EasyCrossPlatform::Thread::ThreadManager::ThreadManager(unsigned int WorkerNum)
{
	this->isRunning = true;
	this->isSupervising = true;
	this->MaxUID = 0;
	if (WorkerNum > 1) {
		this->MaxWorkNum = WorkerNum;
	}
	else{
		this->MaxWorkNum = 1;
	}
	this->supervisorThead = new std::thread(SuperviseThreads, (void*)&this->_runningThreads, &this->_waitThreads, &this->isSupervising, this->MaxWorkNum, &this->isRunning, (void*) &this->MMutex);
	//this->supervisorThead->detach();
}

unsigned int EasyCrossPlatform::Thread::ThreadManager::addThread(ThreadTask MyTask, void * ThreadParameter, ThreadCallBack CallBack)
{
	if (this->isRunning) {
		unsigned int newThreadUID;
		if (MyTask != NULL && CallBack != NULL) {
			newThreadUID = ++(this->MaxUID);
			pendingThreadInfo newThreadInfo;
			newThreadInfo.TasktoExec = MyTask;
			newThreadInfo.Parameter = ThreadParameter;
			newThreadInfo.CallBack = CallBack;
			newThreadInfo.Unique_ID = newThreadUID;
			this->_waitThreads.push_back(newThreadInfo);
		}
		else {
			newThreadUID = 0;
			throw std::invalid_argument("Your Callback Function and the Main Text cannot be point to NULL");
		}
		return newThreadUID;
	}
	else {
		throw std::runtime_error("This Thread Pool is no longer available after cleanAllThread()");
	}
}

bool EasyCrossPlatform::Thread::ThreadManager::deleteThread(unsigned int UniqueID, bool ThreadFinishCall)
{
	if (this->isRunning) {
		if (!ThreadFinishCall) { //ThreadFinishCall只会发生在RunningThreads中
			for (auto i = this->_waitThreads.begin(); i != this->_waitThreads.end();i++) {
				if ((*i).Unique_ID == UniqueID) {
					i = this->_waitThreads.erase(i);
					return true;
				}
			}
		}
		for (auto i = this->_runningThreads.begin();i != this->_runningThreads.end();i++) {
			if ((*i).Unique_ID == UniqueID) {
				(*i).exited = true;
				return true;
			}
		}
	}else{
		throw std::runtime_error("This Thread Pool is no longer available after cleanAllThread()");
	}
	return false;
}

void EasyCrossPlatform::Thread::ThreadManager::cleanAllThread()
{
	this->isRunning = false;
	this->isSupervising = false;
	this->supervisorThead->join();
	this->_waitThreads.clear();
	/* for (executingThreadInfo i : this->_runningThreads) {
		i.MyThread->~thread(); //销毁
		delete i.MyThread; //释放内存
	}*/
	//this->cleanlock.lock(); //等待完毕
	this->_runningThreads.clear();
	this->MaxUID = 0;
}


EasyCrossPlatform::Thread::ThreadManager::~ThreadManager()
{
	this->cleanAllThread();
	delete this->supervisorThead;
}
