#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

// 播放 beep.wav
void PlayBeepSound() {
    PlaySound(L"beep.wav", nullptr, SND_FILENAME | SND_ASYNC);
}

// 入口函数（隐藏窗口）
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 隐藏控制台窗口（如果程序以控制台模式启动）
    FreeConsole();

    while (true) {
        PlayBeepSound();
        Sleep(13 * 60 * 1000); // 13分钟 = 780000 毫秒
    }

    return 0;
}
