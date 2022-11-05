#ifndef INPUT_H
#define INPUT_H

typedef struct input_args {
    struct notcurses* nc;
    struct tres* tr;
} input_args;

void* input_run(void* args);



#endif

