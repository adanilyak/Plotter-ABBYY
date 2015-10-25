#include <vector>
#include <Windowsx.h>

#include "graphWindow.h"
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

GraphWindow::GraphWindow(int width, int height, MathCore &mathCore) :
	windowWidth(width),
	windowHeight(height),
	graphInPoints(mathCore, 80)
{
}

// Удалять чужой код нельзя, поэтому будем его комментировать
//GraphWindow::~GraphWindow()
//{
//}

const wchar_t* GraphWindow::nameClassWindow = L"ClassGraphWindow";
const wchar_t* GraphWindow::nameWindow = L"GraphWindow";

bool GraphWindow::RegisterClass(HINSTANCE hInstance) {
	WNDCLASSEX tag;
	tag.cbSize = sizeof(WNDCLASSEX);
	tag.style = CS_HREDRAW | CS_VREDRAW;
	tag.lpfnWndProc = windowProc;
	tag.cbClsExtra = 0;
	tag.cbWndExtra = 0;
	tag.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
	tag.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	tag.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
	tag.lpszMenuName = NULL;
	tag.lpszClassName = nameClassWindow;
	tag.hInstance = hInstance;
	tag.hIconSm = NULL;

	if (!::RegisterClassEx(&tag)) {
		::MessageBox(NULL, L"Can't register class", L"ERROR!", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

bool GraphWindow::Create(HINSTANCE hInstance, int nCmdShow) {
	cmdShow = nCmdShow;

	handle = ::CreateWindowEx( NULL, nameClassWindow, NULL, 
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_BORDER,
		200, 20, windowWidth, windowHeight,
		NULL, NULL, hInstance, this);

	return handle;
}

void GraphWindow::Show() {
	::ShowWindow(handle, cmdShow);
	::UpdateWindow(handle);
}

HWND GraphWindow::GetHandle() {
	return handle;
}

void GraphWindow::OnDestroy() {
	::PostQuitMessage(0);
}

void GraphWindow::OnClose() {
	int msgboxID = ::MessageBox(handle, (LPCWSTR)L"Do you surely want to exit?", (LPCWSTR)L"Exit",
		MB_YESNO | MB_ICONWARNING);

	switch (msgboxID) {
	case IDNO:
		return;
	}

	::DestroyWindow(handle);
}

void GraphWindow::OnKeyDown(WPARAM wParam) {
	switch (wParam) {
	case VK_UP:
		graphInPoints.turnRight();
		::InvalidateRect(handle, NULL, FALSE);
		::UpdateWindow(handle);
		break;
	case VK_DOWN:
		graphInPoints.turnLeft();
		::InvalidateRect(handle, NULL, FALSE);
		::UpdateWindow(handle);
		break;
	case VK_LEFT:
		graphInPoints.turnUp();
		::InvalidateRect(handle, NULL, FALSE);
		::UpdateWindow(handle);
		break;
	case VK_RIGHT:
		graphInPoints.turnDown();
		::InvalidateRect(handle, NULL, FALSE);
		::UpdateWindow(handle);
		break;
	// Z key
	case 0x5A:
		graphInPoints.changeScale(-2 );
		::InvalidateRect(handle, NULL, FALSE);
		::UpdateWindow(handle);
		break;
	// X key
	case 0x58:
		graphInPoints.changeScale( 2 );
		::InvalidateRect(handle, NULL, FALSE);
		::UpdateWindow(handle);
		break;
	// Q key
	case 0x51:
		graphInPoints.moveAlongX( 1 );
		::InvalidateRect(handle, NULL, FALSE);
		::UpdateWindow(handle);
		break;
	// A key
	case 0x41:
		graphInPoints.moveAlongX( -1 );
		::InvalidateRect(handle, NULL, FALSE);
		::UpdateWindow(handle);
		break;
	// W key
	case 0x57:
		graphInPoints.moveAlongY( 1 );
		::InvalidateRect(handle, NULL, FALSE);
		::UpdateWindow(handle);
		break;
	// S key
	case 0x53:
		graphInPoints.moveAlongY( -1 );
		::InvalidateRect(handle, NULL, FALSE);
		::UpdateWindow(handle);
		break;
	}
}

void GraphWindow::OnMouseWheel( WPARAM wParam )
{
	if( GET_WHEEL_DELTA_WPARAM( wParam ) > 0 ) {
		::SendMessage( handle, WM_KEYDOWN, 0x5A, 0 );
	} else {
		::SendMessage( handle, WM_KEYDOWN, 0x58, 0 );
	}
}

void GraphWindow::OnMouseMove( WPARAM wParam, int x, int y )
{
	if( wParam == MK_LBUTTON ) {
		if( x - prevMousePosX > 5 ) {
			::SendMessage( handle, WM_KEYDOWN, VK_RIGHT, 0 );
			prevMousePosX = x;
		} else if( x - prevMousePosX < -5 ) {
			::SendMessage( handle, WM_KEYDOWN, VK_LEFT, 0 );
			prevMousePosX = x;
		}
		if( y - prevMousePosY > 5 ) {
			::SendMessage( handle, WM_KEYDOWN, VK_UP, 0 );
			prevMousePosY = y;
		} else if( y - prevMousePosY < -5 ) {
			::SendMessage( handle, WM_KEYDOWN, VK_DOWN, 0 );
			prevMousePosY = y;
		}
	}
}

void GraphWindow::OnLButtonDown( int xMousePos, int yMousePos )
{
	prevMousePosX = xMousePos;
	prevMousePosY = yMousePos;
}

void GraphWindow::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = ::BeginPaint(handle, &ps);
	HDC newHdc = ::CreateCompatibleDC(hdc);
	RECT rect;
	::GetClientRect(handle, &rect);

	HBITMAP bitmap = ::CreateCompatibleBitmap( hdc, rect.right - rect.left, rect.bottom - rect.top );
	HGDIOBJ oldbitmap = ::SelectObject(newHdc, bitmap);

	drawGraph(newHdc);
	drawAxes(newHdc);

	::BitBlt( hdc, 0, 0, rect.right, rect.bottom, newHdc, 0, 0, SRCCOPY );

	::SelectObject(newHdc, oldbitmap);
	::DeleteObject(bitmap);

	::DeleteDC(hdc);
	::DeleteDC(newHdc);

	::ValidateRect(handle, NULL);
	::EndPaint(handle, &ps);
}

void GraphWindow::drawGraph(HDC dc) {
	::SetBkColor(dc, RGB(0, 0, 0));

	HPEN linePen = ::CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	::SelectObject(dc, linePen);

	std::vector< std::vector < std::pair<double, double> > > points = graphInPoints.getRelativePoints();

	for (size_t i = 0; i < points.size(); ++i) {
		int size = points[i].size() % 3 == 0 ? points[i].size() - 2 : 3 * (points[i].size() / 3) + 1;
		POINT* lppoints = new POINT[size];
		//::MoveToEx(dc, round(points[i][0].first), round(points[i][0].second), NULL);
		for( size_t j = 0; j < size; ++j ) {
		//	::LineTo(dc, round(points[i][j].first), round(points[i][j].second));
			lppoints[j] = { round( points[i][j].first ), round( points[i][j].second ) };
		}
		::PolyBezier( dc, lppoints, size );

		delete[] lppoints;
	}

	for (size_t j = 0; j < points[0].size(); ++j) {
		//::MoveToEx(dc, round(points[0][j].first), round(points[0][j].second), NULL);
		//for (size_t i = 1; i < points.size(); ++i) {
		//	::LineTo(dc, round(points[i][j].first), round(points[i][j].second));
		//}

		int size = points.size() % 3 == 0 ? points.size() - 2 : 3 * (points.size() / 3) + 1;
		POINT* lppoints = new POINT[size];
		for( size_t i = 0; i < size; ++i ) {
			lppoints[i] = { round( points[i][j].first ), round( points[i][j].second ) };
		}
		::PolyBezier( dc, lppoints, size );

		delete[] lppoints;
	}

	fillPolygons( dc, points );

	::DeleteObject(linePen);
}

void GraphWindow::drawAxes(HDC dc) {
	std::pair<double, double> xAxis = graphInPoints.getAxisVectorVisual(0);
	std::pair<double, double> yAxis = graphInPoints.getAxisVectorVisual(1);
	std::pair<double, double> zAxis = graphInPoints.getAxisVectorVisual(2);

	std::pair<double, double> origin = graphInPoints.getOriginCoordinates();

	HPEN linePen = ::CreatePen(PS_SOLID, 1, RGB(100, 100, 200));
	::SelectObject(dc, linePen);
	::MoveToEx(dc, round(origin.first), round(origin.second), NULL);
	::LineTo(dc, round(xAxis.first * 10000000000) / 1000, round(xAxis.second * 10000000000) / 1000);
	::DeleteObject(linePen);

	std::string text = "X";
	::SetTextColor(dc, RGB(100, 100, 200));
	::TextOut(dc, round(origin.first + xAxis.first * 200), round(origin.second + xAxis.second * 200),
		(LPCWSTR)std::wstring(text.begin(), text.end()).c_str(), text.length());

	linePen = ::CreatePen(PS_SOLID, 1, RGB(200, 100, 200));
	::SelectObject(dc, linePen);
	::MoveToEx(dc, round(origin.first), round(origin.second), NULL);
	::LineTo(dc, round(yAxis.first * 10000000000) / 1000, round(yAxis.second * 10000000000) / 1000);

	text = "Y";
	::SetTextColor(dc, RGB(200, 100, 200));
	::TextOut(dc, round(origin.first + yAxis.first * 200), round(origin.second + yAxis.second * 200),
		(LPCWSTR)std::wstring(text.begin(), text.end()).c_str(), text.length());

	linePen = ::CreatePen(PS_SOLID, 1, RGB(100, 200, 200));
	::SelectObject(dc, linePen);
	::MoveToEx(dc, round(origin.first), round(origin.second), NULL);
	::LineTo(dc, round(zAxis.first * 10000000000) / 1000, round(zAxis.second * 10000000000) / 1000);
	::DeleteObject(linePen);

	text = "Z";
	::SetTextColor(dc, RGB(100, 200, 200));
	::TextOut(dc, round(origin.first + zAxis.first * 200), round(origin.second + zAxis.second * 200),
		(LPCWSTR)std::wstring(text.begin(), text.end()).c_str(), text.length());
}

struct polyWrap {
	PointF poly[4];
};

void GraphWindow::fillPolygons(HDC dc, std::vector< std::vector < std::pair<double, double> > > &points) {
	Graphics graphics( dc );
	
	std::vector< std::vector< double > > zCoordinates = graphInPoints.getPoints();

	int indexXmax, indexYmax, indexXmin, indexYmin;
	double max, min;

	indexXmax = 0;
	indexXmin = 0;
	indexYmax = 0;
	indexYmin = 0;
	max = zCoordinates[0][0];
	min = zCoordinates[0][0];


	for (int i = 0; i < zCoordinates.size(); ++i) {
		for (int j = 0; j < zCoordinates[i].size(); ++j) {
			if (zCoordinates[i][j] <= min) {
				min = zCoordinates[i][j];
				indexXmin = i;
				indexYmin = j;
			}

			if (zCoordinates[i][j] >= max) {
				indexXmax = i;
				indexYmax = j;
			}
		}
	}

	LinearGradientBrush linGrBrush(
		Point((int)points[indexXmax][indexYmax].first, (int)points[indexXmax][indexYmax].second),
		Point((int)points[indexXmin][indexYmin].first, (int)points[indexXmin][indexYmin].second),
		Color(255, 255, 0, 0),   // opaque red
		Color(255, 0, 0, 255));  // opaque blue


	//graphics.FillRectangle( &linGrBrush, 0, 0, 200, 200 );

	//graphics.FillRectangle( &linGrBrush, 300, 0, 200, 200 );

	
	PointF* firstPointsArray;
	PointF* secondPointsArray;

	std::vector< polyWrap > cells;

	if (points.size() > 1) {
		for (size_t i = 0; i < points.size(); ++i) {
			int firstSize = points[i].size() % 3 == 0 ? points[i].size() - 2 : 3 * (points[i].size() / 3) + 1;
			firstPointsArray = new PointF[firstSize];
			for (size_t j = 0; j < firstSize; ++j) {
				firstPointsArray[j] = PointF( points[i][j].first, points[i][j].second );
			}

			int secondSize = 0;

			if (i != points.size() - 1) {
				secondSize = points[i+1].size() % 3 == 0 ? points[i+1].size() - 2 : 3 * (points[i+1].size() / 3) + 1;
				secondPointsArray = new PointF[secondSize];
				for (size_t j = 0; j < secondSize; ++j) {
					secondPointsArray[j] = PointF( points[i][j].first, points[i][j].second );
				}
			}

			int size = min( firstSize, secondSize );

			if (size > 0) {
				for (size_t t = 0; t < size - 1; ++t) {
					polyWrap wrap;
					wrap.poly[0] = firstPointsArray[t];
					wrap.poly[1] = firstPointsArray[t+1];
					wrap.poly[2] = secondPointsArray[t+1];
					wrap.poly[3] = secondPointsArray[t];
					
					cells.push_back( wrap );
				}
			}

			delete[] firstPointsArray;
			if (i != points.size() - 1) {
				delete[] secondPointsArray;
			}
		}
	}

	for (int i = 0; i < cells.size(); ++i) {
		graphics.FillPolygon( &linGrBrush, cells[i].poly, 4 );
	}

	graphics.FillPolygon( &linGrBrush, cells[150].poly, 4 );
}


LRESULT __stdcall GraphWindow::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_NCCREATE) {
		GraphWindow* that = reinterpret_cast< GraphWindow* >(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
		::SetWindowLong(handle, GWL_USERDATA, reinterpret_cast<LONG>(that));

		return ::DefWindowProc(handle, message, wParam, lParam);
	}

	GraphWindow* that = reinterpret_cast< GraphWindow* >(::GetWindowLong(handle, GWL_USERDATA));

	switch (message) {
		case WM_CLOSE:
			that->OnClose();
			return 0;

		case WM_DESTROY:
			that->OnDestroy();
			return 0;

		case WM_PAINT:
			that->OnPaint();
			return 0;

		case WM_KEYDOWN:
			that->OnKeyDown(wParam);
			return 0;

		case WM_MOUSEWHEEL:
			that->OnMouseWheel( wParam );
			return 0;

		case WM_LBUTTONDOWN:
			that->OnLButtonDown( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
			return 0;

		case WM_MOUSEMOVE:
			that->OnMouseMove( wParam, GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
			return 0;

		case WM_ERASEBKGND:
			return 1;
	}
		
	return ::DefWindowProc(handle, message, wParam, lParam);
}