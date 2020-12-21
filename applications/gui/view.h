#pragma once

typedef struct View View;

View* view_alloc();

void view_free(View* view);
