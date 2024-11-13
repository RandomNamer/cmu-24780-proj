//
// Created by Zeyu Zhang on 11/12/24.
//
#include "fssimplewindow.h"
#include <iostream>

int main() {
    FsOpenWindow(16,16,800,600,1);
    while(FSKEY_NULL==FsInkey()) {
        FsPollDevice();
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
        glBegin(GL_QUADS);
        glColor3ub(255,0,0);
        glVertex2i(100,100);
        glVertex2i(700,100);
        glVertex2i(700,500);
        glVertex2i(100,500);
        glEnd();
        FsSwapBuffers();
        FsSleep(25);
    }
}
