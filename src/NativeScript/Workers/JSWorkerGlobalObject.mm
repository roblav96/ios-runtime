#include "JSWorkerGlobalObject.h"
#include "WorkerMessagingProxy.h"
#include "Error.h"
#include "Task.h"

#include <JavaScriptCore/runtime/JSJob.h>

using namespace JSC;

namespace NativeScript {

static EncodedJSValue JSC_HOST_CALL jsWorkerGlobalObjectClose(ExecState* execState) {
    JSWorkerGlobalObject* globalObject = jsCast<JSWorkerGlobalObject*>(execState->lexicalGlobalObject());
    globalObject->close();
    return JSValue::encode(jsUndefined());
}

static EncodedJSValue JSC_HOST_CALL jsWorkerGlobalObjectPostMessage(ExecState* state) {
    //TODO: call this->postMessage with resolved arguments
    return JSValue::encode(jsNull());
}

const ClassInfo JSWorkerGlobalObject::s_info = { "NativeScriptWorkerGlobal", &Base::s_info, 0, CREATE_METHOD_TABLE(JSWorkerGlobalObject) };

void JSWorkerGlobalObject::finishCreation(VM& vm, WTF::String applicationPath) {
    Base::finishCreation(vm, applicationPath);

    this->_onCloseIdentifier = Identifier::fromString(&vm, "onclose");
    this->putDirect(vm, _onCloseIdentifier, jsUndefined(), DontEnum | DontDelete);

    this->putDirect(vm, Identifier::fromString(&vm, "self"), this->globalExec()->globalThisValue(), DontEnum | ReadOnly | DontDelete);
    this->putDirectNativeFunction(vm, this, vm.propertyNames->close, 0, jsWorkerGlobalObjectClose, NoIntrinsic, DontEnum | DontDelete | ReadOnly);
    this->putDirectNativeFunction(vm, this, vm.propertyNames->postMessage, 0, jsWorkerGlobalObjectPostMessage, NoIntrinsic, DontEnum | DontDelete | ReadOnly);
    // TODO: importScripts should be able to receive more than one argument
    this->putDirectNativeFunction(vm, this, Identifier::fromString(&vm, "importScripts"), 1, commonJSRequire, NoIntrinsic, DontEnum | DontDelete | ReadOnly);
}

void JSWorkerGlobalObject::setWorkerObjectProxy(WorkerMessagingProxy* workerObjectProxy) {
    this->_workerObjectProxy = workerObjectProxy;
}

void JSWorkerGlobalObject::close() {
    if (this->_isClosing)
        return;

    this->_isClosing = true;
    // Allow only cleanup tasks to be executed
    this->setMicrotasksMask((MicrotaskFlags)(this->microtasksMask() | MicrotaskFlags::Cleanup));

    this->queueTaskToEventLoop(adoptRef(new Task([this](ExecState* execState) {
                                   JSValue onClose = this->getDirect(this->vm(), this->_onCloseIdentifier);
                                   if (onClose.isCell() && onClose.isValidCallee()) {
                                       this->queueTaskToEventLoop(createJSJob(this->vm(), onClose, constructEmptyArray(this->globalExec(), nullptr)), MicrotaskFlags::Cleanup);
                                   }
                                   this->_workerObjectProxy->workerGlobalScopeClosed();
                               })),
                               MicrotaskFlags::Cleanup);
}
}
