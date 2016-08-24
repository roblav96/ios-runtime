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
        : _action(action) {
    }

    virtual void run(JSC::ExecState* execState) override {
        this->_action(execState);
    }

private:
    std::function<void(JSC::ExecState*)> _action;
};
}

#endif /* defined(__NativeScript__Task__) */