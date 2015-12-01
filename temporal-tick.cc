#include <node.h>
#include <nan.h>
#include <unistd.h>

using namespace v8;

class TemporalTickWorker : public Nan::AsyncWorker {
  public:
    TemporalTickWorker(Nan::Callback *callback, int usec)
      : Nan::AsyncWorker(callback), usec(usec) {}
    ~TemporalTickWorker() {}

    // Blocking, executed inside the worker-thread.
    // This might do nothing at all. If usec > 0, then
    // the process sleeps in a thread before calling
    // HandleOKCallback
    void Execute() {
      usleep(usec);
    }

    // Async work is complete
    void HandleOKCallback() {
      Nan::HandleScope scope;

      Local<Value> argv[] = {
        Nan::Null(),
        Nan::New<Number>(usec)
      };

      callback->Call(2, argv);
    }

  private:
    int usec;
};

struct TemporalTickType {
  Nan::Callback *callback;
  int usec;
};

NAN_METHOD(TemporalTick) {
  Nan::HandleScope scope;

  TemporalTickType *tick = new TemporalTickType();

  if (info.Length() == 1) {
    if (!info[0]->IsFunction()) {
      Nan::ThrowTypeError("Expected function");
      return;
    }

    tick->usec = 0;
    tick->callback = new Nan::Callback(info[0].As<Function>());
  }

  if (info.Length() == 2) {
    if (!info[0]->IsNumber()) {
      Nan::ThrowTypeError("Expected number");
      return;
    }

    if (!info[1]->IsFunction()) {
      Nan::ThrowTypeError("Expected function");
      return;
    }

    tick->usec = info[0]->NumberValue();
    tick->callback = new Nan::Callback(info[1].As<Function>());
  }

  Nan::AsyncQueueWorker(new TemporalTickWorker(tick->callback, tick->usec));
  return;
}

void Init(Handle<Object> exports) {
  Nan::SetMethod(exports, "tick", TemporalTick);
  Nan::SetMethod(exports, "usleep", TemporalTick);
}

NODE_MODULE(temporal_tick, Init);
