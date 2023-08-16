#include <iostream>
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>



int main() {
    HWND gameWindowHandle = FindWindow(NULL, L"Grand Theft Auto V");

    if (gameWindowHandle == NULL) {
        std::cerr << "Error: Game window not found." << std::endl;
        return 1;
    }

    cv::dnn::Net net;
    net = cv::dnn::readNetFromCaffe("deploy.prototxt", "model.caffemodel");

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

        cv::Mat blob = cv::dnn::blobFromImage(capturedImage, 1.0, cv::Size(300, 300), cv::Scalar(104, 177, 123));
        net.setInput(blob);
        cv::Mat detections = net.forward();

        // 검출된 객체에 사각형 표시
        for (int i = 0; i < detections.size[2]; i++) {
            float confidence = detections.at<float>(0, 0, i);
            if (confidence > 0.5) { // 임계값 조정 가능
                int classId = static_cast<int>(detections.at<float>(0, 0, i));
                int xLeftBottom = static_cast<int>(capturedImage.cols * detections.at<float>(0, 0, i + 3));
                int yLeftBottom = static_cast<int>(capturedImage.rows * detections.at<float>(0, 0, i + 4));
                int xRightTop = static_cast<int>(capturedImage.cols * detections.at<float>(0, 0, i + 5));
                int yRightTop = static_cast<int>(capturedImage.rows * detections.at<float>(0, 0, i + 6));

                cv::Rect objectRect(xLeftBottom, yLeftBottom, xRightTop - xLeftBottom, yRightTop - yLeftBottom);
                cv::rectangle(capturedImage, objectRect, cv::Scalar(0, 255, 0), 2);
            }
        }


        cv::imshow("Captured Game Window", capturedImage);

        if (cv::waitKey(100) == 27) { // ESC 키를 누르면 종료
            break;
        }
    }

    return 0;
}
