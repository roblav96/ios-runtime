#ifndef JSWorkerGlobalObject_h
#define JSWorkerGlobalObject_h

#include "GlobalObject.h"

namespace NativeScript {
class WorkerMessagingProxy;

class JSWorkerGlobalObject : public GlobalObject {
public:
    typedef GlobalObject Base;

    static JSWorkerGlobalObject* create(JSC::VM& vm, JSC::Structure* structure, WTF::String applicationPath) {
        JSWorkerGlobalObject* object = new (NotNull, JSC::allocateCell<JSWorkerGlobalObject>(vm.heap)) JSWorkerGlobalObject(vm, structure);
        object->finishCreation(vm, applicationPath);
        return object;
    }

    DECLARE_INFO;

    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSValue prototype) {
        return JSC::Structure::create(vm, 0, prototype, JSC::TypeInfo(JSC::GlobalObjectType, JSWorkerGlobalObject::StructureFlags), JSWorkerGlobalObject::info());
    }

    void close();

    void setWorkerObjectProxy(WorkerMessagingProxy*);

protected:
    JSWorkerGlobalObject(JSC::VM& vm, JSC::Structure* structure)
        : GlobalObject(vm, structure) {
    }

    ~JSWorkerGlobalObject() = default;

    void finishCreation(JSC::VM&, WTF::String applicationPath);

private:
    static void destroy(JSC::JSCell* cell) {
        static_cast<JSWorkerGlobalObject*>(cell)->~JSWorkerGlobalObject();
    }

    JSC::Identifier _onCloseIdentifier;

    bool _isClosing = false;
    WorkerMessagingProxy* _workerObjectProxy = nullptr;
};
} // namespace NativeScript

#endif