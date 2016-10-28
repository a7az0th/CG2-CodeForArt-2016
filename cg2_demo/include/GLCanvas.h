#pragma once

#include <wx/wx.h>
#include "wx/glcanvas.h"
#include "demoFilters.h"

namespace CG2 {

class MainFrame;
class Image;

class BasicGLPane : public wxGLCanvas {
public:
	BasicGLPane(MainFrame* frame, const Image &img, const int* args, float mixVal, int nbhd);
	virtual ~BasicGLPane();

	//Event handlers 
	void OnResize(wxSizeEvent& evt);
	void OnRender(wxPaintEvent& evt);

	void updateImage(const Image &img, const float mixVal, const int nbhd);

public:
	deviceType device;
	uint32*      buffer;
	int width, height;
	float mixVal;
	int kernelNeighbourhood;
	int cudaInitialized;
	Image image;
	MainFrame *frame;
	wxGLContext* glContext;
	DeviceInfo cudaDevInfo;
};

}
