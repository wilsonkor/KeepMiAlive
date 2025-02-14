#include <windows.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <iostream>

#include <endpointvolume.h>

// ����������Ƶ���ʱ�䣨10 ���ӣ�
constexpr int NO_SOUND_TIMEOUT_MS = 10 * 60 * 1000; // 10 ����

std::string beepFilePath = "./beep.wav";

// ����Ƿ�����Ƶ����
bool IsAudioPlaying() {
    HRESULT hr;

    // ��ʼ�� COM
    CoInitialize(nullptr);

    // ��ȡĬ����Ƶ��Ⱦ�豸
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

    // ��ȡ��Ƶ������Ƶ�ͻ���
    IAudioMeterInformation* pMeterInfo = nullptr;
    hr = pDevice->Activate(__uuidof(IAudioMeterInformation), CLSCTX_ALL, nullptr, (void**)&pMeterInfo);
    pDevice->Release();
    if (FAILED(hr)) {
        CoUninitialize();
        return false;
    }

    // ��ȡ��Ƶ��ֵ
    float peakValue = 0.0f;
    hr = pMeterInfo->GetPeakValue(&peakValue);
    pMeterInfo->Release();
    CoUninitialize();
    if (FAILED(hr)) {
        return false;
    }

    // �����ֵ���� 0����˵������Ƶ����
    return peakValue > 0.0f;
}

// ���� beep.wav �ļ�
void PlayBeepSound() {
    if (GetFileAttributesA(beepFilePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        PlaySoundA(beepFilePath.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
    }
    else {
        // beep.wav �ļ������ڣ����Լ�¼������ȡ������ʩ
        std::cerr << "Error: beep.wav file not found in the current directory." << std::endl;
    }
}

// ���������
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ULONGLONG startTime = GetTickCount64(); // ��ʼ��ʱ

    // �����û�� beep.wav�ļ� û�е�����ʾ
    if (GetFileAttributesA(beepFilePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
		// �����Ի���
        MessageBoxA(NULL, "Error: beep.wav file not found in the current directory.", "Error", MB_OK | MB_ICONERROR);
		return 1;
	}
    PlayBeepSound();

    while (true) {
        // ����Ƿ�����Ƶ����
        if (IsAudioPlaying()) {
            ULONGLONG startTime = GetTickCount64(); // ��������������ü�ʱ��
        }
        else {
            // ����Ƿ񳬹� 10 ����������
            if (GetTickCount64() - startTime >= NO_SOUND_TIMEOUT_MS) {
                // ���� Beep ����
                PlayBeepSound();
                startTime = static_cast<DWORD>(GetTickCount64()); // ���ü�ʱ��
            }
        }

        // ÿ�� 1 ����һ��
        Sleep(1000);
    }

    return 0;
}