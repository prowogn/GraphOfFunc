#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <algorithm>
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include "../vcpkg_installed/x64-windows/include/raymath.h"

template <typename T>
T Clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float CalculateGridStep(float zoom) {
    float baseStep = 50.0f;
    float step = baseStep / zoom;
    float scale = powf(10.0f, floorf(log10f(step)));
    step = roundf(step / scale) * scale;
    return step;
}

void DrawGrid(Camera2D camera, int screenWidth, int screenHeight) {
    Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera);
    Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera);
    Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera);
    Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera);
    float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
    float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
    float gridThickness = (2.0f / camera.zoom) * 0.5f;
    float gridStep = CalculateGridStep(camera.zoom);

    for (float x = gridStep; x < maxX; x += gridStep) {
        DrawLineEx({ x, minY }, { x, maxY }, gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    }
    for (float x = -gridStep; x > minX; x -= gridStep) {
        DrawLineEx({ x, minY }, { x, maxY }, gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    }
    for (float y = gridStep; y < maxY; y += gridStep) {
        DrawLineEx({ minX, y }, { maxX, y }, gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    }
    for (float y = -gridStep; y > minY; y -= gridStep) {
        DrawLineEx({ minX, y }, { maxX, y }, gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    }
}

void DrawGridLabels(Camera2D camera, int screenWidth, int screenHeight) {
    Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera);
    Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera);
    Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera);
    Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera);
    float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
    float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
    float gridStep = CalculateGridStep(camera.zoom);
    int fontSize = (int)(18 / camera.zoom);
    fontSize = Clamp(fontSize, 12, 36);

    for (float x = gridStep; x < maxX; x += gridStep) {
        Vector2 screenPos = GetWorldToScreen2D({ x, 0 }, camera);
        DrawText(TextFormat("%.1f", x), screenPos.x + 5, screenPos.y - 20, fontSize, DARKGRAY);
    }
    for (float x = -gridStep; x > minX; x -= gridStep) {
        Vector2 screenPos = GetWorldToScreen2D({ x, 0 }, camera);
        DrawText(TextFormat("%.1f", x), screenPos.x + 5, screenPos.y - 20, fontSize, DARKGRAY);
    }
    for (float y = gridStep; y < maxY; y += gridStep) {
        Vector2 screenPos = GetWorldToScreen2D({ 0, y }, camera);
        DrawText(TextFormat("%.1f", y), screenPos.x + 5, screenPos.y - 10, fontSize, DARKGRAY);
    }
    for (float y = -gridStep; y > minY; y -= gridStep) {
        Vector2 screenPos = GetWorldToScreen2D({ 0, y }, camera);
        DrawText(TextFormat("%.1f", y), screenPos.x + 5, screenPos.y - 10, fontSize, DARKGRAY);
    }
}

void UpdateCamera(Camera2D& camera, bool& centering, Vector2& desiredTarget, Vector2& prevMousePos, float deltaTime)
{
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D) || IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        centering = false;

    float cameraSpeed = 300.0f * deltaTime;
    if (IsKeyDown(KEY_D)) camera.target.x += cameraSpeed;
    if (IsKeyDown(KEY_A)) camera.target.x -= cameraSpeed;
    if (IsKeyDown(KEY_W)) camera.target.y -= cameraSpeed;
    if (IsKeyDown(KEY_S)) camera.target.y += cameraSpeed;

    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        float zoomFactor = (wheel > 0) ? 1.1f : 0.9f;
        camera.zoom *= zoomFactor;
        camera.zoom = Clamp(camera.zoom, 0.1f, 40.0f);
    }

    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Vector2 delta = Vector2Subtract(prevMousePos, mousePos);
        delta = Vector2Scale(delta, 1.0f / camera.zoom);
        camera.target = Vector2Add(camera.target, delta);
    }
    prevMousePos = mousePos;

    if (centering) {
        float smoothing = 5.0f * deltaTime;
        camera.target = Vector2Lerp(camera.target, desiredTarget, smoothing);
        if (Vector2Length(Vector2Subtract(camera.target, desiredTarget)) < 0.1f)
        {
            camera.target = desiredTarget;
            centering = false;
        }
    }
}

void DrawScene(Camera2D& camera, int screenWidth, int screenHeight)
{
    BeginMode2D(camera);
    Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera);
    Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera);
    Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera);
    Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera);
    float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
    float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });

    DrawGrid(camera, screenWidth, screenHeight);
    float axisThickness = 2.0f / camera.zoom;
    DrawLineEx({ minX, 0 }, { maxX, 0 }, axisThickness, BLACK);
    DrawLineEx({ 0, minY }, { 0, maxY }, axisThickness, BLACK);
    EndMode2D();

    DrawGridLabels(camera, screenWidth, screenHeight);
}

void DrawUI(std::string& func, char* inputText, bool& inputMode, bool& centering, Vector2& desiredTarget)
{
    if (GuiTextBox({ 10, 10, 250, 30 }, inputText, 64, inputMode))
        inputMode = !inputMode;
    if (GuiButton({ 270, 10, 140, 30 }, "Build graph"))
        func = inputText;
    if (GuiButton({ 420, 10, 140, 30 }, "Center Camera"))
    {
        centering = true;
        desiredTarget = { 0, 0 };
    }
    DrawText(TextFormat("Function: %s", func.c_str()), 10, 50, 20, BLACK);
}

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Graph Renderer");
    SetTargetFPS(60);

    Camera2D camera = { 0 };
    camera.target = { 0, 0 };
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    bool centering = false;
    Vector2 desiredTarget = { 0, 0 };
    Vector2 prevMousePos = GetMousePosition();

    char inputText[64] = "sin(x)";
    bool inputMode = false;
    std::string func;

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        UpdateCamera(camera, centering, desiredTarget, prevMousePos, deltaTime);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawScene(camera, screenWidth, screenHeight);
        DrawUI(func, inputText, inputMode, centering, desiredTarget);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

