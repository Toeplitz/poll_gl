#pragma once
#include "poll.h"

extern void common_debug_use();
extern void common_init(Poll &poll);
extern void common_fpcamera_use(Node *node);

extern Poll *p;
