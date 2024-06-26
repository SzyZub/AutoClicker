#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <Windows.h>
#include <chrono>
#include <thread>

class MyFrame : public wxFrame
{
    HHOOK hook;
    HHOOK MouseHook;
    int timer, type;
    int press;
    std::thread clicking;
    wxBoxSizer* vertBox;
    wxPanel* panel;
    bool triggerNextClickAction;
    int clicked;
    static MyFrame* instance;
public:
    MyFrame() : wxFrame(nullptr, wxID_ANY, "AutoClicker", wxDefaultPosition, wxSize(800,400)) {
        Center();
        _mainFunc();
        instance = this;
        timer = 1000;
        triggerNextClickAction = false;
        hook = SetWindowsHookExA(WH_KEYBOARD_LL, _Call, NULL, 0);
        MouseHook = SetWindowsHookEx(WH_MOUSE_LL, _MouseProc, NULL, 0);
        clicked = 0;
    };
    void _mainFunc() {
        panel = new wxPanel(this, -1);
        vertBox = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(vertBox);
        _DrawInstructions();
        _DrawSLider();
        _DrawLoopType();
        _DrawMouseButton();
    }
    void _DrawInstructions() {
        wxStaticText* info = new wxStaticText(panel, wxID_ANY, "Clicking F4 makes you toggle on/off the autoclicker");
        vertBox->Add(info, 1, wxALL | wxALIGN_CENTRE_HORIZONTAL, 0);
    }
    void _DrawSLider() {
        wxBoxSizer* timBox = new wxBoxSizer(wxVERTICAL);
        wxSlider* tim = new wxSlider(panel, 2, 1000, 5, 10000, wxDefaultPosition, wxDefaultSize, wxSL_LABELS);
        wxStaticText* timLabel = new wxStaticText(panel, wxID_ANY, "Miliseconds between clicks");
        timBox->Add(timLabel, 0, wxALL | wxALIGN_CENTRE_HORIZONTAL, 10);
        timBox->Add(tim, 5, wxEXPAND, 0);
        tim->Bind(wxEVT_SLIDER, &MyFrame::_OnSlider, this);
        vertBox->Add(timBox, 3, wxEXPAND, 0);
    }
    void _DrawLoopType() {
        wxBoxSizer* TypeBox = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer* LeftTypeBox = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* RightTypeBox = new wxBoxSizer(wxVERTICAL);
        wxCheckBox* amountCheck = new wxCheckBox(panel, 4, "Click a certain amount of times");
        wxSpinCtrl* amount = new wxSpinCtrl(panel, 5, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxSP_WRAP, 1, 10000);
        wxCheckBox* posCheck = new wxCheckBox(panel, 6, "Click at this position");
        wxSpinCtrl* x = new wxSpinCtrl(panel, 7, "x", wxDefaultPosition, wxDefaultSize, wxSP_WRAP, 0, 4000);
        wxSpinCtrl* y = new wxSpinCtrl(panel, 8, "y", wxDefaultPosition, wxDefaultSize, wxSP_WRAP, 0, 2000);
        wxButton* aim = new wxButton(panel, 9, "Indicate a position");
        LeftTypeBox->Add(amountCheck, 1, wxEXPAND, 0);
        LeftTypeBox->Add(amount, 3, wxEXPAND, 0);
        RightTypeBox->Add(posCheck, 1, wxEXPAND, 0);
        RightTypeBox->Add(x, 1, wxEXPAND, 0);
        RightTypeBox->Add(y, 1, wxEXPAND, 0);
        RightTypeBox->Add(aim, 1, wxEXPAND, 0);
        TypeBox->Add(LeftTypeBox, 1, wxEXPAND, 0);
        TypeBox->Add(RightTypeBox, 1, wxEXPAND, 0);
        vertBox->Add(TypeBox, 3, wxEXPAND, 0);
        aim->Bind(wxEVT_BUTTON, &MyFrame::_MousePos, this);
    }
    void _MousePos(wxCommandEvent& event) {
        triggerNextClickAction = true;
    }
    void _DrawMouseButton() {
        wxString arr[] = { "Left Mouse Button", "Middle Mouse Button", "Right Mouse Button" };
        wxRadioBox* typeClick = new wxRadioBox(panel, 3, "Which mouse button should be clicked", wxDefaultPosition, wxDefaultSize, 3, arr);
        typeClick->Bind(wxEVT_RADIOBOX, &MyFrame::_OnRadio, this);
        vertBox->Add(typeClick, 3, wxEXPAND, 0);
    }
    void _OnSlider(wxCommandEvent& event) {
        timer = event.GetInt();
    }
    void _OnRadio(wxCommandEvent& event) {
        type = event.GetInt();
    }
    void _startClicking() {
        press = 1;
        clicked = ((wxSpinCtrl*)FindWindowById(5))->GetValue();
        if (clicking.joinable())
            clicking.join();
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
            if (((wxCheckBox*)FindWindowById(6))->GetValue()) {
                SetCursorPos(((wxSpinCtrl*)FindWindowById(7))->GetValue(), ((wxSpinCtrl*)FindWindowById(8))->GetValue());
            }
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
            if (((wxCheckBox*)FindWindowById(4))->GetValue()) { 
                clicked--;
                if (clicked == 0) {
                    press = 0;
                }
            }
        }
    }
    static LRESULT CALLBACK _Call(int nCode, WPARAM wParam, LPARAM lParam)
    {
        PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
        if (wParam == WM_KEYDOWN && nCode == HC_ACTION)
        {
            if (key->vkCode == VK_F4) {
                if (instance->press == 1) {
                    instance->_stopClicking();
                }
                else {
                    instance->_startClicking();
                }
            }
        }
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }
    static LRESULT CALLBACK _MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if ((nCode >= 0 && (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN) && instance->triggerNextClickAction)) {
            POINT pt;
            GetCursorPos(&pt);
            ((wxSpinCtrl*)FindWindowById(7))->SetValue(pt.x);
            ((wxSpinCtrl*)FindWindowById(8))->SetValue(pt.y);
            instance->triggerNextClickAction = false;
        }
        return CallNextHookEx(instance->MouseHook, nCode, wParam, lParam);
    }
    ~MyFrame() {
        _stopClicking();
    }
};

MyFrame* MyFrame::instance = nullptr;

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

