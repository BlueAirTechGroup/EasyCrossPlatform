#include "XSYDMultiTask.h"

int EasyCrossPlatform::Thread::SingleWork::DoingJob(EasyCrossPlatform::Thread::WorkInfo MyInfo)
{
	MyInfo.MyWork(std::this_thread::get_id(), MyInfo.Parameters, MyInfo.RunningSign, MyInfo.mMutex);
	*(MyInfo.RunningSign) = false;
	return 0;
}

EasyCrossPlatform::Thread::SingleWork::SingleWork(EasyCrossPlatform::Thread::SpecificWorkPtr mWork)
{
	this->MyWork = mWork;
	this->mThread = NULL;
	this->RunningSign = false;
}

bool EasyCrossPlatform::Thread::SingleWork::StartJob(std::mutex *MyMutex, void * Parameter)
{
	if (this->RunningSign) { return false; }
	if (this->mThread != NULL) {
		if (this->mThread->joinable()) {
			this->mThread->join();
		}
		delete this->mThread;
		this->mThread = NULL;
	}
	WorkInfo MyInfo;
	MyInfo.mMutex = MyMutex;
	MyInfo.MyWork = this->MyWork;
	MyInfo.Parameters = Parameter;
	MyInfo.RunningSign = &(this->RunningSign);
	this->RunningSign = true;
	this->mThread = new std::thread(EasyCrossPlatform::Thread::SingleWork::DoingJob, MyInfo);
	return true;
}

void EasyCrossPlatform::Thread::SingleWork::StopJob()
{
	this->RunningSign = false;
	return;
}

bool EasyCrossPlatform::Thread::SingleWork::getRunningStatus()
{
	return this->RunningSign;
}

EasyCrossPlatform::Thread::SingleWork::~SingleWork()
{
	this->RunningSign = false;
	if (this->mThread != NULL) {
		if (this->mThread->joinable()) {
			this->mThread->join();
		}
		delete this->mThread;
		this->mThread = NULL;
	}
}

void EasyCrossPlatform::Thread::WorkPool::SuperviseThreads(std::thread::id & ThreadID, void * Parameters, bool * RunningSign, std::mutex * Mutex)
{
	//��ϢԤ����
	WorksInfo MyInfo = *((WorksInfo *)Parameters);
	std::deque<WorkerInfo> &MyWork = *(MyInfo.CurrentWorksAddr);
	std::deque<WorkerInfo> &PendingWork = *(MyInfo.PendingWorksAddr);
	unsigned int MaxThreadNum = MyInfo.MaxThread;
	std::mutex* LineMutex = MyInfo.LineMutex;
	delete Parameters;
	//��ϢԤ�������

	WorkerInfo TmpWorkerInfo;
	while ((*RunningSign)) {
		if (!MyWork.empty()) {
			for (auto i = MyWork.begin(); i != MyWork.end(); i++) {
				//����������, ���Զ�ɾ��, �ճ��̳߳ؿռ�
				if ((*i).wInfo->getRunningStatus() == false) {
					delete (*i).wInfo;
					i = MyWork.erase(i);
					if (i == MyWork.end()) {
						break;
					}
				}
			}
		}
		//�������ִ�е��߳������ڹ涨���߳���
		if (MyWork.size() < MaxThreadNum && PendingWork.size() > 0) {
			LineMutex->lock();
			TmpWorkerInfo = PendingWork.front();
			PendingWork.pop_front();
			LineMutex->unlock();
			TmpWorkerInfo.wInfo->StartJob(MyInfo.SharedMutex, TmpWorkerInfo.wParameters);
			MyWork.push_back(TmpWorkerInfo);
		}
	}
	//��Running��,��������
	if (!MyWork.empty()) {
		for (auto i = MyWork.begin(); i != MyWork.end(); i++) {
			(*i).wInfo->StopJob();
			delete (*i).wInfo;
			i = MyWork.erase(i);
			if (i == MyWork.end()) {
				break;
			}
		}
	}
	if (!PendingWork.empty()) {
		for (auto i = PendingWork.begin(); i != PendingWork.end(); i++) {
			delete (*i).wInfo;
			i = MyWork.erase(i);
			if (i == MyWork.end()) {
				break;
			}
		}
	}
	return;
}

EasyCrossPlatform::Thread::WorkPool::WorkPool(const unsigned int ThreadNum)
{
	this->MaxThread = ThreadNum;
	this->SupervisingThread = new EasyCrossPlatform::Thread::SingleWork(this->SuperviseThreads);
	WorksInfo* MyInfo = new WorksInfo;
	MyInfo->CurrentWorksAddr = &(this->CurrentWorks);
	MyInfo->PendingWorksAddr = &(this->PendingWorks);
	MyInfo->MaxThread = this->MaxThread;
	MyInfo->SharedMutex = &this->MyMutex;
	MyInfo->LineMutex = &this->LineMutex;
	this->SupervisingThread->StartJob(NULL,(void*) MyInfo);
}

EasyCrossPlatform::Thread::WorkPool::~WorkPool()
{
	this->SupervisingThread->StopJob();
	delete this->SupervisingThread;
}

void EasyCrossPlatform::Thread::WorkPool::addWork(SingleWork &MyWork, void* Parameter)
{
	SingleWork* TmpWork = new SingleWork(MyWork);
	WorkerInfo MInfo;
	MInfo.wInfo = TmpWork;
	MInfo.wParameters = Parameter;
	this->LineMutex.lock();
	this->PendingWorks.push_back(MInfo);
	this->LineMutex.unlock();
}

void EasyCrossPlatform::Thread::WorkPool::addWork_AtFront(SingleWork &MyWork, void* Parameter)
{
	SingleWork* TmpWork = new SingleWork(MyWork);
	WorkerInfo MInfo;
	MInfo.wInfo = TmpWork;
	MInfo.wParameters = Parameter;
	this->LineMutex.lock();
	this->PendingWorks.push_front(MInfo);
	this->LineMutex.unlock();
}

int EasyCrossPlatform::Thread::SingleWorkCls::DoingJob(SingleWorkCls * ClassPtr)
{
	ClassPtr->ThreadJob(std::this_thread::get_id());
	ClassPtr->RunningSign = false;
	return 0;
}

EasyCrossPlatform::Thread::SingleWorkCls::SingleWorkCls()
{
	this->RunningSign = false;
}

bool EasyCrossPlatform::Thread::SingleWorkCls::StartJob()
{
	if (this->RunningSign) {
		return false;
	}
	this->RunningSign = true;
	this->mThread = new std::thread(EasyCrossPlatform::Thread::SingleWorkCls::DoingJob, this);
	return true;
}

void EasyCrossPlatform::Thread::SingleWorkCls::StopJob()
{
	
	if (this->mThread != NULL) {
		if (this->RunningSign) {
			this->RunningSign = false;
			if (this->mThread->joinable()) {
				this->mThread->join();
			}
		}
	}
	if (this->mThread != NULL) {
		delete this->mThread;
		this->mThread = NULL;
	}
	return;
}

bool EasyCrossPlatform::Thread::SingleWorkCls::getRunningStatus()
{
	return this->RunningSign;
}

void EasyCrossPlatform::Thread::SingleWorkCls::ThreadJob(std::thread::id & ThreadID)
{
	return;
}

EasyCrossPlatform::Thread::SingleWorkCls::~SingleWorkCls()
{
	this->RunningSign = false;
	if (this->mThread != NULL) {
		if (this->mThread->joinable()) {
			this->mThread->join();
		}
		delete this->mThread;
		this->mThread = NULL;
	}
}
