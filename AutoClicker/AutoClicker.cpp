#include <wx/wx.h>
#include <Windows.h>
#include <chrono>
#include <thread>

class MyFrame : public wxFrame
{
    HHOOK hook;
    static int press;
public:
    MyFrame() : wxFrame(nullptr, wxID_ANY, "AutoClicker", wxDefaultPosition, wxSize(800,400)) {
        Center();
        _mainFunc();
        hook = SetWindowsHookExA(WH_KEYBOARD_LL, _Call, NULL, 0);
    };
    void _mainFunc() {
        wxPanel* panel = new wxPanel(this, -1);
        wxBoxSizer* vertBox = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(vertBox);
    }
    static LRESULT CALLBACK _Call(int nCode, WPARAM wParam, LPARAM lParam)
    {
        PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
        if (wParam == WM_KEYDOWN && nCode == HC_ACTION)
        {
            switch (key->vkCode)
            {
            case VK_F5:
                press = 1;
                break;
            case VK_F6:
                press = 0;
                break;
            }
        }
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }
    void _Click() {
        while (press) {

        }
    }
};

int MyFrame::press = 0;

class MyApp : public wxApp
{
public:
    bool OnInit() {
        MyFrame* Frame = new MyFrame();
        Frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);

