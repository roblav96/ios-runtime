#ifndef JSWorkerGlobalObject_h
#define JSWorkerGlobalObject_h

#include "../GlobalObject.h"

namespace NativeScript {
class WorkerMessagingProxy;

class JSWorkerGlobalObject : public GlobalObject {
public:
    typedef GlobalObject Base;

    static JSWorkerGlobalObject* create(WTF::String applicationPath, JSC::VM& vm, JSC::Structure* structure);

    DECLARE_INFO;

    static const unsigned StructureFlags;

    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSValue prototype);

    void close();

    void setWorkerObjectProxy(WorkerMessagingProxy* workerObjectProxy);

protected:
    JSWorkerGlobalObject(JSC::VM&, JSC::Structure*);

    ~JSWorkerGlobalObject();

    void finishCreation(WTF::String applicationPath, JSC::VM& vm);

private:
    bool isClosing;
    WorkerMessagingProxy* workerObjectProxy;
};
} // namespace NativeScript

#endif