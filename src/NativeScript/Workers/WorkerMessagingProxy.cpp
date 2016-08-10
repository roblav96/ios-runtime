//
//  WorkerMessagingProxy.cpp
//  NativeScript
//
//  Created by Ivan Buhov on 7/5/16.
//
//

#include "WorkerMessagingProxy.h"
#include "WorkerThread.h"

namespace NativeScript {
using namespace JSC;

WorkerMessagingProxy::WorkerMessagingProxy(JSWorkerInstance* workerInstance)
    : worker(workerInstance) {
    ASSERT(worker);
    ASSERT(isMainThread()); // TODO: Should be revisited when supporting nested workers
}

void WorkerMessagingProxy::startWorkerGlobalScope(const String& applicationPath, const String& entryModuleId) {
    this->workerThread = adoptRef(new WorkerThread(applicationPath, entryModuleId, this));
    this->workerThread->start();
}
}