#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <algorithm>  // ��� std::min � std::max
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

int main(void)
{
    // ��������� ����
    const int screenWidth = 1280;
    const int screenHeight = 720;
    const char* windowTitle = "Graph Renderer";

    InitWindow(screenWidth, screenHeight, windowTitle);
    SetTargetFPS(60);

    // ������������� 2D-������
    Camera2D camera = { 0 };
    camera.target = { 0, 0 };                                // ������ ������� �� ������ ���������
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f }; // �������� ������ � ����� ������
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // ���������� ��� GUI
    char inputText[64] = "sin(x)";
    bool inputMode = false;
    std::string func;

    while (!WindowShouldClose())
    {
        // ���������� ������: ����������� � ������� ������ WASD
        float cameraSpeed = 300.0f * GetFrameTime(); // �������� ����������� ������ � ��������/���
        if (IsKeyDown(KEY_D)) camera.target.x += cameraSpeed;
        if (IsKeyDown(KEY_A)) camera.target.x -= cameraSpeed;
        if (IsKeyDown(KEY_W)) camera.target.y -= cameraSpeed;
        if (IsKeyDown(KEY_S)) camera.target.y += cameraSpeed;

        // ���������� ��� ������ � ������� �������� ����
        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            camera.zoom += wheel * 0.1f; // ����������� ����������� ����
            if (camera.zoom < 0.1f) camera.zoom = 0.1f; // ����������� �������� ����
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // ��������� ���� � ������ ���������� ������
        BeginMode2D(camera);
        // ���������� ������� ������� ������� � ������� �����������
        Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera);
        Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera);
        Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera);
        Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera);

        float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
        float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
        float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
        float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });

        // ������ "�����������" ��� ����� ������ ���������:
        DrawLineEx({ minX, 0 }, { maxX, 0 }, 2.0f, BLACK);  // ��� X
        DrawLineEx({ 0, minY }, { 0, maxY }, 2.0f, BLACK);    // ��� Y
        EndMode2D();

        // ��������� GUI (�������� �� ������� �� ������)
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
