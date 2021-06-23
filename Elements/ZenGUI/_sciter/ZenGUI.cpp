/*
<html>
    <head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
        <title>Sciter Application example</title>
    </head>

    <script type="text/tiscript">
	$(#test).onClick = function() {
	    try {        
		view.msgbox('started');
	      view.setText('dsadas');
		  view.getText();              
		  view.msgbox( view.getText());
		  view.msgbox('ended');
	    }
	    catch (e) { view.msgbox(#alert, e); }
      	};
    </script>
	
	<script type="text/tiscript">
    namespace Test {
        function add(n1,n2) 
		{ 
			view.msgbox('add');
			return n1+n2; 
		}
    }
</script>
    <body>
        <button name="TestButton" id="test">TEST</button>
    </body>
</html>
*/
#include "ZenGUI.h"

ZenUIBridge *uiBridge = new ZenUIBridge();

#if defined(_WIN32)
bool _isLoaded = false;

window::window()
{
    static ATOM cls = 0;
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = wnd_proc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = ghInstance;
    wcex.hIcon = LoadIcon(ghInstance, MAKEINTRESOURCE(IDI_PLAINWIN));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = 0; //MAKEINTRESOURCE(IDC_PLAINWIN);
    wcex.lpszClassName = WINDOW_CLASS_NAME;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    cls = RegisterClassEx(&wcex);

    _hwnd = CreateWindow(WINDOW_CLASS_NAME, L"demo", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, ghInstance, this);
    uiBridge->AttachEventHandler(_hwnd);

    if (!_hwnd)
        return;

    init();

    ShowWindow(_hwnd, SW_SHOW);
    UpdateWindow(_hwnd);
    uiBridge->InitCallback(_hwnd);
}

window *window::ptr(HWND hwnd)
{
    return reinterpret_cast<window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
}

bool window::init()
{
    SetWindowLongPtr(_hwnd, GWLP_USERDATA, LONG_PTR(this));
    setup_callback();
    return true;
}

LRESULT CALLBACK window::wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    BOOL handled = FALSE;
    LRESULT lr = SciterProcND(hWnd, message, wParam, lParam, &handled);

    if (handled)   // if it was handled by the Sciter
        return lr; // then no further processing is required.

    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CREATE:
        // SciterProcND has created instance of the engine for the HWND
        // while handling WM_CREATE thus we can load content:
        std::string current_dir(COMMON_PROJECT_ROOT);
        std::string www_index_path = "file://" + current_dir + "/gui/index.html";

        SciterLoadFile(hWnd, s2ws(www_index_path).c_str());
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}
#else if defined(__APPLE__)
static BOOL SC_CALLBACK element_proc(LPVOID tag, HELEMENT he, UINT evtg, LPVOID prms)
{
    sciter::event_handler *pThis = static_cast<sciter::event_handler *>(tag);
    if (pThis)
        switch (evtg)
        {
        case SUBSCRIPTIONS_REQUEST:
        {
            UINT *p = (UINT *)prms;
            return pThis->subscription(he, *p);
        }
        case HANDLE_INITIALIZATION:
        {
            INITIALIZATION_PARAMS *p = (INITIALIZATION_PARAMS *)prms;
            if (p->cmd == BEHAVIOR_DETACH)
                pThis->detached(he);
            else if (p->cmd == BEHAVIOR_ATTACH)
                pThis->attached(he);
            return true;
        }
        case HANDLE_MOUSE:
        {
            MOUSE_PARAMS *p = (MOUSE_PARAMS *)prms;
            return pThis->handle_mouse(he, *p);
        }
        case HANDLE_KEY:
        {
            KEY_PARAMS *p = (KEY_PARAMS *)prms;
            return pThis->handle_key(he, *p);
        }
        case HANDLE_FOCUS:
        {
            FOCUS_PARAMS *p = (FOCUS_PARAMS *)prms;
            return pThis->handle_focus(he, *p);
        }
        case HANDLE_DRAW:
        {
            DRAW_PARAMS *p = (DRAW_PARAMS *)prms;
            return pThis->handle_draw(he, *p);
        }
        case HANDLE_TIMER:
        {
            TIMER_PARAMS *p = (TIMER_PARAMS *)prms;
            return pThis->handle_timer(he, *p);
        }
        case HANDLE_BEHAVIOR_EVENT:
        {
            BEHAVIOR_EVENT_PARAMS *p = (BEHAVIOR_EVENT_PARAMS *)prms;
            return pThis->handle_event(he, *p);
        }
        case HANDLE_METHOD_CALL:
        {
            METHOD_PARAMS *p = (METHOD_PARAMS *)prms;
            return pThis->handle_method_call(he, *p);
        }
        case HANDLE_DATA_ARRIVED:
        {
            DATA_ARRIVED_PARAMS *p = (DATA_ARRIVED_PARAMS *)prms;
            return pThis->handle_data_arrived(he, *p);
        }
        case HANDLE_SCROLL:
        {
            SCROLL_PARAMS *p = (SCROLL_PARAMS *)prms;
            return pThis->handle_scroll(he, *p);
        }
        case HANDLE_SIZE:
        {
            pThis->handle_size(he);
            return false;
        }
        // call using sciter::value's (from CSSS!)
        case HANDLE_SCRIPTING_METHOD_CALL:
        {
            SCRIPTING_METHOD_PARAMS *p = (SCRIPTING_METHOD_PARAMS *)prms;
            return pThis->handle_scripting_call(he, *p);
        }
        // call using tiscript::value's (from the script)
        //case HANDLE_TISCRIPT_METHOD_CALL:
        //{
        //    TISCRIPT_METHOD_PARAMS *p = (TISCRIPT_METHOD_PARAMS *)prms;
        //    return pThis->handle_scripting_call(he, *p);
        //}
        case HANDLE_GESTURE:
        {
            GESTURE_PARAMS *p = (GESTURE_PARAMS *)prms;
            return pThis->handle_gesture(he, *p);
        }
        default:
            assert(false);
        }
    return false;
}

HWINDOW startup()
{
    RECT frame;
    frame.top = 100;
    frame.left = 100;
    frame.right = 100 + 500 + 1;
    frame.bottom = 100 + 500 + 1;

    HWINDOW hw = SciterCreateWindow(SW_MAIN | SW_ALPHA | SW_TOOL, &frame, 0, 0, 0);

    SciterLoadFile(hw, WSTR("file:///Users/tomaz/Documents/algonia/zenunmanaged/ZenEngine/default.htm"));
    uiBridge->AttachEventHandler(hw);

    return hw;
}
#endif

//########### START JS CALLBACKS##########################
void exec(char *node_id)
{
    Node *node = common_get_node_by_id(node_id);
    if (node != NULL)
        common_exec_node(node);
}

void get_result_raw(char *node_id, char **result)
{
    Node *node = common_get_node_by_id(node_id);
    if (node != NULL)
    {
        int len = coreclr_get_result_len(node);
        *result = (char *)malloc(len * sizeof(char) + 1);
        coreclr_get_result(node, *result);
    }
}

void set_node_arg(char *node_id, char *arg_val)
{
    Node *node = common_get_node_by_id(node_id);
    if (node != NULL)
    {
        common_set_node_param_arg((Node *)node, arg_val);
    }
}

char *get_node_arg(char *node_id)
{
    Node *node = common_get_node_by_id(node_id);
    if (node != NULL)
    {
        return common_get_node_param_arg((Node *)node);
    }
    return NULL;
}
//########### END JS CALLBACKS#############################

//############ START ALGONIA SYSTEM AREA###################
EXTERN_DLL_EXPORT Node **getNodesToExecute(Node *node, int *nodesToExecuteCnt)
{
    char *result = NULL;
    char *val = common_get_node_arg((Node *)node, "NODES_TO_EXECUTE");

    return common_fill_disconnected_nodes(val, nodesToExecuteCnt);
}

EXTERN_DLL_EXPORT int onEngineStarted(Node *node)
{
    //Insoector...
    SciterSetOption(NULL, SCITER_SET_DEBUG_MODE, TRUE);
    uiBridge->InitAlgoniaCallbacks(&exec, &get_result_raw, &set_node_arg, &get_node_arg);

#if defined(_WIN32)
    MSG msg;
    HACCEL hAccelTable;
    window w;

    /*SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES,
                    ALLOW_FILE_IO |
                        ALLOW_SOCKET_IO |
                        ALLOW_EVAL |
                        ALLOW_SYSINFO);*/

    

    hAccelTable = LoadAccelerators(NULL, MAKEINTRESOURCE(IDC_PLAINWIN));

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    OleUninitialize();
#else if defined(__APPLE__)
    NSApplication *application = [NSApplication sharedApplication];

    NSView *ns = (NSView *)startup();

    [[ns window] makeKeyAndOrderFront:nil];

    [application run];
#endif
    return 0;
}
//############ END ALGONIA SYSTEM AREA#####################

//############ START HELPERS ##############################
std::wstring s2ws(const std::string &s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t *buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}
//############ END HELPERS   ##############################