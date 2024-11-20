//
// Created by Zeyu Zhang on 11/12/24.
//
#include "fssimplewindow.h"
#include "Human.h"
#include <iostream>

int main() {
    FsOpenWindow(16,16,800,600,1);
    Human::Config config = {1, 9.8, 0.1, 0, {30, -15}, {400, 300}, Human::DEFAULT};
    Human human(config);
    FsPassedTime();
    while(FSKEY_ESC!=FsInkey()) {
        FsPollDevice();
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
        human.input(FsInkey());
        human.update(FsPassedTime());
        human.draw({0,0});
        FsSwapBuffers();
        FsSleep(25);
    }
}
