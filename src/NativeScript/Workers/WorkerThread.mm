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
#include "TNSRuntime+Private.h"
#include "Task.h"
#include <JavaScriptCore/Completion.h>
#include <JavaScriptCore/StrongInlines.h>
#include <WTF/RunLoop.h>

namespace NativeScript {
using namespace JSC;

WorkerThread::WorkerThread(const String applicationPath, const String& entryModuleId, WorkerMessagingProxy* workerObjectProxy)
    : _applicationPath(applicationPath)
    , _entryModuleId(entryModuleId)
    , _threadID(0)
    , _workerObjectProxy(workerObjectProxy)
    , _useMicrotasksQueue(true) {
}

WTF::ThreadIdentifier WorkerThread::start() {
    WTF::LockHolder lock(this->_threadCreationMutex);
    if (!this->_threadID)
        this->_threadID = createThread(WorkerThread::workerThreadMain, this, "NativeScript: Worker");
    return this->_threadID;
}

void WorkerThread::stop() {
    this->enqueTaskOnGlobalObject(adoptRef(new Task([this](ExecState* execState) {
                                      WTF::RunLoop::current().stop();
                                  })),
                                  MicrotaskFlags::Cleanup);
}

JSWorkerGlobalObject* WorkerThread::workerGlobalObject() {
    return jsCast<JSWorkerGlobalObject*>(this->_runtime->_globalObject.get());
}

void WorkerThread::workerThreadMain(void* thread) {
    static_cast<WorkerThread*>(thread)->workerThreadStart();
}

void WorkerThread::workerThreadStart() {
    WTF::LockHolder lock(this->_threadCreationMutex);
    @autoreleasepool {
        // create new runtime instance
        this->_runtime = [[TNSWorkerRuntime alloc] initWithApplicationPath:this->_applicationPath];
        this->workerGlobalObject()->setWorkerObjectProxy(this->_workerObjectProxy);
        [this->_runtime scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
        // Add the entry module execution task to the run loop
        this->workerGlobalObject()->queueTaskToEventLoop(adoptRef(new Task([this](ExecState* execState) {
            [this->_runtime executeModule:this->_entryModuleId];
        })));
        {
            WTF::LockHolder lock(this->_microtasksQueueMutex);
            while (!this->_microtasksQueue.isEmpty()) {
                this->workerGlobalObject()->queueTaskToEventLoop(this->_microtasksQueue.takeFirst());
            }
            this->_useMicrotasksQueue = false;
        }

        WTF::RunLoop::run();
    }

    [this->_runtime release];
    this->_runtime = nil;
    this->_workerObjectProxy = nullptr;
    detachThread(this->_threadID);
}

void WorkerThread::enqueTaskOnGlobalObject(WTF::PassRefPtr<Microtask> task, MicrotaskFlags flags) {
    WTF::LockHolder lock(this->_microtasksQueueMutex);
    if (UNLIKELY(this->_useMicrotasksQueue)) {
        // The runtime is not initialized yet
        this->_microtasksQueue.append(task);
    } else {
        this->workerGlobalObject()->queueTaskToEventLoop(task, flags);
    }
}
}