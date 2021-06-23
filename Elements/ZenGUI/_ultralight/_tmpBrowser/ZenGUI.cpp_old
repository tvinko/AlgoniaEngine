#include <iostream>
#include "ZenCommon.h"
#include "ZenCoreCLR.h"

#include <AppCore/App.h>

#include <AppCore/Overlay.h>
#include <AppCore/JSHelpers.h>

#include "TaskQueue.h"
#include "WorkerThread.h"

using namespace ultralight;
std::mutex mtx;
std::condition_variable cv;

char *_current_node_id;

int callback_node_match = 0;
bool is_node_completed() { return callback_node_match; }

//std::vector<std::string> _running_nodes;

class AlgoniaGUI : public LoadListener,
                   public AppListener

{
  RefPtr<Overlay> overlay_;

public:
  AlgoniaGUI(Ref<Window> win)
  {
    overlay_ = Overlay::Create(win, win->width(), win->height(), 0, 0);
    overlay_->view()->set_load_listener(this);
    overlay_->view()->LoadURL("file:///app.html");
  }

  virtual ~AlgoniaGUI() {}

  TaskQueue<Task> task_queue_;
  WorkerThread<Task> worker_thread_;

  JSValue algonia_get_result(const JSObject &thisObject, const JSArgs &args)
  {
    if (args.size() == 1)
    {
      RefPtr<JSContext> context = overlay_->view()->LockJSContext();
      JSContextRef ctx = context->ctx();

      JSStringRef js_node_id = JSValueToStringCopy(ctx, args[0], nullptr);
      size_t node_id_size = JSStringGetMaximumUTF8CStringSize(js_node_id);
      char *node_id = new char[node_id_size];
      JSStringGetUTF8CString(js_node_id, node_id, node_id_size);

      Node *node = NULL;

      node = common_get_node_by_id(node_id);
      if (node != NULL)
      {
        int len = coreclr_get_result_len(node);
        char *result = (char *)malloc(len * sizeof(char) + 1);
        coreclr_get_result(node, result);

        auto jsVal = JSValue(result);
        free(result);
        return JSValue(result);
      }
    }
    return JSValue("ERROR");
  }

  JSValue algonia_set_node_arg(const JSObject &thisObject, const JSArgs &args)
  {
    if (args.size() == 2)
    {

      RefPtr<JSContext> context = overlay_->view()->LockJSContext();
      JSContextRef ctx = context->ctx();

      JSStringRef js_node_id = JSValueToStringCopy(ctx, args[0], nullptr);
      size_t node_id_size = JSStringGetMaximumUTF8CStringSize(js_node_id);
      char *node_id = new char[node_id_size];
      JSStringGetUTF8CString(js_node_id, node_id, node_id_size);

      JSStringRef js_arg_value = JSValueToStringCopy(ctx, args[1], nullptr);
      size_t arg_value_size = JSStringGetMaximumUTF8CStringSize(js_arg_value);
      char *arg_value = new char[arg_value_size];
      JSStringGetUTF8CString(js_arg_value, arg_value, arg_value_size);

      Node *node = NULL;

      node = common_get_node_by_id(node_id);
      common_set_node_param_arg((Node *)node, arg_value);
    }
    return JSValue();
  }

  JSValue algonia_execute_node_and_get_result(const JSObject &thisObject, const JSArgs &args)
  {
    algonia_execute_node(thisObject, args);
    return algonia_get_result(thisObject, args);
  }

  JSValue algonia_execute_node(const JSObject &thisObject, const JSArgs &args)
  {
    if (args.size() == 1)
    {
      RefPtr<JSContext> context = overlay_->view()->LockJSContext();
      JSContextRef ctx = context->ctx();

      JSStringRef js_node_id = JSValueToStringCopy(ctx, args[0], nullptr);
      size_t node_id_size = JSStringGetMaximumUTF8CStringSize(js_node_id);
      char *node_id = new char[node_id_size];
      JSStringGetUTF8CString(js_node_id, node_id, node_id_size);

      Node *node = NULL;

      node = common_get_node_by_id(node_id);
      if (node != NULL)
        common_exec_node(node);
    }
    return JSValue();
  }

  JSValue algonia_execute_node_async(const JSObject &thisObject, const JSArgs &args)
  {
    if (args.size() == 4)
    {
      callback_node_match = 0;
      JSValue resolve = args[2];
      JSValue reject = args[3];
      if (resolve.IsFunction())
      {
        JSFunction *resolve_func = new JSFunction(resolve.ToFunction());

        RefPtr<JSContext> context = overlay_->view()->LockJSContext();
        JSContextRef ctx = context->ctx();

        resolve_func->set_context(ctx);

        JSStringRef js_node_id = JSValueToStringCopy(ctx, args[0], nullptr);
        size_t node_id_size = JSStringGetMaximumUTF8CStringSize(js_node_id);
        char *node_id = new char[node_id_size];
        JSStringGetUTF8CString(js_node_id, node_id, node_id_size);

        JSStringRef js_cc_node_id = JSValueToStringCopy(ctx, args[1], nullptr);
        size_t cc_node_id_size = JSStringGetMaximumUTF8CStringSize(js_cc_node_id);
        char *cc_node_id = new char[cc_node_id_size];
        JSStringGetUTF8CString(js_cc_node_id, cc_node_id, cc_node_id_size);

        worker_thread_.PostTask([=] {
          Node *node = NULL;
          //_running_nodes.push_back(cc_node_id);
          _current_node_id = cc_node_id;
          node = common_get_node_by_id(node_id);
          common_exec_node(node);

          std::unique_lock<std::mutex> lck(mtx);
          cv.wait(lck, is_node_completed);

          task_queue_.push([=] {
            (*resolve_func)({"Hello from async task!"});
            delete resolve_func;
          });
        });
      }
    }
    return JSValue();
  }

  virtual void OnUpdate()
  {
    while (!task_queue_.empty())
    {
      Task task = task_queue_.pop();
      task();
    }
  }

  virtual void OnDOMReady(ultralight::View *caller,
                          uint64_t frame_id,
                          bool is_main_frame,
                          const String &url) override
  {
    Ref<JSContext> context = caller->LockJSContext();
    SetJSContext(context.get());

    JSObject global = JSGlobalObject();
    global["algonia_execute_node"] = BindJSCallbackWithRetval(&AlgoniaGUI::algonia_execute_node);
    global["algonia_execute_node_async"] = BindJSCallbackWithRetval(&AlgoniaGUI::algonia_execute_node_async);
    global["algonia_execute_node_and_get_result"] = BindJSCallbackWithRetval(&AlgoniaGUI::algonia_execute_node_and_get_result);
    global["algonia_set_node_arg"] = BindJSCallbackWithRetval(&AlgoniaGUI::algonia_set_node_arg);
    global["algonia_get_result"] = BindJSCallbackWithRetval(&AlgoniaGUI::algonia_get_result);
    //String result = overlay_->view()->EvaluateScript("algonia_get_nodes_to_execute_callback()");
  }
};

//################# ALGONIA SYSTEM AREA ************************************
EXTERN_DLL_EXPORT int onNodeComplete(Node *completedNode, Node *currentNode)
{

  if (_current_node_id != NULL && strcmp(completedNode->id, _current_node_id) == 0)
  {
    callback_node_match = 1;
    cv.notify_one();
  }

  /*for (const auto &el_node_id : _running_nodes)
  {
    char *c_nodeId = new char[el_node_id.length() + 1];
    strcpy(c_nodeId, el_node_id.c_str());
    if (strcmp(completedNode->id, c_nodeId) == 0)
    {
      callback_node_match = 1;
      cv.notify_one();
    }
    delete c_nodeId;
  }*/
  return 0;
}

EXTERN_DLL_EXPORT int onEngineStarted(Node *node)
{
  auto app = App::Create();
  auto window = Window::Create(app->main_monitor(), 800, 800, false, kWindowFlags_Titled);
  window->SetTitle("Algonia");
  app->set_window(window);

  AlgoniaGUI *algonia_gui = new AlgoniaGUI(window);
  app->set_listener(algonia_gui);

  app->Run();
  return 0;
}

EXTERN_DLL_EXPORT Node **getNodesToExecute(Node *node, int *nodesToExecuteCnt)
{
  char *node_ids = common_get_node_arg((Node *)node, "NODES_TO_EXECUTE");

  int numNodesToExecute = 0;
  char **nodesToExecute = NULL;
  return common_fill_disconnected_nodes(node_ids, nodesToExecuteCnt);
}

//################# END ALGONIA SYSTEM AREA **********************************