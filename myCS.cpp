#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "CScode.h"
#endif

#include "CScodeDoc.h"
#include "CScodeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define LEFT_EDGE 0x1
#define RIGHT_EDGE 0x2
#define BOTTOM_EDGE 0x4
#define TOP_EDGE 0x8
#define INSIDE(a) (!a)
#define REJECT(a,b) (a&b)
#define ACCEPT(a,b) (!(a|b))
#define FALSE 0
#define TRUE 1
#include<stack>
#include<math.h>
#define PI 3.14159265358979323846
using std::stack;


void DDA(CPoint start, CPoint end, CClientDC& dc, COLORREF color = RGB(0, 0, 0))
{
	int dx = end.x - start.x;
	int dy = end.y - start.y;
	int steps, k;
	float xIncrement, yIncrement, x, y;
	x = start.x; y = start.y;
	if (abs(dx) > abs(dy))steps = abs(dx);
	else steps = abs(dy);
	xIncrement = dx / float(steps);
	yIncrement = dy / float(steps);
	dc.SetPixel(round(x), round(y), color);
	for (k = 0; k < steps; k++)
	{
		x += xIncrement;
		y += yIncrement;
		dc.SetPixel(round(x), round(y), color);
	}
}

void Bresenhamline(CPoint start, CPoint end, CClientDC& dc, COLORREF color = RGB(0, 0, 0))
{
	double m, e;
	int i, dx, dy, x, y, steps, flagx, flagy;
	x = start.x;
	y = start.y;
	dx = end.x - start.x;
	dy = end.y - start.y;
	flagx = (dx > 0) ? 1 : -1;
	flagy = (dy > 0) ? 1 : -1;

	if (abs(dx) > abs(dy))
	{
		steps = abs(dx);
		m = dy / (double)dx;
		int flagm = (m > 0) ? 1 : -1;
		e = m - 0.5 * flagm;
		for (i = 0; i < steps; i++)
		{
			dc.SetPixel(x, y, color);
			if (e * flagm >= 0)
			{
				y = y + flagy;
				e = e - flagm;
			}
			x = x + flagx;
			e = e + m;
		}
	}
	else
	{
		steps = abs(dy);
		m = dx / (double)dy;//c++ 0/0=1
		int flagm = (m > 0) ? 1 : -1;
		e = m - 0.5 * flagm;
		for (i = 0; i < steps; i++)
		{
			dc.SetPixel(x, y, color);
			if (e * flagm >= 0)
			{
				x = x + flagx;
				e = e - flagm;
			}
			y = y + flagy;
			e = e + m;
		}
	}
}

CPoint rotate(CPoint c, CPoint p, double a)
{
	CPoint k;
	//平移
	p.x = p.x - c.x;
	p.y = p.y - c.y;
	//旋转
	k.x = float(p.x * cos(a) - p.y * sin(a));
	k.y = float(p.x * sin(a) + p.y * cos(a));
	//平移
	k.x = k.x + c.x;
	k.y = k.y + c.y;
	return k;
}

void CirclePolygon(CPoint start, CPoint end, CClientDC& dc, COLORREF color = RGB(0, 0, 0))
{
	float r, a;
	long n, i;
	r = sqrt(pow(end.x - start.x, 2) + pow(end.y - start.y, 2));
	//最大误差小于一个像元
	n = 2 * PI / (2 * acos((r - 1) / r)) + 1;
	a = 2 * PI / n;
	CPoint p1, p2, p;
	p1.x = start.x - r * sin(a / 2);
	p2.x = start.x + r * sin(a / 2);
	p1.y = start.y - r * cos(a / 2);
	p2.y = p1.y;
	p = p1;
	Bresenhamline(p1, p2, dc);
	for (i = 0; i < n; i++)
	{
		p1 = p2;
		p2 = rotate(start, p, (i + 1) * a);
		Bresenhamline(p1, p2, dc);
	}

}

//绘制矩形
void Rectangle(CPoint start, CPoint end, CClientDC& dc, COLORREF color = RGB(0, 0, 0))
{
	CPoint p1, p2, p3, p4;
	p1.x = start.x;
	p1.y = start.y;
	p2.x = start.x;
	p2.y = end.y;
	p3.x = end.x;
	p3.y = end.y;
	p4.x = end.x;
	p4.y = start.y;
	Bresenhamline(p1, p2, dc);
	Bresenhamline(p2, p3, dc);
	Bresenhamline(p3, p4, dc);
	Bresenhamline(p4, p1, dc);
}

unsigned char encode(CPoint pt, CPoint winMin, CPoint winMax)
{
	unsigned char code = 0x00;
	if (pt.x < winMin.x)
		code = code | LEFT_EDGE;
	if (pt.x > winMax.x)
		code = code | RIGHT_EDGE;
	if (pt.y < winMin.y)
		code = code | BOTTOM_EDGE;
	if (pt.y > winMax.y)
		code = code | TOP_EDGE;
	return code;
}
void swapPts(CPoint* p1, CPoint* p2)
{
	CPoint tmp;
	tmp = *p1;
	*p1 = *p2;
	*p2 = tmp;
}
void swapCodes(unsigned char* c1, unsigned char* c2)
{
	unsigned char tmp; tmp = *c1;
	*c1 = *c2;
	*c2 = tmp;
}

void clipLine(CPoint winMin, CPoint winMax, CPoint p1, CPoint  p2, CClientDC& dc, COLORREF color = RGB(0, 0, 0))
{
	unsigned char code1, code2;
	int done = FALSE, draw = FALSE;
	float m;
	while (!done)
	{
		code1 = encode(p1, winMin, winMax);
		code2 = encode(p2, winMin, winMax);
		if (ACCEPT(code1, code2))
		{
			done = TRUE;
			draw = TRUE;
		}
		else
		{
			if (REJECT(code1, code2))
			{
				done = TRUE;
			}
			else
			{
				if (INSIDE(code1))
				{
					swapPts(&p1, &p2);
					swapCodes(&code1, &code2);
				}
				if (p2.x != p1.x)
					m = (float)(p2.y - p1.y) / (p2.x - p1.x);
				if (code1 & LEFT_EDGE)
				{
					p1.y += (winMin.x - p1.x) * m;
					p1.x = winMin.x;
				}
				else
				{
					if (code1 & RIGHT_EDGE)
					{
						p1.y += (winMax.x - p1.x) * m;
						p1.x = winMax.x;
					}
					else
					{
						if (code1 & BOTTOM_EDGE)
						{
							if (p2.x != p1.x)
								p1.x += (winMin.y - p1.y) / m;
							p1.y = winMin.y;
						}
						else
						{
							if (code1 & TOP_EDGE)
							{
								if (p2.x != p1.x)
									p1.x += (winMax.y - p1.y) / m;
								p1.y = winMax.y;
							}
						}
					}
				}
			}
		}
	}
	if (draw)
		Bresenhamline(p1, p2, dc);
}

//扫描线种子填充
void ScanlineSeed(CPoint p, CClientDC& dc, COLORREF Fill = RGB(0, 0, 0), COLORREF Boundary = RGB(0, 0, 0))
{
	stack<CPoint> s;
	s.push(p);
	while (!s.empty())
	{
		//Get the seed pixel and set it to the new value
		CPoint p1=s.top();
		int x = p1.x, y = p1.y;
		s.pop();
		dc.SetPixel(x,y,Fill);
		//save the x coordinate of the seed pixel
		int Savex = x;
		// fill the scan to the right of the seed pixel
		x = x + 1;
		while (dc.GetPixel(x,y) != Boundary)
		{
			dc.SetPixel(x,y, Fill);
			x = x + 1;
		}
		//save the extreme right pixel
		int Xright = x - 1;
		//reset the x coordinate to the value for the seed pixel
		x = Savex;
		// fill the span to the left of the seed pixel
		x = x - 1;
		while (dc.GetPixel(x,y) != Boundary)
		{
			dc.SetPixel(x,y, Fill);
			x = x - 1;
		}
		// save the extreme left pixel
		int Xleft = x + 1;
		//reset the x coordinate to the value for seed pixel
		x = Savex;
		// check that the scan the below is neither a polygon boundary nor has been previously filled;
		//if not, seed the scan line
		//start at the left edge of the scan line subspan
		//dc.SetPixel(Xright,p1.y, RGB(255,0,0));
		x = Xleft;
		y = y - 1;
		
		while (x <= Xright)
		{//seed the scan line below
			int Pflag = 0;
			while (dc.GetPixel(x,y) != Boundary && dc.GetPixel(x,y) != Fill && x <= Xright)
			{
				if (Pflag == 0) { Pflag = 1; }
				x = x + 1;

			}
			//push the extreme right pixel onto the stack
			if (Pflag == 1)
			{
				if (x == Xright && dc.GetPixel(x,y) != Boundary && dc.GetPixel(x,y) != Fill)
				{
					CPoint p2(x - 1, y);
					s.push(p2);
				}
				else
				{
					CPoint p2(x - 1, y);
					s.push(p2);
				}
			}
			// continue checking in case the span is interrupted
			int Xenter = x;
			while ((dc.GetPixel(x,y) == Boundary || dc.GetPixel(x,y) == Fill)&& x < Xright)
			{
				x = x + 1;
			}
			// make sure that pixel coordinate is incremented
			if (x == Xenter)x = x + 1;
		}
		//check that the scan line above is not polygon boundary
		//nor has been previously completely filled
		// this algorithm is exactly the same as that checking the scan line below
		//except that y=y+1 is substituted for y=y-1
		y = y + 2;
		x = Xleft;
		while (x <= Xright)
		{//seed the scan line below
			int Pflag = 0;
			while (dc.GetPixel(x, y) != Boundary && dc.GetPixel(x, y) != Fill && x <= Xright)
			{
				if (Pflag == 0) { Pflag = 1; }
				x = x + 1;

			}
			//push the extreme right pixel onto the stack
			if (Pflag == 1)
			{
				if (x == Xright && dc.GetPixel(x, y) != Boundary && dc.GetPixel(x, y) != Fill)
				{
					CPoint p2(x - 1, y);
					s.push(p2);
				}
				else
				{
					CPoint p2(x - 1, y);
					s.push(p2);
				}
			}
			// continue checking in case the span is interrupted
			int Xenter = x;
			while ((dc.GetPixel(x, y) == Boundary || dc.GetPixel(x, y) == Fill) && x < Xright)
			{
				x = x + 1;
			}
			// make sure that pixel coordinate is incremented
			if (x == Xenter)x = x + 1;
		}

	}
}