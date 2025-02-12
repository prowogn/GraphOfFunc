#define _CRT_SECURE_NO_WARNINGS
#include <iostream>               // да
#include <algorithm>              // min, max
#include <raylib.h>               // функционал
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>               // интерфейс
#include "../vcpkg_installed/x64-windows/include/raymath.h"


template <typename T>
T Clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float CalculateGridStep(float zoom) {
    // Базовый шаг сетки (при zoom = 1.0)
    float baseStep = 50.0f;

    // Динамически изменяем шаг в зависимости от зума
    float step = baseStep / zoom;

    // Округляем шаг до ближайшего "красивого" значения (10, 20, 50, 100 и т.д.)
    float scale = powf(10.0f, floorf(log10f(step)));
    step = roundf(step / scale) * scale;

    return step;
}

void DrawGrid(Camera2D camera, int screenWidth, int screenHeight) {
    // получаем границы видимой области в мировых координатах
    Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera);
    Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera);
    Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera);
    Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera);

    float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
    float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });

    // Вычисляем толщину линий сетки аналогично осевым (например, сетка в два раза тоньше осей)
    float gridThickness = (2.0f / camera.zoom) * 0.5f; // или просто 1.0f / camera.zoom

    // Вычисляем шаг сетки
    float gridStep = CalculateGridStep(camera.zoom);

    // Рисуем вертикальные линии
    for (float x = gridStep; x < maxX; x += gridStep) {
        DrawLineEx({ x, minY }, { x, maxY }, gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    }
    for (float x = -gridStep; x > minX; x -= gridStep) {
        DrawLineEx({ x, minY }, { x, maxY }, gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    }

    // Рисуем горизонтальные линии
    for (float y = gridStep; y < maxY; y += gridStep) {
        DrawLineEx({ minX, y }, { maxX, y }, gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    }
    for (float y = -gridStep; y > minY; y -= gridStep) {
        DrawLineEx({ minX, y }, { maxX, y }, gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    }
}

void DrawGridLabels(Camera2D camera, int screenWidth, int screenHeight) {
    // Получаем границы видимой области
    Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera);
    Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera);
    Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera);
    Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera);

    float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
    float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });

    // Шаг сетки
    float gridStep = CalculateGridStep(camera.zoom);

    // Рассчитываем размер шрифта
    int fontSize = (int)(12 / camera.zoom);
    fontSize = Clamp(fontSize, 8, 24);

    // Подписи для вертикальных линий (ось X)
    for (float x = gridStep; x < maxX; x += gridStep) {
        Vector2 screenPos = GetWorldToScreen2D({ x, 0 }, camera);
        DrawText(TextFormat("%.1f", x), screenPos.x + 5, screenPos.y - 20, fontSize, DARKGRAY);
    }
    for (float x = -gridStep; x > minX; x -= gridStep) {
        Vector2 screenPos = GetWorldToScreen2D({ x, 0 }, camera);
        DrawText(TextFormat("%.1f", x), screenPos.x + 5, screenPos.y - 20, fontSize, DARKGRAY);
    }

    // Подписи для горизонтальных линий (ось Y)
    for (float y = gridStep; y < maxY; y += gridStep) {
        Vector2 screenPos = GetWorldToScreen2D({ 0, y }, camera);
        DrawText(TextFormat("%.1f", y), screenPos.x + 5, screenPos.y - 10, fontSize, DARKGRAY);
    }
    for (float y = -gridStep; y > minY; y -= gridStep) {
        Vector2 screenPos = GetWorldToScreen2D({ 0, y }, camera);
        DrawText(TextFormat("%.1f", y), screenPos.x + 5, screenPos.y - 10, fontSize, DARKGRAY);
    }
}

int main()
{
    // Параметры окна
    const int screenWidth = 1280;
    const int screenHeight = 720;
    const char* windowTitle = "Graph Renderer";

    InitWindow(screenWidth, screenHeight, windowTitle);
    SetTargetFPS(60);

    // Инициализация 2D-камеры
    Camera2D camera = { 0 };
    camera.target = { 0, 0 };                                     // Камера смотрит на начало координат
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };  // Смещение камеры — центр экрана
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Переменные для GUI
    char inputText[64] = "sin(x)";
    bool inputMode = false;
    std::string func;

    Vector2 prevMousePos = GetMousePosition(); // Храним предыдущую позицию мыши

    while (!WindowShouldClose())
    {
        float cameraSpeed = 300.0f * GetFrameTime(); // Скорость перемещения камеры

        // Управление клавишами WASD
        if (IsKeyDown(KEY_D)) camera.target.x += cameraSpeed;
        if (IsKeyDown(KEY_A)) camera.target.x -= cameraSpeed;
        if (IsKeyDown(KEY_W)) camera.target.y -= cameraSpeed;
        if (IsKeyDown(KEY_S)) camera.target.y += cameraSpeed;

        // Зум камеры с динамическим шагом
        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            float zoomFactor = (wheel > 0) ? 1.1f : 0.9f; // Увеличение или уменьшение
            camera.zoom *= zoomFactor;

            // Ограничение минимума и максимума зума
            if (camera.zoom < 0.1f) camera.zoom = 0.1f;
            if (camera.zoom > 40.0f) camera.zoom = 40.0f;
        }

        // Перемещение графика с помощью мыши
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 mousePos = GetMousePosition(); // Текущая позиция мыши
            Vector2 delta = Vector2Subtract(prevMousePos, mousePos); // Разница в позициях
            delta = Vector2Scale(delta, 1.0f / camera.zoom); // Учитываем зум

            camera.target = Vector2Add(camera.target, delta); // Смещаем камеру
        }

        prevMousePos = GetMousePosition(); // Обновляем позицию мыши

        // Рассчитываем толщину осей (2 пикселя)
        float axisThickness = 2.0f / camera.zoom;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // камера
        BeginMode2D(camera);

        // границы видимой области в мировых координатах
        Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera);
        Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera);
        Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera);
        Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera);

        float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
        float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
        float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
        float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });

        // Отрисовка сетки
        DrawGrid(camera, screenWidth, screenHeight);

        // отрисовка осей
        DrawLineEx({ minX, 0 }, { maxX, 0 }, axisThickness, BLACK); // ось X
        DrawLineEx({ 0, minY }, { 0, maxY }, axisThickness, BLACK); // ось Y

        EndMode2D();

        // Подписи сетки
        DrawGridLabels(camera, screenWidth, screenHeight);

        // GUI
        if (GuiTextBox({ 10, 10, 250, 30 }, inputText, 64, inputMode))
        {
            inputMode = !inputMode;
        }
        if (GuiButton({ 270, 10, 140, 30 }, "Build graph"))
        {
            func = inputText;
        }
        if (GuiButton({ 420, 10, 140, 30 }, "Center Camera"))
        {
            camera.target = { 0, 0 };
            camera.zoom = 1.0f;
        }
        DrawText(TextFormat("Function: %s", func.c_str()), 10, 50, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
