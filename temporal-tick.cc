#include <node.h>
#include <nan.h>

using namespace v8;

class TemporalTickWorker : public NanAsyncWorker {
  public:
    TemporalTickWorker(NanCallback *callback, int usec)
      : NanAsyncWorker(callback), usec(usec) {}
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
      NanScope();

      Local<Value> argv[] = {
        NanNull(),
        NanNew<Number>(usec)
      };

      callback->Call(2, argv);
    }

  private:
    int usec;
};

struct TemporalTickType {
  NanCallback *callback;
  int usec;
};

NAN_METHOD(TemporalTick) {
  NanScope();

  TemporalTickType *tick = new TemporalTickType();

  if (args.Length() == 1) {
    if (!args[0]->IsFunction()) {
      NanThrowTypeError("Expected function");
      NanReturnUndefined();
    }

    tick->usec = 0;
    tick->callback = new NanCallback(args[0].As<Function>());
  }

  if (args.Length() == 2) {
    if (!args[0]->IsNumber()) {
      NanThrowTypeError("Expected number");
      NanReturnUndefined();
    }

    if (!args[1]->IsFunction()) {
      NanThrowTypeError("Expected function");
      NanReturnUndefined();
    }

    tick->usec = args[0]->NumberValue();
    tick->callback = new NanCallback(args[1].As<Function>());
  }

  NanAsyncQueueWorker(new TemporalTickWorker(tick->callback, tick->usec));
  NanReturnUndefined();
}

void Init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "tick", TemporalTick);
  NODE_SET_METHOD(exports, "usleep", TemporalTick);
}

NODE_MODULE(temporal_tick, Init)
