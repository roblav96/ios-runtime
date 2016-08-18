//
//  Task.h
//  NativeScript
//
//  Created by Ivan Buhov on 7/5/16.
//
//

#ifndef __NativeScript__Task__
#define __NativeScript__Task__

namespace NativeScript {

class Task : public JSC::Microtask {
public:
    Task(std::function<void(JSC::ExecState*)> action)
        : action(action) {
    }

    virtual void run(JSC::ExecState* execState) override {
        this->action(execState);
    }

private:
    std::function<void(JSC::ExecState*)> action;
};
}

#endif /* defined(__NativeScript__Task__) */