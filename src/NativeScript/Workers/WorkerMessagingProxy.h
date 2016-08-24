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
    virtual void startWorkerGlobalScope(const WTF::String& applicationPath, const WTF::String& entryModuleId) = 0;
    virtual void terminateWorkerGlobalScope() = 0;
};

class WorkerReportingProxy {
};

class WorkerObjectProxy : public WorkerReportingProxy {
public:
    virtual void workerGlobalScopeClosed() = 0;
};

class WorkerLoaderProxy {
};

class WorkerMessagingProxy : public WorkerGlobalScopeProxy, public WorkerObjectProxy, public WorkerLoaderProxy {
public:
    WorkerMessagingProxy(GlobalObject* parentGlobalObject, JSWorkerInstance* worker);

    // WorkerGlobalScopeProxy
    virtual void startWorkerGlobalScope(const WTF::String& applicationPath, const WTF::String& entryModuleId) override;
    virtual void terminateWorkerGlobalScope() override;

    // WorkerObjectProxy
    virtual void workerGlobalScopeClosed() override;

private:
    // TODO: These should be in JSC::WriteBarrier
    GlobalObject* _parentGlobalObject;
    JSWorkerInstance* _worker;
    std::unique_ptr<WorkerThread> _workerThread;

    bool _askedToTerminate;
};
}

#endif /* defined(__NativeScript__WorkerMessagingProxy__) */