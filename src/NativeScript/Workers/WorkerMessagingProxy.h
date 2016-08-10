//
//  WorkerMessagingProxy.h
//  NativeScript
//
//  Created by Ivan Buhov on 7/5/16.
//
//

#ifndef __NativeScript__WorkerMessagingProxy__
#define __NativeScript__WorkerMessagingProxy__

#include "WorkerThread.h"
#include <JavaScriptCore/InternalFunction.h>

namespace NativeScript {
class JSWorkerInstance;

class WorkerGlobalScopeProxy {
public:
    virtual void startWorkerGlobalScope(const String& applicationPath, const String& entryModuleId) = 0;
};

class WorkerReportingProxy {
};

class WorkerObjectProxy : public WorkerReportingProxy {
};

class WorkerLoaderProxy {
};

class WorkerMessagingProxy : public WorkerGlobalScopeProxy, public WorkerObjectProxy, public WorkerLoaderProxy {
public:
    WorkerMessagingProxy(JSWorkerInstance* worker);

    virtual void startWorkerGlobalScope(const String& applicationPath, const String& entryModuleId) override;

private:
    // TODO: This should be in JSC::WriteBarrier
    JSWorkerInstance* worker;
    RefPtr<WorkerThread> workerThread;
};
}

#endif /* defined(__NativeScript__WorkerMessagingProxy__) */