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
    : _parentGlobalObject(parentGlobalObject)
    , _worker(worker)
    , _askedToTerminate(false) {
    ASSERT(this->_worker);
    ASSERT(isMainThread()); // TODO: Should be revisited when supporting nested workers
}

void WorkerMessagingProxy::startWorkerGlobalScope(const WTF::String& applicationPath, const WTF::String& entryModuleId) {
    this->_workerThread = std::make_unique<WorkerThread>(applicationPath, entryModuleId, this);
    this->_workerThread->start();
}

void WorkerMessagingProxy::terminateWorkerGlobalScope() {
    if (this->_askedToTerminate)
        return;
    this->_askedToTerminate = true;

    if (this->_workerThread)
        this->_workerThread->stop();
}

void WorkerMessagingProxy::workerGlobalScopeClosed() {
    this->_parentGlobalObject->queueTaskToEventLoop(adoptRef(new Task([this](ExecState* execState) {
        this->terminateWorkerGlobalScope();
    })));
}
}