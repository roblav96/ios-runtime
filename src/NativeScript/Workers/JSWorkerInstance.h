//
//  JSWorkerInstance.h
//  NativeScript
//
//  Created by Ivan Buhov on 7/5/16.
//
//

#ifndef __NativeScript__JSWorkerInstance__
#define __NativeScript__JSWorkerInstance__

namespace NativeScript {
class WorkerMessagingProxy;

class JSWorkerInstance : public JSC::JSDestructibleObject {
public:
    typedef JSC::JSDestructibleObject Base;

    DECLARE_INFO;

    static JSWorkerInstance* create(JSC::VM& vm, JSC::Structure* structure, WTF::String& applicationPath, const WTF::String& entryModuleId);

    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype) {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

    /* TODO: Implement postMessage communication
    void postMessage(PassRefPtr<SerializedScriptValue> message, const MessagePortArray*, ExceptionCode&);
    
    void postMessage(PassRefPtr<SerializedScriptValue> message, MessagePort*, ExceptionCode&);
     */

    void terminate() {
        //contextProxy->terminateWorkerGlobalScope();
    }

private:
    WTF::String applicationPath;
    WTF::String entryModuleId;
    std::shared_ptr<WorkerMessagingProxy> globalObjectProxy;

    JSWorkerInstance(JSC::VM& vm, JSC::Structure* structure, WTF::String& applicationPath, const WTF::String& entryModuleId);

    ~JSWorkerInstance();

    void finishCreation(JSC::VM& vm);

    static void destroy(JSC::JSCell* cell) {
        JSC::jsCast<JSWorkerInstance*>(cell)->~JSWorkerInstance();
    }
};
}

#endif /* defined(__NativeScript__JSWorkerInstance__) */