#include "dummy_scene.h"
#include "main_loop.h"

int main(void)
{
    enter_main_loop(add_dummy_scene_component);
    return 0;
}
