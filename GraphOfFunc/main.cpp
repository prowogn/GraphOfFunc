#define _CRT_SECURE_NO_WARNINGS
#include <iostream>               // ��
#include <algorithm>              // min, max
#include <raylib.h>               // ����������
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>               // ���������


// ���������� ���������� (� ������ ����)
float targetAxisThickness = 2.0f;
float currentAxisThickness = 2.0f;

template <typename T>
T Clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float Lerp(float start, float end, float t) {
    t = Clamp(t, 0.0f, 1.0f); // ������������ t ���������� [0, 1]
    return start + t * (end - start);
}

float CalculateGridStep(float zoom) {
    // ������� ��� ����� (��� zoom = 1.0)
    float baseStep = 50.0f;

    // ����������� �������� ��� � ����������� �� ����
    float step = baseStep / zoom;

    // ��������� ��� �� ���������� "���������" �������� (10, 20, 50, 100 � �.�.)
    float scale = powf(10.0f, floorf(log10f(step)));
    step = roundf(step / scale) * scale;

    return step;
}

void DrawGrid(Camera2D camera, int screenWidth, int screenHeight) {
    // �������� ������� ������� ������� � ������� �����������
    Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera);
    Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera);
    Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera);
    Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera);

    float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
    float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });

    // ������������ ������� ����� �����
    float gridThickness = Lerp(0.8f, 0.3f, camera.zoom / 5.0f); // ������ ���������� ������������
    gridThickness = Clamp(gridThickness, 0.2f, 1.5f);

    // ��������� ��� �����
    float gridStep = CalculateGridStep(camera.zoom);

    // ������ ������������ ����� (������������ ��� Y)
    for (float x = gridStep; x < maxX; x += gridStep) {
        DrawLineEx({ x, minY }, { x, maxY }, gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    }
    for (float x = -gridStep; x > minX; x -= gridStep) {
        DrawLineEx({ x, minY }, { x, maxY }, gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    }

    // ������ �������������� ����� (������������ ��� X)
    for (float y = gridStep; y < maxY; y += gridStep) {
        DrawLineEx({ minX, y }, { maxX, y }, gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    }
    for (float y = -gridStep; y > minY; y -= gridStep) {
        DrawLineEx({ minX, y }, { maxX, y }, gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    }
}

void DrawGridLabels(Camera2D camera, int screenWidth, int screenHeight) {
    // �������� ������� ������� �������
    Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera);
    Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera);
    Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera);
    Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera);

    float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
    float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });

    // ��� �����
    float gridStep = CalculateGridStep(camera.zoom);

    // ������������ ������ ������
    int fontSize = (int)(12 / camera.zoom);
    fontSize = Clamp(fontSize, 8, 24);

    // ������� ��� ������������ ����� (��� X)
    for (float x = gridStep; x < maxX; x += gridStep) {
        Vector2 screenPos = GetWorldToScreen2D({ x, 0 }, camera);
        DrawText(TextFormat("%.1f", x), screenPos.x + 5, screenPos.y - 20, fontSize, DARKGRAY);
    }
    for (float x = -gridStep; x > minX; x -= gridStep) {
        Vector2 screenPos = GetWorldToScreen2D({ x, 0 }, camera);
        DrawText(TextFormat("%.1f", x), screenPos.x + 5, screenPos.y - 20, fontSize, DARKGRAY);
    }

    // ������� ��� �������������� ����� (��� Y)
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
    // ��������� ����
    const int screenWidth = 1280;
    const int screenHeight = 720;
    const char* windowTitle = "Graph Renderer";

    InitWindow(screenWidth, screenHeight, windowTitle);
    SetTargetFPS(60);

    // ������������� 2D-������
    Camera2D camera = { 0 };
    camera.target = { 0, 0 };                                     // ������ ������� �� ������ ���������
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };  // �������� ������ � ����� ������
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // ���������� ��� GUI
    char inputText[64] = "sin(x)";
    bool inputMode = false;
    std::string func;


    while (!WindowShouldClose())
    {
        float cameraSpeed = 300.0f * GetFrameTime(); // �������� ����������� ������ � ��������/���

        // ����������� � ������� ������ WASD
        if (IsKeyDown(KEY_D)) camera.target.x += cameraSpeed;
        if (IsKeyDown(KEY_A)) camera.target.x -= cameraSpeed;
        if (IsKeyDown(KEY_W)) camera.target.y -= cameraSpeed;
        if (IsKeyDown(KEY_S)) camera.target.y += cameraSpeed;

        // ��� ������
        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            camera.zoom += wheel * 0.1f; // ����������� ����
            if (camera.zoom < 0.1f) camera.zoom = 0.1f; // ����������� �������� ����
        }

        currentAxisThickness = Lerp(currentAxisThickness, targetAxisThickness / camera.zoom, 0.1f);
        currentAxisThickness = Clamp(currentAxisThickness, 0.5f, 4.0f);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // ������
        BeginMode2D(camera);

        // ������� ������� ������� � ������� �����������
        Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera);
        Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera);
        Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera);
        Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera);

        float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
        float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
        float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
        float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });

        // ��������� �����
        DrawGrid(camera, screenWidth, screenHeight);

        float axisThickness = 2.0f / camera.zoom; // ������� ������� 2px � ����������
        axisThickness = Clamp(axisThickness, 0.5f, 4.0f); // ������������ ����������� � ������������ �������

        DrawLineEx({ minX, 0 }, { maxX, 0 }, currentAxisThickness, BLACK);    // ��� X
        DrawLineEx({ 0, minY }, { 0, maxY }, currentAxisThickness, BLACK);    // ��� Y

        EndMode2D();

        // ������� �����
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
