//
//  WorkerMessagingProxy.cpp
//  NativeScript
//
//  Created by Ivan Buhov on 7/5/16.
//
//

#include "WorkerMessagingProxy.h"
#include "WorkerThread.h"
#include "Task.h"

namespace NativeScript {
using namespace JSC;

WorkerMessagingProxy::WorkerMessagingProxy(GlobalObject* parentGlobalObject, JSWorkerInstance* worker)
    : parentGlobalObject(parentGlobalObject)
    , worker(worker)
    , askedToTerminate(false) {
    ASSERT(this->worker);
    ASSERT(isMainThread()); // TODO: Should be revisited when supporting nested workers
}

void WorkerMessagingProxy::startWorkerGlobalScope(const String& applicationPath, const String& entryModuleId) {
    this->workerThread = std::make_unique<WorkerThread>(applicationPath, entryModuleId, this);
    this->workerThread->start();
}

void WorkerMessagingProxy::terminateWorkerGlobalScope() {
    if (this->askedToTerminate)
        return;
    this->askedToTerminate = true;

    if (this->workerThread)
        this->workerThread->stop();
}

void WorkerMessagingProxy::workerGlobalScopeClosed() {
    this->parentGlobalObject->queueTaskToEventLoop(adoptRef(new Task([this](JSC::ExecState* execState) {
        this->terminateWorkerGlobalScope();
    })));
}
}