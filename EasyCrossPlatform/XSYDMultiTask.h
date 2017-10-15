#ifndef XSYDMultiTaskFile
	#define XSYDMultiTaskFile
	#include "EasyCP_Common.h"
	#include <thread>
	#include <mutex>
	namespace EasyCrossPlatform {
		namespace Thread {
			typedef void (*ThreadTask)(unsigned int&,void*, bool *, std::mutex &);
			typedef void (*ThreadCallBack)(unsigned int&, std::mutex &);
			struct pendingThreadInfo {
				ThreadTask TasktoExec;
				void* Parameter;
				ThreadCallBack CallBack;
				unsigned int Unique_ID;
			};
			struct executingThreadInfo {
				std::thread* MyThread;
				unsigned int Unique_ID;
				bool exited;
			};

			int DoMultiThread(pendingThreadInfo PendingInfo, bool *isRunningSign, void* myMutex);
			int SuperviseThreads(void *Infos, void* pendingInfos, bool* isSupervising, unsigned int MaxWorkerNum, bool *isRunning, void* myMutex);

			class ThreadManager{
			private:
				std::deque<executingThreadInfo> _runningThreads;
				std::deque<pendingThreadInfo> _waitThreads;
				std::thread *supervisorThead;
				std::mutex MMutex;
				unsigned int MaxWorkNum = 0;
				unsigned int MaxUID = 0;
				bool isRunning = false;
				bool isSupervising = false;
				unsigned int deleteID;
				void cleanAllThread();
			public:
				ThreadManager() : ThreadManager(1) {};
				ThreadManager(unsigned int WorkerNum);
				unsigned int addThread(ThreadTask MyTask, void* ThreadParameter, ThreadCallBack CallBack);
				bool deleteThread(unsigned int UniqueID, bool ThreadFinishCall = false);
				~ThreadManager();
			};
			
		}
	}
#endif