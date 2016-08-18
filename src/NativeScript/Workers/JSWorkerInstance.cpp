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

JSWorkerInstance* JSWorkerInstance::create(JSC::VM& vm, JSC::Structure* structure, WTF::String& applicationPath, const WTF::String& entryModuleId) {
    // We don't currently support nested workers, so workers can only be created from the main thread.
    ASSERT(isMainThread());

    JSWorkerInstance* object = new (NotNull, JSC::allocateCell<JSWorkerInstance>(vm.heap)) JSWorkerInstance(vm, structure, applicationPath, entryModuleId);
    object->finishCreation(vm);
    return object;
}

JSWorkerInstance::JSWorkerInstance(JSC::VM& vm, JSC::Structure* structure, WTF::String& applicationPath, const WTF::String& entryModuleId)
    : Base(vm, structure)
    , applicationPath(applicationPath)
    , entryModuleId(entryModuleId)
    , globalObjectProxy(new WorkerMessagingProxy(jsCast<GlobalObject*>(structure->globalObject()), this)) {
}

JSWorkerInstance::~JSWorkerInstance() {
    this->globalObjectProxy->terminateWorkerGlobalScope();
}

void JSWorkerInstance::finishCreation(JSC::VM& vm) {
    Base::finishCreation(vm);

    // WorkerThreadStartMode startMode = DontPauseWorkerGlobalScopeOnStart;
    /* TODO: Check whether should stop on the first line on the Worker thread
     if (InspectorInstrumentation::shouldPauseDedicatedWorkerOnStart(scriptExecutionContext()))
     startMode = PauseWorkerGlobalScopeOnStart;
     */

    this->globalObjectProxy->startWorkerGlobalScope(this->applicationPath, entryModuleId);

    /* TDOO: Notify the inspector that a script is imported
     InspectorInstrumentation::scriptImported(scriptExecutionContext(), m_scriptLoader->identifier(), m_scriptLoader->script());
     */
}
}