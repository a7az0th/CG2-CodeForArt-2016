#pragma once

#include "wx/wx.h"

namespace CG2 {

class BasicGLPane;

class kernelSlider : public wxSlider {
public:
	kernelSlider(wxWindow * parent,
	             wxWindowID id,
	             int defaultValue,
	             int minValue,
	             int maxValue,
	             const wxPoint & pos,
	             const wxSize & size,
	             long style,
	             BasicGLPane* glPane);
	~kernelSlider();

private:
	void OnScroll(wxScrollEvent& event);
	BasicGLPane* glPane;
};

class nbhdSlider : public wxSlider
{
public:
	nbhdSlider(wxWindow * parent,
	           wxWindowID id,
	           int defaultValue,
	           int minValue,
	           int maxValue,
	           const wxPoint & pos,
	           const wxSize & size,
	           long style,
	           BasicGLPane* glPane);
	~nbhdSlider();

private:
	void OnScroll(wxScrollEvent& event);
	BasicGLPane* glPane;
};

}
