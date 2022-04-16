#pragma once
#include <Windows.h>
#include <chrono>
#include <thread>
#include <string>
#include <sstream>
#include <vector>

#include "glfw/include/GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl2.h"
#include "stb_image.h"

#include "Structs.h"
#include <dinput.h>

static HWND GameHWND = NULL;
static const char* GameClassName = "Grand theft auto San Andreas";//"Malinovka Client (SA-MP 0.3.7-R3)";
static RECT tSize;

static int Width = 640;
static int Height = 480;
static bool open = true;

static int WindowWidth = 300;
static int WindowHeight = 300;
static HWND hwnd = NULL;



static bool Bot = false;
static bool LastGoingRight = true;
static bool GoingRight = true;
static bool ShouldPanic = false;
static bool GoingForResourse = true;
static std::vector<Waypoint> BotPoints;
static Vector3 BotPoint;
static int BotPointNumber = 0;
static bool ShouldStop = false;
static float BotSmooth = 20.f;

static bool WalkForward = false;

static float airbrakespeedX = 0.005f;
static float airbrakespeedY = 0.02f;
static float PlayerZ;
static int AirBrakeTimer = 1500;

static bool NoFall = false;
static bool ArrowFlood = false;

static void SendChat(const char* Message)
{
	INPUT input; // INPUT structure
	memset(&input, 0, sizeof(input));
	input.type = INPUT_KEYBOARD;

	input.ki.wScan = DIK_F6;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.dwFlags = KEYEVENTF_SCANCODE;
	SendInput(1, &input, sizeof(INPUT));
	input.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
	SendInput(1, &input, sizeof(INPUT));

	//MessageBox(NULL, std::to_string(strlen(Message)).c_str(), NULL, NULL);
	std::this_thread::sleep_for(std::chrono::milliseconds(80));
	for (int i = 0; i < strlen(Message); i++)
	{
		input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = VkKeyScanA(Message[i]);
		SendInput(1, &input, sizeof(INPUT));
		std::this_thread::sleep_for(std::chrono::milliseconds(8));
		input.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &input, sizeof(INPUT));
	}

	input.ki.wScan = 0x1C;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.dwFlags = KEYEVENTF_SCANCODE;
	SendInput(1, &input, sizeof(INPUT));
	input.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
	SendInput(1, &input, sizeof(INPUT));
}

////////////////////////////////////////////
///////////// OpenGL  Shit /////////////////
////////////////////////////////////////////

static void glfw_error_callback(int error, const char* description) //можно удалить
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL) return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Upload pixels into texture
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}

static bool LoadTextureFromMemory(unsigned char* MappedFile, int FileSize, GLuint* out_texture, int* out_width, int* out_height)
{
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load_from_memory(MappedFile, FileSize, &image_width, &image_height, NULL, 4);

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Upload pixels into texture
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}


static void GetMyStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	{
		style.Alpha = 1.f;
		style.WindowPadding = ImVec2(0, 0);
		style.WindowRounding = 0.0f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.ChildRounding = 3.0f;
		style.FrameBorderSize = 1;
		style.FramePadding = ImVec2(4, 3);
		style.FrameRounding = 5;
		style.ItemSpacing = ImVec2(8, 0);
		style.ItemInnerSpacing = ImVec2(4, 4);
		style.TouchExtraPadding = ImVec2(0, 0);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 6.0f;
		style.ScrollbarSize = 10.0f;
		style.ScrollbarRounding = 3.0f;
		style.GrabMinSize = 10.0f;
		style.GrabRounding = 3.0f;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.DisplayWindowPadding = ImVec2(22, 22);
		style.DisplaySafeAreaPadding = ImVec2(4, 4);
		style.AntiAliasedLines = true;

		// Setup style
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);						//Цвет текста
		colors[ImGuiCol_TextDisabled] = ImColor(53, 53, 53, 255);
		colors[ImGuiCol_WindowBg] = ImColor(25, 25, 25, 255);						// Задний фон окна
		colors[ImGuiCol_ChildBg] = ImColor(25, 25, 25, 255);						// Задний фон "ребенка" (Child)
		colors[ImGuiCol_PopupBg] = ImColor(25, 25, 25, 255);
		colors[ImGuiCol_Border] = ImColor(36, 35, 35, 255);							//Обводка прямоуголника приложения и айтемом
		colors[ImGuiCol_BorderShadow] = ImColor(0, 0, 0, 200);						//Тень от обводки
		colors[ImGuiCol_FrameBg] = ImColor(20, 20, 20, 200);						// Задний фон слайдеров, кнопок и прочей херни
		colors[ImGuiCol_FrameBgHovered] = ImColor(140, 0, 0, 255);					// Когда наведен курсор
		colors[ImGuiCol_FrameBgActive] = ImColor(255, 0, 0, 175);					//Когда нажато
		colors[ImGuiCol_TitleBg] = ImColor(15, 15, 15, 255);						//Верхняя полоска с название программы когда окно неактивно
		colors[ImGuiCol_TitleBgActive] = ImColor(20, 20, 20, 255);					//Когда активно
		colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 0, 0, 255);
		colors[ImGuiCol_MenuBarBg] = ImColor(25, 25, 25, 255);
		colors[ImGuiCol_ScrollbarBg] = ImColor(20, 20, 20, 255);					//Ползунок(ScrollBar) задний фон
		colors[ImGuiCol_ScrollbarGrab] = ImColor(242, 0, 0, 232);					//Ползунок(ScrollBar) при нажатии??
		colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(241, 0, 0, 174);			//Ползунок(ScrollBar) при наведении	
		colors[ImGuiCol_ScrollbarGrabActive] = ImColor(255, 0, 0, 133);				//Ползунок(ScrollBar) при перетягивании	
		colors[ImGuiCol_CheckMark] = ImColor(255, 0, 0, 253);						//Галочка (CheckBox)	//ImVec4(0.8f, 0.15f, 0.15f, 1.00f);  //CheckBox 
		colors[ImGuiCol_SliderGrab] = ImColor(246, 0, 0, 255);						// Слайдер когда нажимаем?
		colors[ImGuiCol_SliderGrabActive] = ImColor(255, 0, 0, 175);				// Слайдер когда перетягиваем
		colors[ImGuiCol_Button] = ImColor(20, 20, 20, 249);							// Кнопка 
		colors[ImGuiCol_ButtonHovered] = ImColor(140, 0, 0, 223);					// Кнопка при наведении
		colors[ImGuiCol_ButtonActive] = ImColor(0, 0, 0, 168);				        // Кнопка при нажамтии
		colors[ImGuiCol_Header] = ImColor(255, 0, 0, 175);							// Выбранный айтем из комбобокса
		colors[ImGuiCol_HeaderHovered] = ImColor(244, 0, 0, 223);
		colors[ImGuiCol_HeaderActive] = ImColor(255, 0, 0, 255);
		colors[ImGuiCol_Separator] = ImColor(0, 0, 0, 78);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.81f, 0.81f, 0.81f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.75f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.98f, 0.26f, 0.26f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.98f, 0.26f, 0.26f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.87f, 0.87f, 0.87f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.01f, 0.01f, 0.01f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.77f, 0.33f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.87f, 0.55f, 0.08f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.47f, 0.60f, 0.76f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.58f, 0.58f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
	}
}