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

@class TNSRuntime;

namespace NativeScript {
class JSWorkerGlobalObject;
class WorkerMessagingProxy;

class WorkerThread {
public:
    WTF::ThreadIdentifier start();

    // Called on worker termination
    void stop();

    JSWorkerGlobalObject* workerGlobalObject();

    void enqueTaskOnGlobalObject(WTF::PassRefPtr<JSC::Microtask> task, MicrotaskFlags flags = static_cast<MicrotaskFlags>(0));

    WorkerThread(const String applicationPath, const String& entryModuleId, WorkerMessagingProxy* workerObjectProxy);

private:
    // This method is executed as the core routine on the new thread
    static void workerThreadMain(void*);
    void workerThreadStart();

    WTF::String _applicationPath;
    WTF::String _entryModuleId;
    WTF::ThreadIdentifier _threadID;
    WorkerMessagingProxy* _workerObjectProxy;
    WTF::Lock _threadCreationMutex;

    TNSRuntime* _runtime;

    // Tasks queued before runtime initilaization are saved here and executed after the runtime is initialized
    WTF::Deque<WTF::RefPtr<JSC::Microtask>> _microtasksQueue;
    std::atomic<bool> _useMicrotasksQueue;
    WTF::Lock _microtasksQueueMutex;
};
}

#endif /* defined(__NativeScript__WorkerThread__) */