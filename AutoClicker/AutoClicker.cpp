#include <wx/wx.h>
#include <Windows.h>
#include <chrono>
#include <thread>

class MyFrame : public wxFrame
{
    HHOOK hook;
    int timer;
    static int press;
    std::thread clicking;
    static MyFrame* instance;
public:
    MyFrame() : wxFrame(nullptr, wxID_ANY, "AutoClicker", wxDefaultPosition, wxSize(800,400)) {
        Center();
        _mainFunc();
        instance = this;
        timer = 1000;
        hook = SetWindowsHookExA(WH_KEYBOARD_LL, _Call, NULL, 0);
    };
    void _mainFunc() {
        wxPanel* panel = new wxPanel(this, -1);
        wxBoxSizer* vertBox = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(vertBox);
        wxSlider* tim = new wxSlider(panel, 2, timer, 10, 10000);
        vertBox->Add(tim, 1, wxEXPAND, 0);
    }
    void _startClicking() {
        press = 1;
        clicking = std::thread(&MyFrame::_clickLoop, this);
    }
    void _stopClicking() {
        press = 0;
        if (clicking.joinable())
            clicking.join();;
    }
    void _clickLoop() {
        while (press)
        {
            INPUT input = { 0 };
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            SendInput(1, &input, sizeof(INPUT));
            input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            SendInput(1, &input, sizeof(INPUT));
            std::this_thread::sleep_for(std::chrono::milliseconds(timer));
        }
    }
    static LRESULT CALLBACK _Call(int nCode, WPARAM wParam, LPARAM lParam)
    {
        PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
        if (wParam == WM_KEYDOWN && nCode == HC_ACTION)
        {
            if (key->vkCode == VK_F5) {
                if (press == 1) {
                    instance->_stopClicking();
                }
                else {
                    instance->_startClicking();
                }
            }
        }
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }
};

MyFrame* MyFrame::instance = nullptr;
int MyFrame::press = 0;

class MyApp : public wxApp
{
public:
    bool OnInit() {
        MyFrame* frame = new MyFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);

