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
class WorkerMessagingProxy;

class WorkerThread {
public:
    bool start();

    // Called on worker termination
    void stop();

    JSWorkerGlobalObject* workerGlobalObject();

    void enqueTaskOnGlobalObject(WTF::PassRefPtr<JSC::Microtask> task, MicrotaskFlags flags = static_cast<MicrotaskFlags>(0));

    WorkerThread(const String applicationPath, const String& entryModuleId, WorkerMessagingProxy* workerObjectProxy);

private:
    // This method is executed as the core routine on the new thread
    static void workerThreadMain(void*);
    void workerThreadStart();

    String applicationPath;
    String entryModuleId;
    bool threadStarted;
    WorkerMessagingProxy* workerObjectProxy;

    id runtime;
    bool continueRunLoop;

    // Tasks queued before runtime initilaization are saved here and executed after the runtime is initialized
    WTF::Deque<WTF::RefPtr<JSC::Microtask>> microtasksQueue;
    std::atomic<bool> useMicrotasksQueue;
    WTF::Lock microtasksQueueMutex;
};
}

#endif /* defined(__NativeScript__WorkerThread__) */