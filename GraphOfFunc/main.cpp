#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

int main()
{
	// параметры экрана, константы

	const int screenWidth = 800;
	const int screenHeight = 600;
	const char* windowTitle = "Graph Renderer";

	InitWindow(screenWidth, screenHeight, windowTitle); //инициализация окна

	SetTargetFPS(60); //FPS

	// переменные gui

	char inputText[64] = "sin(x)";   // буфер для хранения функции
	bool inputMode = false;          // режим ввода
	bool plotButtonPressed = false;  // состояние кнопки построения графика

	std::string func;

	while (!WindowShouldClose()) //основной цикл приложения
	{
		BeginDrawing(); //начало отрисовки

		ClearBackground(RAYWHITE); //заливка бэкграунда
		
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


		EndDrawing(); //конец отрисовки
	}

	CloseWindow(); //закрытые окна

	return 0;
}