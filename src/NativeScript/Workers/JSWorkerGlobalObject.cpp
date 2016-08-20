#include "JSWorkerGlobalObject.h"
#include "WorkerMessagingProxy.h"
#include "Error.h"
#include "Task.h"

using namespace JSC;

namespace NativeScript {

static EncodedJSValue JSC_HOST_CALL jsWorkerGlobalObjectClose(ExecState* execState) {
    JSWorkerGlobalObject* globalObject = jsCast<JSWorkerGlobalObject*>(execState->lexicalGlobalObject());
    ASSERT_GC_OBJECT_INHERITS(globalObject, JSWorkerGlobalObject::info());
    globalObject->close();
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL jsWorkerGlobalObjectPostMessage(ExecState* state) {
    //TODO: call this->postMessage with resolved arguments
    return JSValue::encode(jsNull());
}

JSWorkerGlobalObject* JSWorkerGlobalObject::create(WTF::String applicationPath, VM& vm, Structure* structure) {
    JSWorkerGlobalObject* object = new (NotNull, allocateCell<JSWorkerGlobalObject>(vm.heap)) JSWorkerGlobalObject(vm, structure);
    object->finishCreation(applicationPath, vm);
    vm.heap.addFinalizer(object, destroy);
    return object;
}

const ClassInfo JSWorkerGlobalObject::s_info = { "NativeScriptWorkerGlobal", &Base::s_info, 0, CREATE_METHOD_TABLE(JSWorkerGlobalObject) };

const unsigned JSWorkerGlobalObject::StructureFlags = Base::StructureFlags;

Structure* JSWorkerGlobalObject::createStructure(VM& vm, JSValue prototype) {
    return Structure::create(vm, 0, prototype, TypeInfo(GlobalObjectType, JSWorkerGlobalObject::StructureFlags), JSWorkerGlobalObject::info());
}

JSWorkerGlobalObject::JSWorkerGlobalObject(VM& vm, Structure* structure)
    : GlobalObject(vm, structure)
    , isClosing(false)
    , workerObjectProxy(nullptr) {
}

JSWorkerGlobalObject::~JSWorkerGlobalObject() {
}

void JSWorkerGlobalObject::finishCreation(WTF::String applicationPath, VM& vm) {
    Base::finishCreation(applicationPath, vm);

    this->putDirect(vm, Identifier::fromString(&vm, "self"), this->globalExec()->globalThisValue(), DontEnum | ReadOnly | DontDelete);
    this->putDirectNativeFunction(vm, this, vm.propertyNames->close, 0, jsWorkerGlobalObjectClose, NoIntrinsic, DontEnum | DontDelete | ReadOnly);
    this->putDirectNativeFunction(vm, this, vm.propertyNames->postMessage, 0, jsWorkerGlobalObjectPostMessage, NoIntrinsic, DontEnum | DontDelete | ReadOnly);
    // TODO: importScripts should be able to receive more than one argument
    this->putDirectNativeFunction(vm, this, Identifier::fromString(&vm, "importScripts"), 1, commonJSRequire, NoIntrinsic, DontEnum | DontDelete | ReadOnly);
}

void JSWorkerGlobalObject::setWorkerObjectProxy(WorkerMessagingProxy* workerObjectProxy) {
    this->workerObjectProxy = workerObjectProxy;
}

void JSWorkerGlobalObject::close() {
    if (isClosing)
        return;

    isClosing = true;
    // Allow only cleanup tasks to be executed
    this->setMicrotasksMask((MicrotaskFlags)(this->microtasksMask() | MicrotaskFlags::Cleanup));

    this->queueTaskToEventLoop(adoptRef(new Task([this](ExecState* execState) {
                                   this->workerObjectProxy->workerGlobalScopeClosed();
                               })),
                               MicrotaskFlags::Cleanup);
}
}