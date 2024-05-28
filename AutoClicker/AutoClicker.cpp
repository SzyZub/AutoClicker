#include <wx/wx.h>
#include <Windows.h>
#include <chrono>
#include <thread>

class MyFrame : public wxFrame
{
    HHOOK hook;
    int timer, type;
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
        wxStaticText* info = new wxStaticText(panel, wxID_ANY, "Clicking F4 makes you toggle on/off the autoclicker");
        vertBox->Add(info, 1, wxALL | wxALIGN_CENTRE_HORIZONTAL, 0);
        wxBoxSizer* timBox = new wxBoxSizer(wxVERTICAL);
        wxSlider* tim = new wxSlider(panel, 2, 1000, 5, 10000, wxDefaultPosition, wxDefaultSize, wxSL_LABELS);
        wxStaticText* timLabel = new wxStaticText(panel, wxID_ANY, "Miliseconds between clicks");
        timBox->Add(timLabel, 0, wxALL | wxALIGN_CENTRE_HORIZONTAL, 10);
        timBox->Add(tim, 5, wxEXPAND, 0);
        tim->Bind(wxEVT_SLIDER, &MyFrame::_OnSlider, this);
        vertBox->Add(timBox, 3, wxEXPAND, 0);
        wxString arr[] = {"Left Mouse Button", "Middle Mouse Button", "Right Mouse Button"};
        wxRadioBox* typeClick = new wxRadioBox(panel, 3, "Which mouse button should be clicked", wxDefaultPosition, wxDefaultSize, 3, arr);
        typeClick->Bind(wxEVT_RADIOBOX, &MyFrame::_OnRadio, this);
        vertBox->Add(typeClick, 3, wxEXPAND, 0);
    }
    void _OnSlider(wxCommandEvent &event) {
        timer = event.GetInt();
    }
    void _OnRadio(wxCommandEvent& event) {
        type = event.GetInt();
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
            DWORD down;
            DWORD up;
            switch (type) {
            case 0:
                down = MOUSEEVENTF_LEFTDOWN;
                up = MOUSEEVENTF_LEFTUP;
                break;
            case 1:
                down = MOUSEEVENTF_MIDDLEDOWN;
                up = MOUSEEVENTF_MIDDLEUP;
                break;
            case 2:
                down = MOUSEEVENTF_RIGHTDOWN;
                up = MOUSEEVENTF_RIGHTUP;
                break;
            }
            input.mi.dwFlags = down;
            SendInput(1, &input, sizeof(INPUT));
            input.mi.dwFlags = up;
            SendInput(1, &input, sizeof(INPUT));
            std::this_thread::sleep_for(std::chrono::milliseconds(timer));
        }
    }
    static LRESULT CALLBACK _Call(int nCode, WPARAM wParam, LPARAM lParam)
    {
        PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
        if (wParam == WM_KEYDOWN && nCode == HC_ACTION)
        {
            if (key->vkCode == VK_F4) {
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

