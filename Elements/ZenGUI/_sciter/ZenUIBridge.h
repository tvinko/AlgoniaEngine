#if defined(_WIN32)
#include <windows.h>
#include "plain-win.h"
#else if defined(__APPLE__)
#import <Cocoa/Cocoa.h>
#import <AppKit/NSApplication.h>
#endif
#include "sciter-x.h"
#include "sciter-x-graphics.hpp"
#include "sciter-x-window.hpp"
#include "sciter-om.h"
#include "stdafx.h"
#include <functional>
#include "aux-cvt.h"

class ZenUIBridge : public sciter::event_handler
{
public:
    ZenUIBridge();

    BEGIN_FUNCTION_MAP
    FUNCTION_1("algonia_execute_node", algonia_execute_node)
    FUNCTION_1("algonia_get_result", algonia_get_result)
    FUNCTION_2("algonia_set_node_arg", algonia_set_node_arg)
    FUNCTION_1("algonia_get_node_arg", algonia_get_node_arg)
    END_FUNCTION_MAP

    void AttachEventHandler(HWINDOW hw);
    void ZenUIBridge::InitCallback(HWINDOW hw);
    void InitAlgoniaCallbacks(
        void (*algonia_execute_node)(char *),
        void (*algonia_get_result)(char *, char **),
        void (*algonia_set_node_arg)(char *, char *),
        char *(*algonia_get_node_arg)(char *));

private:
    HELEMENT self;
    sciter::value algonia_execute_node(const sciter::value &node_id);
    sciter::value algonia_get_result(const sciter::value &node_id);
    sciter::value algonia_set_node_arg(const sciter::value &node_id, const sciter::value &arg_value);
    sciter::value algonia_get_node_arg(const sciter::value &node_id);
};