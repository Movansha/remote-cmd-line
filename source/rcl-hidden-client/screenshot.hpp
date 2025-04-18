#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <ShlObj.h>
#include <filesystem>
#include <fstream>

//----------

using std::wstring;

//----------

static void save_bitmap(HBITMAP hBitmap, const wstring& filename) {
    BITMAP bmp;
    GetObject(hBitmap, sizeof(bmp), &bmp);

    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bi;


    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = -bmp.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = bmp.bmHeight * bmp.bmWidthBytes;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    std::ofstream file(filename, std::ios::binary);

    bfh.bfType = 0x4D42;
    bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmp.bmHeight * bmp.bmWidthBytes;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    file.write(reinterpret_cast<const char*> (&bfh), sizeof(bfh));
    file.write(reinterpret_cast<const char*> (&bi), sizeof(bi));


    HDC hDC = CreateCompatibleDC(NULL);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hDC, hBitmap);


    int width = bmp.bmWidth;
    int height = bmp.bmHeight;
    int widthBytes = bmp.bmWidthBytes;

    char* pBits = new char[widthBytes * height];
    GetDIBits(hDC, hBitmap, 0, height, pBits, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    file.write(pBits, widthBytes * height);
    delete[] pBits;

    SelectObject(hDC, hOldBmp);
    DeleteDC(hDC);
    file.close();
}


//----------

wstring get_temp_ss_loc() {
    PWSTR path = NULL;
    SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path);

    wstring temp_folder = wstring(path) + L"\\Temp";
    if (!std::filesystem::exists(path)) std::filesystem::create_directories(path);

    return temp_folder + L"\\ss";
}

void take_screenshot() {
    HDC hScreenDC = GetDC(nullptr);

    int width = GetDeviceCaps(hScreenDC, HORZRES);
    int height = GetDeviceCaps(hScreenDC, VERTRES);

    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);


    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
    HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hMemoryDC, hBitmap));

    BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);
    hBitmap = static_cast<HBITMAP>(SelectObject(hMemoryDC, hOldBitmap));

    save_bitmap(hBitmap, get_temp_ss_loc());

    DeleteDC(hMemoryDC);
    DeleteDC(hScreenDC);

    DeleteObject(hBitmap);
}
