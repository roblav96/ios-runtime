//
//  WorkerThread.cpp
//  NativeScript
//
//  Created by Ivan Buhov on 7/5/16.
//
//

#include "WorkerThread.h"
#include "WorkerMessagingProxy.h"
#include "JSWorkerGlobalObject.h"
#include "../TNSRuntime+Private.h"
#include <JavaScriptCore/Completion.h>
#include <JavaScriptCore/StrongInlines.h>

namespace NativeScript {
using namespace JSC;

class ExecuteEntryModuleTask : public JSC::Microtask {
public:
    ExecuteEntryModuleTask(TNSRuntime* runtime, NSString* moduleId)
        : runtime(runtime)
        , moduleId(moduleId) {
    }

    virtual void run(JSC::ExecState* execState) override {
        [runtime executeModule:moduleId];
    }

private:
    TNSRuntime* runtime;
    NSString* moduleId;
};

WorkerThread::WorkerThread(const String applicationPath, const String& entryModuleId, WorkerObjectProxy* workerObjectProxy)
    : applicationPath(applicationPath)
    , entryModuleId(entryModuleId)
    , threadID(0)
    , workerObjectProxy(workerObjectProxy)
    , runtimeIsInitialized(false) {
}

bool WorkerThread::start() {
    if (!this->threadID)
        this->threadID = createThread(WorkerThread::workerThreadStart, this, "NativeScript: Worker");
    return this->threadID;
}

void WorkerThread::workerThreadStart(void* thread) {
    static_cast<WorkerThread*>(thread)->workerThread();
}

void WorkerThread::workerThread() {
    // create new runtime instance
    this->runtime = [[TNSRuntime alloc] initWithApplicationPath:applicationPath isWorker:true];
    [runtime scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
    ((TNSRuntime*)this->runtime)->_globalObject->queueTaskToEventLoop(adoptRef(new ExecuteEntryModuleTask(runtime, entryModuleId)));
    {
        WTF::LockHolder lock(this->microtasksQueueMutex);
        while (!this->microtasksQueue.isEmpty()) {
            ((TNSRuntime*)this->runtime)->_globalObject->queueTaskToEventLoop(this->microtasksQueue.takeFirst());
        }
        this->runtimeIsInitialized = true;
    }
    [[NSRunLoop currentRunLoop] run];
}

void WorkerThread::enqueTaskOnGlobalObject(WTF::PassRefPtr<JSC::Microtask> task) {
    WTF::LockHolder lock(this->microtasksQueueMutex);
    if (this->runtimeIsInitialized) {
        ((TNSRuntime*)this->runtime)->_globalObject->queueTaskToEventLoop(task);
    } else {
        // The runtime is not initialized yet
        this->microtasksQueue.append(task);
    }
}
}