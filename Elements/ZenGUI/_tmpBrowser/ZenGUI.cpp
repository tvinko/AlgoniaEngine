#include "ZenGUI.h"
#include "ZenCommon.h"
#include "ZenCoreCLR.h"
#include "Browser.h"

#include "TaskQueue.h"
#include "WorkerThread.h"

using namespace ultralight;
std::mutex mtx;
std::condition_variable cv;

char *_current_node_id;

int callback_node_match = 0;
bool is_node_completed() { return callback_node_match; }

TaskQueue<Task> task_queue_;
WorkerThread<Task> worker_thread_;

static UI *g_ui = 0;

#define UI_HEIGHT 80

UI::UI(Ref<Window> window) : window_(window)
{
  uint32_t window_width = App::instance()->window()->width();
  ui_height_ = (uint32_t)std::round(UI_HEIGHT * window_->scale());
  overlay_ = Overlay::Create(window_, window_width, ui_height_, 0, 0);
  g_ui = this;

  view()->set_load_listener(this);
  view()->set_view_listener(this);
  view()->LoadURL("file:///ui.html");
}

UI::~UI()
{
  view()->set_load_listener(nullptr);
  view()->set_view_listener(nullptr);
  g_ui = nullptr;
}

void UI::OnClose()
{
}

void UI::OnResize(uint32_t width, uint32_t height)
{
  RefPtr<Window> window = App::instance()->window();

  int tab_height = window->height() - ui_height_;

  if (tab_height < 1)
    tab_height = 1;

  overlay_->Resize(window->width(), ui_height_);

  for (auto &tab : tabs_)
  {
    if (tab.second)
      tab.second->Resize(window->width(), (uint32_t)tab_height);
  }
}

void UI::OnUpdate()
{
  while (!task_queue_.empty())
  {
    Task task = task_queue_.pop();
    task();
  }
}

void UI::OnDOMReady(View *caller, uint64_t frame_id, bool is_main_frame, const String &url)
{
  // Set the context for all subsequent JS* calls
  Ref<JSContext> locked_context = view()->LockJSContext();
  SetJSContext(locked_context.get());

  JSObject global = JSGlobalObject();
  updateBack = global["updateBack"];
  updateForward = global["updateForward"];
  updateLoading = global["updateLoading"];
  updateURL = global["updateURL"];
  addTab = global["addTab"];
  updateTab = global["updateTab"];
  closeTab = global["closeTab"];

  global["OnBack"] = BindJSCallback(&UI::OnBack);
  global["OnForward"] = BindJSCallback(&UI::OnForward);
  global["OnRefresh"] = BindJSCallback(&UI::OnRefresh);
  global["OnStop"] = BindJSCallback(&UI::OnStop);
  global["OnToggleTools"] = BindJSCallback(&UI::OnToggleTools);
  global["OnRequestNewTab"] = BindJSCallback(&UI::OnRequestNewTab);
  global["OnRequestTabClose"] = BindJSCallback(&UI::OnRequestTabClose);
  global["OnActiveTabChange"] = BindJSCallback(&UI::OnActiveTabChange);
  global["OnRequestChangeURL"] = BindJSCallback(&UI::OnRequestChangeURL);

  global["algonia_execute_node"] = BindJSCallbackWithRetval(&UI::algonia_execute_node);
  global["algonia_execute_node_async"] = BindJSCallbackWithRetval(&UI::algonia_execute_node_async);
  global["algonia_execute_node_and_get_result"] = BindJSCallbackWithRetval(&UI::algonia_execute_node_and_get_result);
  global["algonia_set_node_arg"] = BindJSCallbackWithRetval(&UI::algonia_set_node_arg);
  global["algonia_get_result"] = BindJSCallbackWithRetval(&UI::algonia_get_result);
  //String result = overlay_->view()->EvaluateScript("algonia_get_nodes_to_execute_callback()");

  CreateNewTab();
}

void UI::OnBack(const JSObject &obj, const JSArgs &args)
{
  if (active_tab())
    active_tab()->view()->GoBack();
}

void UI::OnForward(const JSObject &obj, const JSArgs &args)
{
  if (active_tab())
    active_tab()->view()->GoForward();
}

void UI::OnRefresh(const JSObject &obj, const JSArgs &args)
{
  if (active_tab())
    active_tab()->view()->Reload();
}

void UI::OnStop(const JSObject &obj, const JSArgs &args)
{
  if (active_tab())
    active_tab()->view()->Stop();
}

void UI::OnToggleTools(const JSObject &obj, const JSArgs &args)
{
  if (active_tab())
    active_tab()->ToggleInspector();
}

void UI::OnRequestNewTab(const JSObject &obj, const JSArgs &args)
{
  CreateNewTab();
}

void UI::OnRequestTabClose(const JSObject &obj, const JSArgs &args)
{
  if (args.size() == 1)
  {
    uint64_t id = args[0];

    auto &tab = tabs_[id];
    if (!tab)
      return;

    if (tabs_.size() == 1 && App::instance())
      App::instance()->Quit();

    if (id != active_tab_id_)
    {
      tabs_[id].reset();
      tabs_.erase(id);
    }
    else
    {
      tab->set_ready_to_close(true);
    }

    Ref<JSContext> lock(view()->LockJSContext());
    closeTab({id});
  }
}

void UI::OnActiveTabChange(const JSObject &obj, const JSArgs &args)
{
  if (args.size() == 1)
  {
    uint64_t id = args[0];

    if (id == active_tab_id_)
      return;

    auto &tab = tabs_[id];
    if (!tab)
      return;

    tabs_[active_tab_id_]->Hide();

    if (tabs_[active_tab_id_]->ready_to_close())
    {
      tabs_[active_tab_id_].reset();
      tabs_.erase(active_tab_id_);
    }

    active_tab_id_ = id;
    tabs_[active_tab_id_]->Show();

    auto tab_view = tabs_[active_tab_id_]->view();
    SetLoading(tab_view->is_loading());
    SetCanGoBack(tab_view->CanGoBack());
    SetCanGoForward(tab_view->CanGoBack());
    SetURL(tab_view->url());
  }
}

void UI::OnRequestChangeURL(const JSObject &obj, const JSArgs &args)
{
  if (args.size() == 1)
  {
    ultralight::String url = args[0];

    if (!tabs_.empty())
    {
      auto &tab = tabs_[active_tab_id_];
      tab->view()->LoadURL(url);
    }
  }
}

void UI::CreateNewTab()
{
  /*uint64_t id = tab_id_counter_++;
  RefPtr<Window> window = App::instance()->window();
  int tab_height = window->height() - ui_height_;
  if (tab_height < 1)
    tab_height = 1;
  tabs_[id].reset(new Tab(this, id, window->width(), (uint32_t)tab_height, 0, ui_height_));
  tabs_[id]->view()->LoadURL("file:///new_tab_page.html");

  Ref<JSContext> lock(view()->LockJSContext());
  addTab({id, "New Tab", "", tabs_[id]->view()->is_loading()});*/
}

RefPtr<View> UI::CreateNewTabForChildView(const String &url)
{
  uint64_t id = tab_id_counter_++;
  RefPtr<Window> window = App::instance()->window();
  int tab_height = window->height() - ui_height_;
  if (tab_height < 1)
    tab_height = 1;
  tabs_[id].reset(new Tab(this, id, window->width(), (uint32_t)tab_height, 0, ui_height_));

  Ref<JSContext> lock(view()->LockJSContext());
  addTab({id, "", url, tabs_[id]->view()->is_loading()});

  return tabs_[id]->view();
}

void UI::UpdateTabTitle(uint64_t id, const ultralight::String &title)
{
  Ref<JSContext> lock(view()->LockJSContext());
  updateTab({id, title, "", tabs_[id]->view()->is_loading()});
}

void UI::UpdateTabURL(uint64_t id, const ultralight::String &url)
{
  if (id == active_tab_id_ && !tabs_.empty())
    SetURL(url);
}

void UI::UpdateTabNavigation(uint64_t id, bool is_loading, bool can_go_back, bool can_go_forward)
{
  if (tabs_.empty())
    return;

  Ref<JSContext> lock(view()->LockJSContext());
  updateTab({id, tabs_[id]->view()->title(), "", tabs_[id]->view()->is_loading()});

  if (id == active_tab_id_)
  {
    SetLoading(is_loading);
    SetCanGoBack(can_go_back);
    SetCanGoForward(can_go_forward);
  }
}

void UI::SetLoading(bool is_loading)
{
  Ref<JSContext> lock(view()->LockJSContext());
  updateLoading({is_loading});
}

void UI::SetCanGoBack(bool can_go_back)
{
  Ref<JSContext> lock(view()->LockJSContext());
  updateBack({can_go_back});
}

void UI::SetCanGoForward(bool can_go_forward)
{
  Ref<JSContext> lock(view()->LockJSContext());
  updateForward({can_go_forward});
}

void UI::SetURL(const ultralight::String &url)
{
  Ref<JSContext> lock(view()->LockJSContext());
  updateURL({url});
}

void UI::SetCursor(ultralight::Cursor cursor)
{
  if (App::instance())
    App::instance()->window()->SetCursor(cursor);
}

JSValue UI::algonia_get_result(const JSObject &thisObject, const JSArgs &args)
{
  if (args.size() == 1)
  {
    RefPtr<JSContext> context = view()->LockJSContext();
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

JSValue UI::algonia_set_node_arg(const JSObject &thisObject, const JSArgs &args)
{
  if (args.size() == 2)
  {

    RefPtr<JSContext> context = view()->LockJSContext();
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

JSValue UI::algonia_execute_node_and_get_result(const JSObject &thisObject, const JSArgs &args)
{
  algonia_execute_node(thisObject, args);
  return algonia_get_result(thisObject, args);
}

JSValue UI::algonia_execute_node(const JSObject &thisObject, const JSArgs &args)
{
  if (args.size() == 1)
  {
    RefPtr<JSContext> context = view()->LockJSContext();
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

JSValue UI::algonia_execute_node_async(const JSObject &thisObject, const JSArgs &args)
{
  if (args.size() == 4)
  {
    callback_node_match = 0;
    JSValue resolve = args[2];
    JSValue reject = args[3];
    if (resolve.IsFunction())
    {
      JSFunction *resolve_func = new JSFunction(resolve.ToFunction());

      RefPtr<JSContext> context = view()->LockJSContext();
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

/*virtual void OnUpdate()
{
  while (!task_queue_.empty())
  {
    Task task = task_queue_.pop();
    task();
  }
}*/

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
  Browser browser;
  browser.Run();

  /*auto app = App::Create();
  auto window = Window::Create(app->main_monitor(), 800, 800, false, kWindowFlags_Titled);
  window->SetTitle("Algonia");
  app->set_window(window);

  AlgoniaGUI *algonia_gui = new AlgoniaGUI(window);
  app->set_listener(algonia_gui);

  app->Run();*/
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