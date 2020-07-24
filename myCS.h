#pragma once
#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "CScode.h"
#endif

#include "CScodeDoc.h"
#include "CScodeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void DDA(CPoint start, CPoint end, CClientDC& dc, COLORREF color = RGB(0, 0, 0));
void Bresenhamline(CPoint start, CPoint end, CClientDC& dc, COLORREF color = RGB(0, 0, 0));
void CirclePolygon(CPoint start, CPoint end, CClientDC& dc, COLORREF color = RGB(0, 0, 0));
void Rectangle(CPoint start, CPoint end, CClientDC& dc, COLORREF color = RGB(0, 0, 0));
void clipLine(CPoint winMin, CPoint winMax, CPoint p1, CPoint  p2, CClientDC& dc, COLORREF color = RGB(0, 0, 0));
void ScanlineSeed(CPoint p, CClientDC& dc, COLORREF Fill = RGB(0, 100, 100), COLORREF Boundary = RGB(0, 0, 0));