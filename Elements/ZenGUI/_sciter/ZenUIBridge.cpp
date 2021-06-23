#pragma once
#include "ZenUIBridge.h"

ZenUIBridge::ZenUIBridge() : sciter::event_handler(), self(0) {}

std::string text;
void (*_algonia_execute_node)(char *);
void (*_algonia_get_result)(char *, char **);
void (*_algonia_set_node_arg)(char *, char *);
char *(*_algonia_get_node_arg)(char *);

void ZenUIBridge::AttachEventHandler(HWINDOW hw)
{
    sciter::attach_dom_event_handler(hw, this);
}

void ZenUIBridge::InitCallback(HWINDOW hw)
{
    window *self = reinterpret_cast<window *>(GetWindowLongPtr(hw, GWLP_USERDATA));
    sciter::dom::element root = self->get_root();

    sciter::value r;
    try
    {
        r = root.call_function("algonia.Initialized" /*,sciter::value(2), sciter::value(2)*/);
    }
    catch (sciter::script_error &err)
    {
        std::cerr << err.what() << std::endl;
    }
}

void ZenUIBridge::InitAlgoniaCallbacks(
    void (*algonia_execute_node)(char *),
    void (*algonia_get_result)(char *, char **),
    void (*algonia_set_node_arg)(char *, char *),
    char *(*algonia_get_node_arg)(char *))
{
    _algonia_execute_node = algonia_execute_node;
    _algonia_get_result = algonia_get_result;
    _algonia_set_node_arg = algonia_set_node_arg;
    _algonia_get_node_arg = algonia_get_node_arg;
}

sciter::value ZenUIBridge::algonia_get_result(const sciter::value &text_val)
{
    auto s_node_id = text_val.get<std::string>();
    char *node_id = new char[s_node_id.length() + 1];
    strcpy(node_id, s_node_id.c_str());
    char *result = NULL;
    _algonia_get_result(node_id, &result);
    auto sciter_result = sciter::value(result);

    delete node_id;
    delete result;

    return sciter_result;
}

sciter::value ZenUIBridge::algonia_get_node_arg(const sciter::value &raw_node_id)
{
    auto s_node_id = raw_node_id.get<std::string>();
    char *node_id = new char[s_node_id.length() + 1];
    strcpy(node_id, s_node_id.c_str());

    auto sciter_result = sciter::value(_algonia_get_node_arg(node_id));

    delete node_id;
    return sciter_result;
}

sciter::value ZenUIBridge::algonia_set_node_arg(const sciter::value &raw_node_id, const sciter::value &raw_arg_val)
{
    auto s_node_id = raw_node_id.get<std::string>();
    char *node_id = new char[s_node_id.length() + 1];
    strcpy(node_id, s_node_id.c_str());

    auto s_arg_val = raw_arg_val.get<std::string>();
    char *arg_val = new char[s_arg_val.length() + 1];
    strcpy(arg_val, s_arg_val.c_str());

    _algonia_set_node_arg(node_id, arg_val);

    delete arg_val;
    delete node_id;

    return sciter::value();
}

sciter::value ZenUIBridge::algonia_execute_node(const sciter::value &text_val)
{
    auto s_node_id = text_val.get<std::string>();
    char *node_id = new char[s_node_id.length() + 1];
    strcpy(node_id, s_node_id.c_str());
    _algonia_execute_node(node_id);
    delete node_id;
    return sciter::value(); // returns undefined value, a.k.a. void
}