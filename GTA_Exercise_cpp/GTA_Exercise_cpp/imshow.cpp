#include <iostream>
#include <Windows.h>
#include <opencv2/opencv.hpp>



int main() {
    HWND gameWindowHandle = FindWindow(NULL, L"Grand Theft Auto V");

    if (gameWindowHandle == NULL) {
        std::cerr << "Error: Game window not found." << std::endl;
        return 1;
    }

    while (true) {
        HDC hdcSrc = GetDC(gameWindowHandle);

        if (hdcSrc == NULL) {
            std::cerr << "Error: Failed to get device context." << std::endl;
            return 1;
        }

        RECT windowRect;
        GetClientRect(gameWindowHandle, &windowRect);
        int width = windowRect.right - windowRect.left;
        int height = windowRect.bottom - windowRect.top;

        HDC hdcDest = CreateCompatibleDC(hdcSrc);
        HBITMAP hBitmap = CreateCompatibleBitmap(hdcSrc, width, height);
        SelectObject(hdcDest, hBitmap);

        BitBlt(hdcDest, 0, 0, width, height, hdcSrc, 0, 0, SRCCOPY);

        cv::Mat capturedImage(height, width, CV_8UC4);
        GetBitmapBits(hBitmap, capturedImage.step * capturedImage.rows, LPVOID(capturedImage.data));

        DeleteObject(hBitmap);
        DeleteDC(hdcDest);
        ReleaseDC(gameWindowHandle, hdcSrc);
        
        cv::imshow("Captured Game Window", capturedImage);

        if (cv::waitKey(100) == 27) { // ESC 키를 누르면 종료
            break;
        }
    }

    return 0;
}
