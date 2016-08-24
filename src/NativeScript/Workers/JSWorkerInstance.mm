//
//  JSWorkerInstance.cpp
//  NativeScript
//
//  Created by Ivan Buhov on 7/5/16.
//
//

#include "JSWorkerInstance.h"
#include "WorkerMessagingProxy.h"

namespace NativeScript {
using namespace JSC;

const ClassInfo JSWorkerInstance::s_info = { "Worker", &Base::s_info, 0, CREATE_METHOD_TABLE(JSWorkerInstance) };

JSWorkerInstance::~JSWorkerInstance() {
    this->terminate();
}

void JSWorkerInstance::terminate() {
    this->_globalObjectProxy->terminateWorkerGlobalScope();
}

void JSWorkerInstance::finishCreation(JSC::VM& vm, WTF::String& applicationPath, const WTF::String& entryModuleId) {
    Base::finishCreation(vm);

    // We don't currently support nested workers, so workers can only be created from the main thread.
    ASSERT(isMainThread());

    this->_applicationPath = applicationPath;
    this->_entryModuleId = entryModuleId;
    this->_globalObjectProxy = std::make_shared<WorkerMessagingProxy>(jsCast<GlobalObject*>(this->globalObject()), this);

    // WorkerThreadStartMode startMode = DontPauseWorkerGlobalScopeOnStart;
    /* TODO: Check whether should stop on the first line on the Worker thread
     if (InspectorInstrumentation::shouldPauseDedicatedWorkerOnStart(scriptExecutionContext()))
     startMode = PauseWorkerGlobalScopeOnStart;
     */

    this->_globalObjectProxy->startWorkerGlobalScope(this->_applicationPath, entryModuleId);

    /* TODO: Notify the inspector that a script is imported
     InspectorInstrumentation::scriptImported(scriptExecutionContext(), m_scriptLoader->identifier(), m_scriptLoader->script());
     */
}
}