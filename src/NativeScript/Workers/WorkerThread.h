//
//  WorkerThread.h
//  NativeScript
//
//  Created by Ivan Buhov on 7/5/16.
//
//

#ifndef __NativeScript__WorkerThread__
#define __NativeScript__WorkerThread__

#include <JavaScriptCore/InternalFunction.h>

namespace NativeScript {
class JSWorkerGlobalObject;
class WorkerObjectProxy;

class WorkerThread : public RefCounted<WorkerThread> {
public:
    bool start();

    void enqueTaskOnGlobalObject(WTF::PassRefPtr<JSC::Microtask> task);

    WorkerThread(const String applicationPath, const String& entryModuleId, WorkerObjectProxy* workerObjectProxy);

private:
    // Static function executed as the core routine on the new thread. Passed a pointer to a WorkerThread object.
    static void workerThreadStart(void* thread);
    void workerThread();

    String applicationPath;
    String entryModuleId;
    ThreadIdentifier threadID;
    std::shared_ptr<WorkerObjectProxy> workerObjectProxy;

    id runtime;
    std::atomic<bool> runtimeIsInitialized;

    // Tasks queued before runtime initilaization are saved here and executed after the runtime is initialized
    WTF::Deque<WTF::RefPtr<JSC::Microtask>> microtasksQueue;
    WTF::Lock microtasksQueueMutex;
};
}

#endif /* defined(__NativeScript__WorkerThread__) */