#pragma once

#include <glew.h>
#include <wx/wx.h>

namespace CG2 {

class Image;
class BasicGLPane;
class kernelSlider;
class nbhdSlider;

class MainFrame: public wxFrame {
public:
	static const wxSize minImageSize;

	MainFrame(const wxString& title, const Image &image, const int style);
	~MainFrame();

	wxBoxSizer * mainSizer;
private:
	void OnMenuSelect(wxCommandEvent& event);
	void fileOperations();
	void displayCudaInfo();
	void updateImage(const Image &image);

	
	BasicGLPane *glPane;
	kernelSlider *kSlider;
	nbhdSlider   *neighbourSlider;

};

}
