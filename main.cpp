#define STB_IMAGE_IMPLEMENTATION
#define GLFW_EXPOSE_NATIVE_WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#pragma comment(lib, "legacy_stdio_definitions")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw\\lib-vc2010-32\\glfw3.lib")
#pragma comment(lib, "dwmapi.lib")

#include <Windows.h>
#include <dwmapi.h>
#include "glfw/include/GLFW/glfw3.h"
#include "glfw/include/GLFW/glfw3native.h"
#include "main.h"
#include "MemoryManager.h"
#include "Offsets.h"
#include "Structs.h"
#include <dinput.h>

MemoryManager* m;



View view;
float fovscale1 = 0.933;
float fovscale2 = 1.333;
int yoffset = 0;
std::vector<Vector3> Targets;
Vector3 PlayerPos;
Vector3 LastPlayerPos;
DWORD LocalPlayer;
ULONGLONG NoFallTimeout;
int pressed;

float debug1;
DWORD debugDWORD1;

float rand_FloatRange(float a, float b)
{
    return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}

Vector3 W2S(Vector3 WorldPos)
{
    Vector3 screen = Vector3(0, 0, 0);
    Vector3 vector = Vector3(0, 0, 0);
    Vector3 LocalPlayerPos = Vector3(view.vOrigin.x, view.vOrigin.y, view.vOrigin.z);
    Vector3 worldLocation = WorldPos - LocalPlayerPos;
    vector.x = worldLocation.Dot(view.vRight);
    vector.y = worldLocation.Dot(view.vUpward);
    vector.z = worldLocation.Dot(view.vForward);
    if ((double)vector.z >= 0.01)
    {
        screen.x = (float)(Width / 2) + (float)(Width / 2) / vector.z * fovscale1 * vector.x;
        screen.y = (float)(Height / 2) - (float)(Height / 2) / vector.z * fovscale2 * vector.y;
        return screen;
    }
    else return Vector3(0, 0, 0);
}

class ViewMatrix
{
public:
    float x1; //0x0000
    float x2; //0x0004
    float x3; //0x0008
    char pad_000C[4]; //0x000C
    float y1; //0x0010
    float y2; //0x0014
    float y3; //0x0018
    char pad_001C[4]; //0x001C
    float z1; //0x0020
    float z2; //0x0024
    float z3; //0x0028
    char pad_002C[4]; //0x002C
    float w1; //0x0030
    float w2; //0x0034
    float w3; //0x0038
    char pad_003C[4]; //0x003C
}; //Size: 0x0040
class WeaponSlot
{
public:
    uint32_t Type; //0x0000
    uint32_t State; //0x0004
    uint32_t AmmoInClip; //0x0008
    uint32_t AmmoRemaining; //0x000C
    char pad_0010[12]; //0x0010
}; //Size: 0x001C
struct CPed
{
    char pad_0000[20]; //0x0000
    class ViewMatrix* pViewMatrix; //0x0014
    char pad_0018[44]; //0x0018
    float SpeedVectorX; //0x0044
    float SpeedVectorY; //0x0048
    char pad_004C[116]; //0x004C
    void* CarNearest; //0x00C0
    char pad_00C4[936]; //0x00C4
    int8_t N0000037C; //0x046C
    char pad_046D[2]; //0x046D
    uint8_t CrouchState; //0x046F
    char pad_0470[208]; //0x0470
    float Health; //0x0540
    float MaxHealth; //0x0544
    char pad_0548[20]; //0x0548
    float Yaw;
    char pad_05482[64]; //0x0548
    class WeaponSlot WeaponSlot[13]; //0x05A0
    char pad_05A4[88]; //0x05A4
    CPed* CpedFired; //0x0764
    char pad_0768[92]; //0x0768

    void GetBonePosition(Vector3& outPosition, unsigned int boneId, bool updateSkinBones);
}; //Size: 0x07C4
struct CPedPoolInfo {
    CPed* FirstCPed;    //0x4
    uint8_t(*CPedsInUse)[140];//0x4
    uint32_t MaxCPed;//0x4
    uint32_t CurrentCPedNum;//0x4
};// Size: 0x10
void Hack()
{
    while (true)
    {
        Sleep(1);
        if (GetForegroundWindow() == GameHWND)
        {
            view.vRight = m->ReadMem<Vector3>(m->eDll.base + Offsets::ViewMatrix);
            view.vRight = -view.vRight;
            view.vUpward = m->ReadMem<Vector3>(m->eDll.base + Offsets::ViewMatrix + 12);
            view.vForward = m->ReadMem<Vector3>(m->eDll.base + Offsets::ViewMatrix + 24);
            view.vOrigin = m->ReadMem<Vector3>(m->eDll.base + Offsets::ViewMatrix + 36);
            PlayerPos = m->ReadMem<Vector3>(m->eDll.base + Offsets::PlayerOrigin);

            LocalPlayer = m->ReadMem<DWORD>(m->eDll.base + 0x007CC958);
            DWORD Pointer1 = m->ReadMem<DWORD>(LocalPlayer + 0x84);

            DWORD LocalPlayerPtr = m->ReadMem<DWORD>(m->eDll.base + Offsets::LocalPalyerPtr);

            

            if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pressed == 0))
            { 
                pressed = 1;
                if (NoFall)
                {
                    NoFallTimeout = GetTickCount64() + 1500;
                }
            }
            else if (GetAsyncKeyState(VK_SHIFT) == 0) pressed = 0;

            if (NoFall && NoFallTimeout < GetTickCount64())
            {
                m->WriteMem<float>(LocalPlayerPtr + Offsets::SpeedY, 0.f);
            }

            if (GetAsyncKeyState(VK_XBUTTON2))
            {
                Vector3 LocalPlayerPos = m->ReadMem<Vector3>(Pointer1 + 0xB04);//m->ReadMem<Vector3>(LocalPlayer + 0x30);
                Vector3 movedir = view.vOrigin - LocalPlayerPos;
                movedir = -(movedir * airbrakespeedX);
                Vector3 NewOrign = LocalPlayerPos + movedir;
                if (LocalPlayerPos.DistTo(NewOrign) < 3) m->WriteMem<Vector3>((Pointer1 + 0xB04), NewOrign);
            }
            if (GetAsyncKeyState(VK_XBUTTON1))
            {
                PlayerZ = PlayerZ + 0.02f;
                Vector3 LocalPlayerPos = m->ReadMem<Vector3>(Pointer1 + 0xB04);
                Vector3 NewOrign = Vector3(LocalPlayerPos.x, LocalPlayerPos.y, PlayerZ);
                if (LocalPlayerPos.DistTo(NewOrign) < 3) m->WriteMem<Vector3>((Pointer1 + 0xB04), NewOrign);
                LastPlayerPos = LocalPlayerPos;
                AirBrakeTimer = 1000;
            }
            else if (GetAsyncKeyState(VK_CONTROL))
            {
                AirBrakeTimer = 0;
                PlayerZ = PlayerZ - 0.01f;
                Vector3 LocalPlayerPos = m->ReadMem<Vector3>(Pointer1 + 0xB04);
                Vector3 NewOrign = Vector3(LocalPlayerPos.x, LocalPlayerPos.y, PlayerZ);
                if (LocalPlayerPos.DistTo(NewOrign) < 3) m->WriteMem<Vector3>((Pointer1 + 0xB04), NewOrign);
                LastPlayerPos = LocalPlayerPos;
            }
            else if (GetAsyncKeyState(VK_HOME))
            {
                m->WriteMem<float>(LocalPlayerPtr + Offsets::Health, 0.f);
            }
            else PlayerZ = PlayerPos.z;


            if (BotPoints.size())
            {
                if (Bot)
                {
                    if (GoingForResourse)
                    {

                        for (int i = 0; i < BotPoints.size(); i++)
                        {
                            if (PlayerPos.DistTo(BotPoints[i].Origin) < rand_FloatRange(BotPoints[i].mindist, BotPoints[i].maxdist))
                            {
                                if (i == BotPoints.size() - 1)
                                {
                                    GoingForResourse = false;
                                    ShouldStop = true;
                                    BotPointNumber = BotPoints.size() - 2;
                                    BotPoint = BotPoints[BotPointNumber].Origin;
                                }
                                else
                                {
                                    BotPointNumber = i + 1;
                                    BotPoint = BotPoints[BotPointNumber].Origin;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (BotPointNumber > 0)
                        {
                            if (PlayerPos.DistTo(BotPoints[BotPointNumber].Origin) < rand_FloatRange(BotPoints[BotPointNumber].mindist, BotPoints[BotPointNumber].maxdist))
                            {
                                BotPointNumber = BotPointNumber - 1;
                                BotPoint = BotPoints[BotPointNumber].Origin;
                            }
                        }
                        else
                        {
                            BotPointNumber = 0;
                            if (PlayerPos.DistTo(BotPoints[BotPointNumber].Origin) < rand_FloatRange(BotPoints[BotPointNumber].mindist, BotPoints[BotPointNumber].maxdist))
                                GoingForResourse = true;
                        }
                    }

                    Vector3 TargetOnScreenPos = W2S(BotPoint);
                    //RenderText(std::to_string(PlayerPos.DistTo(BotPoint)), ImVec2(TargetOnScreenPos.x, TargetOnScreenPos.y), 16.f, ImVec4(1.f, 1.f, 1.f, 1.f), true, fontEsp);

                    if (!ShouldStop)
                    {
                        float DistX = (TargetOnScreenPos.x - (Width / 2)) / BotSmooth;
                        float DistY = (TargetOnScreenPos.y - (Height / 2)) / BotSmooth;
                        if (PlayerPos.DistTo(BotPoints[BotPoints.size() - 1].Origin) < 4)
                        {
                            DistX = (TargetOnScreenPos.x - (Width / 2)) / 5.f;
                            DistY = (TargetOnScreenPos.y - (Height / 2)) / 5.f;
                        }
                        mouse_event(MOUSEEVENTF_MOVE, (int)DistX, (int)DistY, NULL, NULL);
                    }
                }
                else BotPoint = BotPoints[0].Origin;
            }
        }
    }
}

void Updater()
{
    BotPoints.push_back({ Vector3(-847, 1213, 3011), 1.3f, 1.7f, 1 });
    BotPoints.push_back({ Vector3(-808, 1213, 3011), 4.f, 7.f, 1 });
    BotPoints.push_back({ Vector3(-777, 1204, 3011),3.f, 4.5f, 1 });
    BotPoints.push_back({ Vector3(-770, 1193, 3011), 1.0f, 1.25f, 1 });
    while (!ShouldPanic)
    {
        GameHWND = FindWindow(GameClassName, NULL);
        if (GameHWND)
        {
            //GetWindowRect(GameHWND, &tSize);
            //Width = tSize.right - tSize.left;
            //Height = tSize.bottom - tSize.top;
            //DWORD dwStyle = GetWindowLong(GameHWND, GWL_STYLE);
            //if (dwStyle & WS_BORDER)
            //{
            //    tSize.top += 23;
            //    Height -= 23;
            //}
            //MoveWindow(hwnd, tSize.left, tSize.top + yoffset, Width, Height, true);
            //SetWindowPos(GameHWND, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

            if (GetAsyncKeyState(0x51))
            {
                NoFall = !NoFall;
                while (GetAsyncKeyState(0x51)) (Sleep(50));
            }
            if (GetAsyncKeyState(VK_F2))
            {
                WalkForward = !WalkForward;
                while (GetAsyncKeyState(VK_F2)) (Sleep(50));
                if (!WalkForward) PostMessage(GameHWND, WM_KEYUP, 0x57, 0);
            }
            if (GetAsyncKeyState(VK_F3))
            {
                Bot = !Bot;
                while (GetAsyncKeyState(VK_F3)) (Sleep(50));
                if (!Bot) PostMessage(GameHWND, WM_KEYUP, 0x57, 0);
            }
            if (GetAsyncKeyState(VK_NUMPAD1))
            {
                ArrowFlood = !ArrowFlood;
                while (GetAsyncKeyState(VK_NUMPAD1)) (Sleep(50));
                if (!ArrowFlood) PostMessage(GameHWND, WM_KEYUP, VK_UP, 0);
            }
            if (WalkForward)
            {
                m->WriteMem<int>(m->eDll.base + Offsets::IsGameWindowActive, 1);
                PostMessage(GameHWND, WM_KEYDOWN, 0x57, 0);
                if (!GetAsyncKeyState(VK_SHIFT))
                {
                    PostMessage(GameHWND, WM_KEYDOWN, VK_SPACE, 0);
                    Sleep(25);
                    PostMessage(GameHWND, WM_KEYUP, VK_SPACE, 0);
                }
                if (GoingRight != WalkForward && !WalkForward) PostMessage(GameHWND, WM_KEYUP, 0x57, 0);
                GoingRight = WalkForward;
            }
            if (ArrowFlood && GetForegroundWindow() == GameHWND)
            {
                INPUT input; // INPUT structure
                memset(&input, 0, sizeof(input));
                input.type = INPUT_KEYBOARD;
                input.ki.wScan = DIK_L;
                input.ki.time = 0;
                input.ki.dwExtraInfo = 0;
                input.ki.dwFlags = KEYEVENTF_SCANCODE;
                SendInput(1, &input, sizeof(INPUT));
                Sleep(140);
                input.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
                SendInput(1, &input, sizeof(INPUT));
                Sleep(10);
            }

            if (Bot)
            {
                PostMessage(GameHWND, WM_KEYDOWN, 0x57, 0);
                debug1 = PlayerPos.DistTo(BotPoint);
                if (debug1 > 50.5f)
                {
                    ShouldPanic = true;
                }
                if (GoingForResourse)
                {
                    if (PlayerPos.DistTo(BotPoint) > 10.f)
                    {
                        PostMessage(GameHWND, WM_KEYDOWN, VK_SPACE, 0);
                        Sleep(25);
                        PostMessage(GameHWND, WM_KEYUP, VK_SPACE, 0);
                    }
                }
                if (ShouldStop)
                {
                    PostMessage(GameHWND, WM_KEYUP, 0x57, 0);
                    Sleep(10000);
                    ShouldStop = false;
                }

                if (LastPlayerPos.DistTo(PlayerPos) < 0.2f)
                {
                    if (GoingForResourse)
                    {
                        PostMessage(GameHWND, WM_KEYDOWN, 0x41, 0);
                        Sleep(300);
                        PostMessage(GameHWND, WM_KEYUP, 0x41, 0);
                    }
                    else
                    {
                        PostMessage(GameHWND, WM_KEYDOWN, 0x44, 0);
                        Sleep(300);
                        PostMessage(GameHWND, WM_KEYUP, 0x44, 0);
                    }
                }
                LastGoingRight = GoingRight;
                LastPlayerPos = PlayerPos;
            }
        }
        else
        {
            m->~MemoryManager();
            delete m;
            Sleep(3000);
            GameHWND = FindWindow(GameClassName, NULL);
            if (GameHWND) m = new MemoryManager;
            else ExitProcess(EXIT_SUCCESS);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    }
    if (Bot && ShouldPanic)
    {
        PostMessage(GameHWND, WM_KEYUP, 0x57, 0);
        Sleep(1500);
        SendChat("wtf?");
        Sleep(1500);
        TerminateProcess(m->hProc, 1);
        ExitProcess(EXIT_SUCCESS);
    }
}

int main(int, char**)
{
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);

    std::string randomclassname = " ";
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, randomclassname.c_str(), NULL, NULL);
    if (!window) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    hwnd = glfwGetWin32Window(window);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImFont* DefaultFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 20.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
    ImFont* SmallFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 14.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
    GetMyStyle();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    bool open = true;
    DWORD dwFlag = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	m = new MemoryManager;;
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Hack, 0, 0, 0);
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Updater, 0, 0, 0);


    while (!glfwWindowShouldClose(window) && open)
    {
        glfwPollEvents();
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight), ImGuiCond_Once);
        ImGui::Begin("INTERIUM - SAMP", &open, dwFlag);
        ImGui::Checkbox("WalkForward", &WalkForward);
        ImGui::SliderFloat("AirBrake X Speed", &airbrakespeedX, 0.f, 0.2f);
        ImGui::SliderFloat("AirBrake Y Speed", &airbrakespeedY, 0.f, 0.2f);
        ImGui::Checkbox("No fall", &NoFall);
        ImGui::Checkbox("ArrowFlood", &ArrowFlood);

        ImGui::Spacing();

        ImGui::Checkbox("Bot", &Bot);
        ImGui::Checkbox("Panic", &ShouldPanic);

        ImGui::Text(std::to_string(PlayerPos.x).c_str()); ImGui::SameLine();
        ImGui::Text(std::to_string(PlayerPos.y).c_str()); ImGui::SameLine();
        ImGui::Text(std::to_string(PlayerPos.z).c_str());
        ImGui::Text(std::to_string(GoingForResourse).c_str());
        ImGui::Text(std::to_string(BotPointNumber).c_str());
        ImGui::Text(std::to_string(debug1).c_str());

        //CPedPoolInfo PoolInfo = m->ReadMem<CPedPoolInfo>(m->eDll.base + 0x774490);
        //CPed BestTarget = m->ReadMem<CPed>(m->eDll.base + 0x774490 + 0x4);
        //ImGui::Text(("Fisrt ped: " + std::to_string(BestTarget.Health)).c_str());
        //for (int i = 2; i < 140; i++)
        //{
        //    CPed ped = m->ReadMem<CPed>(m->eDll.base + 0x774490 + 0x4 + (0x7c4 * i));
        //    if(ped.Health < 120.f && ped.Health > 1.f) ImGui::Text(std::to_string(ped.Health).c_str());
        //}
        //DWORD BestTarget = m->ReadMem<DWORD>(m->eDll.base + 0x774490 + 0x4);
        //for (int i = 0; i < 140; i++)
        //{
        //    CPed ped = m->ReadMem<CPed>(BestTarget + (0x7c4 * i));
        //    if(ped.Health < 120.f && ped.Health > 0.1f) ImGui::Text(std::to_string(ped.Health).c_str());
        //}


        ImGui::End();

        ImGui::EndFrame();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

