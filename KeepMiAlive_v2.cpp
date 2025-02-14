#include <windows.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <iostream>

#include <endpointvolume.h>

// 定义检测无音频活动的时间（10 分钟）
constexpr int NO_SOUND_TIMEOUT_MS = 10 * 60 * 1000; // 10 分钟

std::string beepFilePath = "./beep.wav";

// 检测是否有音频播放
bool IsAudioPlaying() {
    HRESULT hr;

    // 初始化 COM
    CoInitialize(nullptr);

    // 获取默认音频渲染设备
    IMMDeviceEnumerator* pEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator));
    if (FAILED(hr)) {
        CoUninitialize();
        return false;
    }

    IMMDevice* pDevice = nullptr;
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    pEnumerator->Release();
    if (FAILED(hr)) {
        CoUninitialize();
        return false;
    }

    // 获取音频流的音频客户端
    IAudioMeterInformation* pMeterInfo = nullptr;
    hr = pDevice->Activate(__uuidof(IAudioMeterInformation), CLSCTX_ALL, nullptr, (void**)&pMeterInfo);
    pDevice->Release();
    if (FAILED(hr)) {
        CoUninitialize();
        return false;
    }

    // 获取音频峰值
    float peakValue = 0.0f;
    hr = pMeterInfo->GetPeakValue(&peakValue);
    pMeterInfo->Release();
    CoUninitialize();
    if (FAILED(hr)) {
        return false;
    }

    // 如果峰值大于 0，则说明有音频播放
    return peakValue > 0.0f;
}

// 播放 beep.wav 文件
void PlayBeepSound() {
    if (GetFileAttributesA(beepFilePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        PlaySoundA(beepFilePath.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
    }
    else {
        // beep.wav 文件不存在，可以记录错误或采取其他措施
        std::cerr << "Error: beep.wav file not found in the current directory." << std::endl;
    }
}

// 主程序入口
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ULONGLONG startTime = GetTickCount64(); // 开始计时

    // 检查有没有 beep.wav文件 没有弹出提示
    if (GetFileAttributesA(beepFilePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
		// 弹出对话框
        MessageBoxA(NULL, "Error: beep.wav file not found in the current directory.", "Error", MB_OK | MB_ICONERROR);
		return 1;
	}
    PlayBeepSound();

    while (true) {
        // 检测是否有音频播放
        if (IsAudioPlaying()) {
            ULONGLONG startTime = GetTickCount64(); // 如果有声音，重置计时器
        }
        else {
            // 检查是否超过 10 分钟无声音
            if (GetTickCount64() - startTime >= NO_SOUND_TIMEOUT_MS) {
                // 发出 Beep 声音
                PlayBeepSound();
                startTime = static_cast<DWORD>(GetTickCount64()); // 重置计时器
            }
        }

        // 每隔 1 秒检测一次
        Sleep(1000);
    }

    return 0;
}