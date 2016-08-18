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
#include "Task.h"
#include <JavaScriptCore/Completion.h>
#include <JavaScriptCore/StrongInlines.h>

namespace NativeScript {
using namespace JSC;

WorkerThread::WorkerThread(const String applicationPath, const String& entryModuleId, WorkerMessagingProxy* workerObjectProxy)
    : applicationPath(applicationPath)
    , entryModuleId(entryModuleId)
    , threadStarted(false)
    , workerObjectProxy(workerObjectProxy)
    , continueRunLoop(true)
    , useMicrotasksQueue(true) {
}

bool WorkerThread::start() {
    if (!this->threadStarted) {

        createThread(WorkerThread::workerThreadMain, this, "NativeScript: Worker");
        this->threadStarted = true;
    }
    return this->threadStarted;
}

void WorkerThread::stop() {
    this->enqueTaskOnGlobalObject(adoptRef(new Task([this](JSC::ExecState* execState) {
                                      this->continueRunLoop = false;
                                      [this->runtime removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
                                  })),
                                  MicrotaskFlags::Cleanup);
}

JSWorkerGlobalObject* WorkerThread::workerGlobalObject() {
    return jsCast<JSWorkerGlobalObject*>(((TNSRuntime*)this->runtime)->_globalObject.get());
}

void WorkerThread::workerThreadMain(void* thread) {
    static_cast<WorkerThread*>(thread)->workerThreadStart();
}

void WorkerThread::workerThreadStart() {
    TNSRuntime* tnsRuntime;
    @autoreleasepool {
        // create new runtime instance
        tnsRuntime = [[TNSRuntime alloc] initWithApplicationPath:applicationPath isWorker:true];
        this->runtime = tnsRuntime;
        this->workerGlobalObject()->setWorkerObjectProxy(this->workerObjectProxy);
        [runtime scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
        // Add the entry module execution task to the run loop
        this->workerGlobalObject()->queueTaskToEventLoop(adoptRef(new Task([this](JSC::ExecState* execState) {
            [this->runtime executeModule:this->entryModuleId];
        })));
        {
            WTF::LockHolder lock(this->microtasksQueueMutex);
            while (!this->microtasksQueue.isEmpty()) {
                this->workerGlobalObject()->queueTaskToEventLoop(this->microtasksQueue.takeFirst());
            }
            this->useMicrotasksQueue = false;
        }

        CFRunLoopRunResult result;
        do {
            result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 10, true);
        } while (result != kCFRunLoopRunFinished && result != kCFRunLoopRunStopped && this->continueRunLoop);
    }

    [tnsRuntime release];
    this->runtime = nil;
    this->workerObjectProxy = nullptr;
}

void WorkerThread::enqueTaskOnGlobalObject(WTF::PassRefPtr<JSC::Microtask> task, MicrotaskFlags flags) {
    WTF::LockHolder lock(this->microtasksQueueMutex);
    if (UNLIKELY(this->useMicrotasksQueue)) {
        // The runtime is not initialized yet
        this->microtasksQueue.append(task);
    } else {
        this->workerGlobalObject()->queueTaskToEventLoop(task, flags);
    }
}
}