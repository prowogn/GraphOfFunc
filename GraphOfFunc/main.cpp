#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <algorithm>
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include "../vcpkg_installed/x64-windows/include/raymath.h"
#include "../vcpkg_installed/x64-windows/include/tinyexpr.h"
#include <cmath>
#include <string>
#include <vector>
#include <cctype>
#include <cstdio>  // Для sprintf

// Глобальные переменные для разбора функции
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

// Форматирование метки с динамическим числом знаков после запятой.
// Второй параметр – величина шага подписей (labelStep).
std::string FormatLabel(float value, float step) {
    int decimals = 0;
    if (step < 1.0f) {
        decimals = (int)ceil(-log10f(step));
        if (decimals < 0) decimals = 0;
    }
    char buffer[64];
    sprintf(buffer, "%.*f", decimals, value);
    std::string result(buffer);
    size_t pos = result.find('.');
    if (pos != std::string::npos) {
        while (!result.empty() && result.back() == '0')
            result.pop_back();
        if (!result.empty() && result.back() == '.')
            result.pop_back();
    }
    return result;
}

// Преобразование математических координат (инверсия Y)
Vector2 MathToRaylib(Vector2 mathCoord)
{
    return { mathCoord.x, -mathCoord.y };
}

void DrawGrid(Camera2D camera, int screenWidth, int screenHeight)
{
    // Получаем углы экрана в математических координатах (инвертируем Y)
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

    // Вертикальные линии
    for (float x = gridStep; x < maxX; x += gridStep)
        DrawLineEx(MathToRaylib({ x, minY }), MathToRaylib({ x, maxY }), gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    for (float x = -gridStep; x > minX; x -= gridStep)
        DrawLineEx(MathToRaylib({ x, minY }), MathToRaylib({ x, maxY }), gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));

    // Горизонтальные линии
    for (float y = gridStep; y < maxY; y += gridStep)
        DrawLineEx(MathToRaylib({ minX, y }), MathToRaylib({ maxX, y }), gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
    for (float y = -gridStep; y > minY; y -= gridStep)
        DrawLineEx(MathToRaylib({ minX, y }), MathToRaylib({ maxX, y }), gridThickness, ColorAlpha(LIGHTGRAY, 0.7f));
}

void DrawGridLabels(Camera2D camera, int screenWidth, int screenHeight)
{
    // Вычисляем видимые границы в математических координатах
    Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera); topLeft.y = -topLeft.y;
    Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera); topRight.y = -topRight.y;
    Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera); bottomLeft.y = -bottomLeft.y;
    Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera); bottomRight.y = -bottomRight.y;

    float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
    float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });

    float gridStep = CalculateGridStep(camera.zoom);
    // Подписи строятся через каждые 5 клеток
    float labelStep = gridStep * 5;
    int fontSize = 18;

    // Подписи по оси X (относительно 0)
    for (int i = -5; i <= 5; i++)
    {
        float x = i * labelStep;
        if (x < minX || x > maxX) continue;
        Vector2 screenPos = GetWorldToScreen2D(MathToRaylib({ x, 0 }), camera);
        std::string label = FormatLabel(x, labelStep);
        DrawText(label.c_str(), screenPos.x + 5, screenPos.y - 20, fontSize, DARKGRAY);
    }

    // Подписи по оси Y (относительно 0)
    for (int i = -5; i <= 5; i++)
    {
        float y = i * labelStep;
        // Если y == 0, пропускаем, чтобы не дублировать ноль (он уже отрисован по оси X)
        if (fabs(y) < 1e-6) continue;
        if (y < minY || y > maxY) continue;
        Vector2 screenPos = GetWorldToScreen2D(MathToRaylib({ 0, y }), camera);
        std::string label = FormatLabel(y, labelStep);
        DrawText(label.c_str(), screenPos.x + 5, screenPos.y - 10, fontSize, DARKGRAY);
    }
}

// Функция для отрисовки графика функции с определением разрывов
void DrawGraph(Camera2D camera, int screenWidth, int screenHeight, te_expr* expr)
{
    if (!expr) return; // Если выражение не скомпилировано, ничего не делаем

    // Определяем границы видимой области в математических координатах
    Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera);
    topLeft.y = -topLeft.y;
    Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera);
    topRight.y = -topRight.y;
    Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera);
    bottomLeft.y = -bottomLeft.y;
    Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera);
    bottomRight.y = -bottomRight.y;
    float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });

    // Шаг выборки по оси X
    float sampleStep = (maxX - minX) / (float)screenWidth;
    if (sampleStep <= 0) return;

    Vector2 prevPoint;
    bool validPrev = false;
    const float discontinuityThreshold = 100.0f; // порог в пикселях для разрыва
    // Итерируем по x от minX до maxX
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
            // Переводим точки в экранные координаты для оценки расстояния
            Vector2 screenPrev = GetWorldToScreen2D(MathToRaylib(prevPoint), camera);
            Vector2 screenCurrent = GetWorldToScreen2D(MathToRaylib(currentPoint), camera);
            if (Vector2Distance(screenPrev, screenCurrent) < discontinuityThreshold)
            {
                DrawLineEx(MathToRaylib(prevPoint), MathToRaylib(currentPoint), 2.0f / camera.zoom, RED);
            }
            // Если расстояние слишком велико – считаем, что произошёл разрыв и не соединяем точки
        }
        prevPoint = currentPoint;
        validPrev = true;
    }
}

void UpdateCamera(Camera2D& camera, bool& centering, Vector2& desiredTarget, Vector2& prevMousePos, float deltaTime, Vector2& mathTarget)
{
    // Если нажаты клавиши или мышь – отменяем центрирование
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D) || IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        centering = false;

    float cameraSpeed = 300.0f * deltaTime;
    if (IsKeyDown(KEY_D)) mathTarget.x += cameraSpeed;
    if (IsKeyDown(KEY_A)) mathTarget.x -= cameraSpeed;
    if (IsKeyDown(KEY_W)) mathTarget.y += cameraSpeed; // В математической системе: вверх – увеличение Y
    if (IsKeyDown(KEY_S)) mathTarget.y -= cameraSpeed;

    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        float zoomFactor = (wheel > 0) ? 1.1f : 0.9f;
        camera.zoom *= zoomFactor;
        // Увеличиваем максимально допустимый зум до 5000
        camera.zoom = Clamp(camera.zoom, 0.1f, 5000.0f);
        camera.zoom = roundf(camera.zoom * 10.0f) / 10.0f; // Округляем зум до десятых
    }

    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Vector2 delta = Vector2Subtract(prevMousePos, mousePos);
        delta = Vector2Scale(delta, 1.0f / camera.zoom);
        delta.y = -delta.y; // Инвертируем дельту по Y для математических координат
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
    camera.target = { mathTarget.x, -mathTarget.y }; // Обновляем камеру (инверсия Y)
}

void DrawScene(Camera2D& camera, int screenWidth, int screenHeight)
{
    BeginMode2D(camera);
    Vector2 topLeft = GetScreenToWorld2D({ 0, 0 }, camera); topLeft.y = -topLeft.y;
    Vector2 topRight = GetScreenToWorld2D({ (float)screenWidth, 0 }, camera); topRight.y = -topRight.y;
    Vector2 bottomLeft = GetScreenToWorld2D({ 0, (float)screenHeight }, camera); bottomLeft.y = -bottomLeft.y;
    Vector2 bottomRight = GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera); bottomRight.y = -bottomRight.y;
    float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
    float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });

    DrawGrid(camera, screenWidth, screenHeight);
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
        recompileGraph = true;  // Запрос на перекомпиляцию выражения
    }
    if (GuiButton({ 420, 10, 140, 30 }, "Center Camera"))
    {
        centering = true;
        desiredTarget = { 0, 0 }; // desiredTarget – в математических координатах
    }
    DrawText(TextFormat("Function: %s", func.c_str()), 10, 50, 20, BLACK);
    if (!compiledExpr && !func.empty())
        DrawText("Error compiling expression", 10, 80, 20, RED);
}

// ===== ФУНКЦИИ ПРЕОБРАБОТКИ ВВОДИМОГО ВЫРАЖЕНИЯ =====

enum TokenType { NUMBER, IDENTIFIER, OPERATOR_TOKEN, PAREN };

struct Token {
    TokenType type;
    std::string text;
};

std::vector<Token> tokenize(const std::string& s) {
    std::vector<Token> tokens;
    size_t i = 0;
    while (i < s.size()) {
        if (std::isdigit(s[i]) || s[i] == '.') {
            size_t j = i;
            while (j < s.size() && (std::isdigit(s[j]) || s[j] == '.'))
                j++;
            tokens.push_back({ NUMBER, s.substr(i, j - i) });
            i = j;
        }
        else if (std::isalpha(s[i]) || s[i] == '_') {
            size_t j = i;
            while (j < s.size() && (std::isalnum(s[j]) || s[j] == '_'))
                j++;
            tokens.push_back({ IDENTIFIER, s.substr(i, j - i) });
            i = j;
        }
        else if (s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/' || s[i] == '^') {
            tokens.push_back({ OPERATOR_TOKEN, std::string(1, s[i]) });
            i++;
        }
        else if (s[i] == '(' || s[i] == ')') {
            tokens.push_back({ PAREN, std::string(1, s[i]) });
            i++;
        }
        else {
            i++;
        }
    }
    return tokens;
}

std::string rebuildExpression(const std::vector<Token>& tokens) {
    std::string out;
    auto isOperandToken = [](const Token& t) -> bool {
        return (t.type == NUMBER) || (t.type == IDENTIFIER) || (t.type == PAREN && t.text == ")");
        };
    auto isOperandStartToken = [](const Token& t) -> bool {
        return (t.type == NUMBER) || (t.type == IDENTIFIER) || (t.type == PAREN && t.text == "(");
        };

    for (size_t i = 0; i < tokens.size(); i++) {
        out += tokens[i].text;
        if (i < tokens.size() - 1) {
            if (isOperandToken(tokens[i]) && isOperandStartToken(tokens[i + 1])) {
                // Если предыдущий токен – имя функции, то не вставляем знак умножения.
                if (tokens[i].type == IDENTIFIER &&
                    (tokens[i].text == "sin" || tokens[i].text == "cos" ||
                        tokens[i].text == "tan" || tokens[i].text == "ctg" ||
                        tokens[i].text == "ln" || tokens[i].text == "log2" ||
                        tokens[i].text == "log10") &&
                    tokens[i + 1].type == PAREN && tokens[i + 1].text == "(")
                {
                    // ничего не делаем
                }
                else {
                    out += "*";
                }
            }
        }
    }
    return out;
}

std::string preprocessFunctionString(const std::string& input) {
    std::string noSpaces;
    for (char c : input) {
        if (!std::isspace(static_cast<unsigned char>(c)))
            noSpaces.push_back(c);
    }
    std::vector<Token> tokens = tokenize(noSpaces);
    return rebuildExpression(tokens);
}

// Функция для замены всех вхождений подстроки
std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

// Определяем функцию котангенса
double ctg(double x) {
    return cos(x) / sin(x);
}

// Функция для натурального логарифма (ln)
double ln_wrapper(double x) {
    return log(x);
}

// Функция для двоичного логарифма (log2)
double log2_wrapper(double x) {
    return log2(x);
}

// Функция для десятичного логарифма (log10)
double log10_wrapper(double x) {
    return log10(x);
}

int main()
{
    // Разрешение: 1920 x 1080
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "Graph Renderer");
    SetTargetFPS(60);

    Camera2D camera = { 0 };
    camera.target = { 0, 0 };
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Vector2 mathTarget = { 0, 0 };
    bool centering = false;
    Vector2 desiredTarget = { 0, 0 };
    Vector2 prevMousePos = GetMousePosition();

    char inputText[64] = "tan(x)";
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

        if (recompileGraph)
        {
            if (compiledExpr) te_free(compiledExpr);
            // Регистрируем переменную x и функции:
            // - "ctg" для котангенса
            // - "ln" для натурального логарифма
            // - "log2_" для двоичного логарифма (заменяем log2 на log2_)
            // - "log10_" для десятичного логарифма (заменяем log10 на log10_)
            te_variable vars[] = {
                {"x", &te_x},
                {"ctg", ctg},
                {"ln", ln_wrapper},
                {"log2_", log2_wrapper},
                {"log10_", log10_wrapper}
            };
            int err;
            std::string processedFunc = preprocessFunctionString(func);
            // Выполняем замену, чтобы tinyexpr получил корректное имя функции
            processedFunc = replaceAll(processedFunc, "log10(", "log10_(");
            processedFunc = replaceAll(processedFunc, "log2(", "log2_(");
            compiledExpr = te_compile(processedFunc.c_str(), vars, sizeof(vars) / sizeof(te_variable), &err);
            if (!compiledExpr)
                std::cout << "Error compiling expression at position: " << err << std::endl;
            compiledFuncStr = func;
            recompileGraph = false;
        }
    }

    if (compiledExpr) te_free(compiledExpr);
    CloseWindow();
    return 0;
}
