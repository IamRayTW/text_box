#include <cstdio>
#include "TextBox.h"

#pragma comment(lib, "opengl32.lib")

int main()
{
    TextBox text_box;
    text_box.SetFontSize(18);
    text_box.RunUI(222, 222);
    printf("end\n");
}