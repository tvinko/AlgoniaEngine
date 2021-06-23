#include "ZenCommon.h"
#include "ZenCoreCLR.h"

void init_web_view(void (*algonia_callback)(char *), void (*algonia_free)(void), char *);
char *algonia_callback(char *arg);