#include <windows.h>
#include <iostream>
#include <thread>  // for std::this_thread::sleep_for and std::thread
#include <chrono>  // for std::chrono::milliseconds
#include <cmath>
#include <atomic>  // for std::atomic

void print(const std::string& value) {
    std::cout << value << std::endl;
}

// Function to move the mouse cursor by x and y pixels;
void moveMouse(int x, int y) {
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    int newX = cursorPos.x + x;
    int newY = cursorPos.y + y;
    SetCursorPos(newX, newY);
}

void clickMouse()
{
    // Simulate left mouse button down
    INPUT inputDown = { 0 };
    inputDown.type = INPUT_MOUSE;
    inputDown.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &inputDown, sizeof(INPUT));



    // Simulate left mouse button up
    INPUT inputUp = { 0 };
    inputUp.type = INPUT_MOUSE;
    inputUp.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &inputUp, sizeof(INPUT));
}

COLORREF GetPixelColor(int x, int y)
{
    // Get the device context of the screen
    HDC hdcScreen = GetDC(NULL);
    if (!hdcScreen)
    {
        std::cerr << "Failed to get device context" << std::endl;
        return RGB(0, 0, 0);  // Return black if failed
    }

    // Get the color of the pixel at the specified coordinates
    COLORREF color = GetPixel(hdcScreen, x, y);

    // Release the device context
    ReleaseDC(NULL, hdcScreen);

    return color;
}

bool IsPixelColorInRange(int centerX, int centerY, COLORREF targetColor, int tolerance)
{
    // Define the radius of the scan area
    int radius = 2;

    // Iterate over a square area around (centerX, centerY)
    for (int x = centerX - radius; x <= centerX + radius; ++x)
    {
        for (int y = centerY - radius; y <= centerY + radius; ++y)
        {
            // Check if the pixel at (x, y) matches the target color within tolerance
            COLORREF color = GetPixelColor(x, y);
            int red = GetRValue(color);
            int green = GetGValue(color);
            int blue = GetBValue(color);

            // Apply your custom color range criteria
            if (red >= 200 && red <= 255 && green >= 10 && green <= 80 && blue >= 10 && blue <= 80)
            {
                return true;
            }
        }
    }

    return false;
}


void enable_ansi_escape_codes() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        return;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) {
        return;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

void inputListener(std::atomic<bool>& running) {
    while (true) {
        char input = std::cin.get();
        if (input == '1') {
            std::cout << "Color Bot Activated\n";
            running.store(true);
        }
        else if (input == '0') {
            std::cout << "Color Bot Disabled\n";
            running.store(false);
        }
    }
}

int main()
{
    enable_ansi_escape_codes();  // Enable ANSI escape codes on Windows

    // Print the ASCII art with color gradients
    std::cout << "\033[0;35m" << ".____     ____ ___  _____  .___ _______    _________ ________  .____    ________ __________" << std::endl;
    std::cout << "\033[0;35m" << "|    |   |    |   \\/     \\ |   |\\      \\   \\_   ___ \\_____  \\ |    |   \\_____  \\\\______   \\" << std::endl;
    std::cout << "\033[0;35m" << "|    |   |    |   /  \\ /  \\|   |/   |   \\  /    \\  \\/ /   |   \\|    |    /   |   \\|       _/" << std::endl;
    std::cout << "\033[1;36m" << "|    |___|    |  /    Y    \\   /    |    \\ \\     \\___/    |    \\    |___/    |    \\    |   \\" << std::endl;
    std::cout << "\033[1;36m" << "|_______ \\______\\/____|__  /___\\____|__  /  \\______  |_______  /_______ \\_______  /____|_  /" << std::endl;
    std::cout << "\033[1;36m" << "        \\/               \\/            \\/          \\/        \\/        \\/       \\/       \\/ " << std::endl;
    std::cout << " " << std::endl;
    std::cout << "\033[1;35m" << "{0} - Disable " << std::endl;
    std::cout << "\033[1;35m" << "{1} - Enable " << std::endl;
    std::cout << "\033[0m"; // Reset color to default

    // Get screen resolution
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calculate the center of the screen
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;

    // Define the target color (e.g., pure red)
    COLORREF targetColor = RGB(255, 40, 40);

    // Define the tolerance for color matching (adjust as needed)
    int tolerance = 100; // You can adjust this tolerance value

    // Variable to control the loop
    std::atomic<bool> running(false);

    // Start the input listener thread
    std::thread listenerThread(inputListener, std::ref(running));
    listenerThread.detach();

    while (true) {
        if (running.load()) {
            moveMouse(0, 0);
            // Check if any pixel in the radius around the center matches the target color within tolerance
            if (IsPixelColorInRange(centerX, centerY, targetColor, tolerance))
            {
                clickMouse();
            }
            // No delay between scans
        }
    }

    return 0;
}
