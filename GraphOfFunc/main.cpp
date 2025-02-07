#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

int main()
{
	// ��������� ������, ���������

	const int screenWidth = 800;
	const int screenHeight = 600;
	const char* windowTitle = "Graph Renderer";

	InitWindow(screenWidth, screenHeight, windowTitle); //������������� ����

	SetTargetFPS(60); //FPS

	// ���������� gui

	char inputText[64] = "sin(x)";   // ����� ��� �������� �������
	bool inputMode = false;          // ����� �����
	bool plotButtonPressed = false;  // ��������� ������ ���������� �������

	std::string func;

	while (!WindowShouldClose()) //�������� ���� ����������
	{
		BeginDrawing(); //������ ���������

		ClearBackground(RAYWHITE); //������� ����������
		
		if (GuiTextBox({ 10, 10, 200, 30 }, inputText, 64, inputMode))
		{
			inputMode = !inputMode;
		}

		if (GuiButton({ 220, 10, 120, 30 }, "Build graph"))
		{
			func = inputText;
		}

			DrawText(TextFormat("Function: %s", func.c_str()), 10, 50, 20, BLACK);
			plotButtonPressed = true;


		EndDrawing(); //����� ���������
	}

	CloseWindow(); //�������� ����

	return 0;
}