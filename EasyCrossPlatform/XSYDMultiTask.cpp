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
	//信息预处理
	WorksInfo MyInfo = *((WorksInfo *)Parameters);
	std::deque<WorkerInfo> &MyWork = *(MyInfo.CurrentWorksAddr);
	std::deque<WorkerInfo> &PendingWork = *(MyInfo.PendingWorksAddr);
	unsigned int MaxThreadNum = MyInfo.MaxThread;
	std::mutex* LineMutex = MyInfo.LineMutex;
	delete Parameters;
	//信息预处理完毕

	WorkerInfo TmpWorkerInfo;
	while ((*RunningSign)) {
		if (!MyWork.empty()) {
			for (auto i = MyWork.begin(); i != MyWork.end(); i++) {
				//如果运行完毕, 则自动删除, 空出线程池空间
				if ((*i).wInfo->getRunningStatus() == false) {
					delete (*i).wInfo;
					i = MyWork.erase(i);
					if (i == MyWork.end()) {
						break;
					}
				}
			}
		}
		//如果正在执行的线程数少于规定的线程数
		if (MyWork.size() < MaxThreadNum && PendingWork.size() > 0) {
			LineMutex->lock();
			TmpWorkerInfo = PendingWork.front();
			PendingWork.pop_front();
			LineMutex->unlock();
			TmpWorkerInfo.wInfo->StartJob(MyInfo.SharedMutex, TmpWorkerInfo.wParameters);
			MyWork.push_back(TmpWorkerInfo);
		}
	}
	//不Running了,销毁其余
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
