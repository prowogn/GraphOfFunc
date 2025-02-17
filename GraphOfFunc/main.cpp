#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <algorithm>
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include "../vcpkg_installed/x64-windows/include/raymath.h"
#include "../vcpkg_installed/x64-windows/include/tinyexpr.h"
#include <cmath>

// глобальные переменные дл€ разбора функции
double te_x = 0.0;
te_expr* compiledExpr = nullptr;
std::string compiledFuncStr = "";
bool recompileGraph = false;

template <typename T>
T Clamp(T value, T min, T max) 
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float CalculateGridStep(float zoom) 
{
    float baseStep = 50.0f;
    float step = baseStep / zoom;
    float scale = powf(10.0f, floorf(log10f(step)));
    step = roundf(step / scale) * scale;
    return step;
}

// преобразование математических координат (инверси€ Y)
Vector2 MathToRaylib(Vector2 mathCoord) 
{
    return { mathCoord.x, -mathCoord.y };
}

void DrawGrid(Camera2D camera, int screenWidth, int screenHeight) 
{
    // получаем углы экрана в математических координатах (инвертируем Y)
    Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera); topLeft.y = -topLeft.y;
    Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera); topRight.y = -topRight.y;
    Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera); bottomLeft.y = -bottomLeft.y;
    Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera); bottomRight.y = -bottomRight.y;

    float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
    float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });

    float gridThickness = (2.0f / camera.zoom) * 0.5f;
    float gridStep = CalculateGridStep(camera.zoom);

    // вертикальные линии
    for (float x = gridStep; x < maxX; x += gridStep) 
    {
        DrawLineEx(MathToRaylib({ x, minY }), MathToRaylib({ x, maxY }), gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    }
    for (float x = -gridStep; x > minX; x -= gridStep) 
    {
        DrawLineEx(MathToRaylib({ x, minY }), MathToRaylib({ x, maxY }), gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    }
    // горизонтальные линии
    for (float y = gridStep; y < maxY; y += gridStep) 
    {
        DrawLineEx(MathToRaylib({ minX, y }), MathToRaylib({ maxX, y }), gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    }
    for (float y = -gridStep; y > minY; y -= gridStep) 
    {
        DrawLineEx(MathToRaylib({ minX, y }), MathToRaylib({ maxX, y }), gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    }
}

void DrawGridLabels(Camera2D camera, int screenWidth, int screenHeight) 
{
    Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera); topLeft.y = -topLeft.y;
    Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera); topRight.y = -topRight.y;
    Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera); bottomLeft.y = -bottomLeft.y;
    Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera); bottomRight.y = -bottomRight.y;

    float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
    float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });

    float gridStep = CalculateGridStep(camera.zoom);
    int fontSize = (int)(18 / camera.zoom);
    fontSize = Clamp(fontSize, 12, 36);

    // подписи (ось X)
    for (float x = gridStep; x < maxX; x += gridStep) 
    {
        Vector2 screenPos = GetWorldToScreen2D(MathToRaylib({ x, 0 }), camera);
        DrawText(TextFormat("%.1f", x), screenPos.x + 5, screenPos.y - 20, fontSize, DARKGRAY);
    }
    for (float x = -gridStep; x > minX; x -= gridStep) 
    {
        Vector2 screenPos = GetWorldToScreen2D(MathToRaylib({ x, 0 }), camera);
        DrawText(TextFormat("%.1f", x), screenPos.x + 5, screenPos.y - 20, fontSize, DARKGRAY);
    }
    // подписи (ось Y)
    for (float y = gridStep; y < maxY; y += gridStep) 
    {
        Vector2 screenPos = GetWorldToScreen2D(MathToRaylib({ 0, y }), camera);
        DrawText(TextFormat("%.1f", y), screenPos.x + 5, screenPos.y - 10, fontSize, DARKGRAY);
    }
    for (float y = -gridStep; y > minY; y -= gridStep) 
    {
        Vector2 screenPos = GetWorldToScreen2D(MathToRaylib({ 0, y }), camera);
        DrawText(TextFormat("%.1f", y), screenPos.x + 5, screenPos.y - 10, fontSize, DARKGRAY);
    }
}

// функци€ дл€ отрисовки графика функции
void DrawGraph(Camera2D camera, int screenWidth, int screenHeight, te_expr* expr) 
{
    if (!expr) return; // если выражение не скомпилировано, ничего не делаем

    // границы видимой области в математических координатах
    Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera); topLeft.y = -topLeft.y;
    Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera); topRight.y = -topRight.y;
    Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera); bottomLeft.y = -bottomLeft.y;
    Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera); bottomRight.y = -bottomRight.y;
    float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });

    // шаг выборки по оси X
    float sampleStep = (maxX - minX) / (float)screenWidth;
    if (sampleStep <= 0) return;

    Vector2 prevPoint;
    bool validPrev = false;
    // итерируем по x от minX до maxX
    for (double x = minX; x <= maxX; x += sampleStep) 
    {
        te_x = x;
        double y = te_eval(expr);
        if (!std::isfinite(y)) 
        {
            validPrev = false;
            continue;
        }
        Vector2 currentPoint = { (float)x, (float)y };
        if (validPrev) 
        {
            // рисуем отрезок между предыдущей и текущей точками (ѕ–»ћ»“»¬Ќќ, —“ќ»“ ƒќ–јЅќ“ј“№ ƒЋя ќ“–»—ќ¬ » ‘”Ќ ÷»… — –ј«–џ¬јћ»)
            DrawLineEx(MathToRaylib(prevPoint), MathToRaylib(currentPoint), 2.0f / camera.zoom, RED);
        }
        prevPoint = currentPoint;
        validPrev = true;
    }
}

void UpdateCamera(Camera2D& camera, bool& centering, Vector2& desiredTarget, Vector2& prevMousePos, float deltaTime, Vector2& mathTarget)
{
    // если нажаты клавиши или мышь Ц отмен€ем центрирование
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D) || IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        centering = false;

    float cameraSpeed = 300.0f * deltaTime;
    // обновл€ем математические координаты камеры
    if (IsKeyDown(KEY_D)) mathTarget.x += cameraSpeed;
    if (IsKeyDown(KEY_A)) mathTarget.x -= cameraSpeed;
    if (IsKeyDown(KEY_W)) mathTarget.y += cameraSpeed; // в математической системе: вверх Ц увеличение Y
    if (IsKeyDown(KEY_S)) mathTarget.y -= cameraSpeed;

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
        // инвертируем дельту по Y дл€ математических координат
        delta.y = -delta.y;
        mathTarget = Vector2Add(mathTarget, delta);
    }
    prevMousePos = mousePos;

    if (centering) 
    {
        float smoothing = 5.0f * deltaTime;
        mathTarget = Vector2Lerp(mathTarget, desiredTarget, smoothing);
        if (Vector2Length(Vector2Subtract(mathTarget, desiredTarget)) < 0.1f)
        {
            mathTarget = desiredTarget;
            centering = false;
        }
    }
    // обновл€ем камеру: записываем mathTarget, преобразу€ ось Y
    camera.target = { mathTarget.x, -mathTarget.y };
}

void DrawScene(Camera2D& camera, int screenWidth, int screenHeight)
{
    BeginMode2D(camera);
    // вычисл€ем границы видимой области в математических координатах
    Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera); topLeft.y = -topLeft.y;
    Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera); topRight.y = -topRight.y;
    Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera); bottomLeft.y = -bottomLeft.y;
    Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera); bottomRight.y = -bottomRight.y;
    float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
    float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });

    DrawGrid(camera, screenWidth, screenHeight);
    // рисуем график функции (если выражение скомпилировано)
    DrawGraph(camera, screenWidth, screenHeight, compiledExpr);

    float axisThickness = 2.0f / camera.zoom;
    DrawLineEx(MathToRaylib({ minX, 0 }), MathToRaylib({ maxX, 0 }), axisThickness, BLACK);
    DrawLineEx(MathToRaylib({ 0, minY }), MathToRaylib({ 0, maxY }), axisThickness, BLACK);
    EndMode2D();

    DrawGridLabels(camera, screenWidth, screenHeight);
}

void DrawUI(std::string& func, char* inputText, bool& inputMode, bool& centering, Vector2& desiredTarget)
{
    if (GuiTextBox({ 10, 10, 250, 30 }, inputText, 64, inputMode))
        inputMode = !inputMode;
    if (GuiButton({ 270, 10, 140, 30 }, "Build graph"))
    {
        func = inputText;
        recompileGraph = true;  // запрос на перекомпил€цию выражени€
    }
    if (GuiButton({ 420, 10, 140, 30 }, "Center Camera"))
    {
        centering = true;
        desiredTarget = { 0, 0 }; // desiredTarget Ц в математических координатах
    }
    DrawText(TextFormat("Function: %s", func.c_str()), 10, 50, 20, BLACK);

    // если функци€ уже была скомпилирована, но выражение отсутствует Ц выводим сообщение об ошибке
    if (!compiledExpr && !func.empty())
        DrawText("Error compiling expression", 10, 80, 20, RED);
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

    // математические координаты камеры
    Vector2 mathTarget = { 0, 0 };
    bool centering = false;
    Vector2 desiredTarget = { 0, 0 };
    Vector2 prevMousePos = GetMousePosition();

    char inputText[64] = "sin(x)";
    bool inputMode = false;
    std::string func;

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        UpdateCamera(camera, centering, desiredTarget, prevMousePos, deltaTime, mathTarget);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawScene(camera, screenWidth, screenHeight);
        DrawUI(func, inputText, inputMode, centering, desiredTarget);
        EndDrawing();

        // если требуетс€ перекомпил€ци€ выражени€, то выполн€ем еЄ
        if (recompileGraph)
        {
            if (compiledExpr) te_free(compiledExpr);
            te_variable vars[] = { {"x", &te_x} };
            int err;
            compiledExpr = te_compile(func.c_str(), vars, 1, &err);
            if (!compiledExpr) {
                std::cout << "Error compiling expression at position: " << err << std::endl;
            }
            compiledFuncStr = func;
            recompileGraph = false;
        }
    }

    if (compiledExpr) te_free(compiledExpr);
    CloseWindow();
    return 0;
}


