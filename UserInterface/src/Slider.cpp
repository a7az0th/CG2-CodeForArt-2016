#include "Slider.h"
#include "GLCanvas.h"

namespace CG2 {

kernelSlider::kernelSlider(wxWindow * parent,
	                       wxWindowID id,
	                       int defaultValue,
	                       int minValue,
	                       int maxValue,
	                       const wxPoint & pos,
	                       const wxSize & size,
	                       long style,
	                       BasicGLPane* basicGLPane)
	: wxSlider(parent,id,defaultValue, minValue, maxValue, pos, size, style)
{
	glPane = basicGLPane;
	Connect(id , wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(kernelSlider::OnScroll));
}

kernelSlider::~kernelSlider() {
	glPane = NULL;
}

void kernelSlider::OnScroll(wxScrollEvent& event)
{
	glPane->mixVal = -1.f + GetValue() / 500.f;
	glPane->Refresh();
}

nbhdSlider::nbhdSlider(wxWindow * parent,
	                       wxWindowID id,
	                       int defaultValue,
	                       int minValue,
	                       int maxValue,
	                       const wxPoint & pos,
	                       const wxSize & size,
	                       long style,
	                       BasicGLPane* basicGLPane)
	: wxSlider(parent,id,defaultValue, minValue, maxValue, pos, size, style)
{
	glPane = basicGLPane;
	Connect(id , wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(nbhdSlider::OnScroll));
}

nbhdSlider::~nbhdSlider() {
	glPane = NULL;
}

void nbhdSlider::OnScroll(wxScrollEvent& event) {
	glPane->kernelNeighbourhood = GetValue();
	glPane->Refresh();
}

}
