#include <stdio.h>

#include "image.h"
#include "MainFrame.h"

namespace CG2 {

class demoApp: public wxApp {
public:
	virtual bool OnInit() override;

private:
	MainFrame *frame;
	Image img;
};

void initImage(Color* buffer, int imageW, int imageH) {
	const Color R(1, 0, 0);
	const Color G(0, 1, 0);
	const Color B(0, 0, 1);

#pragma omp parallel for
	for (int i = 0; i < imageH; i++) {
		for (int j =0; j < imageW; j++) {
			Color &c = buffer[i*imageW + j];
			c = mix(R, G, i / (float) imageW) + mix(R, B, j / (float) imageH);
		}
	}
}

bool demoApp::OnInit() {

	img.load("D:\\Dropbox\\Data\\Code-for-Art.jpg");

	if (!img.isValid()) {
		img.allocate(MainFrame::minImageSize.x, MainFrame::minImageSize.y);
		initImage(img.getData(), img.getWidth(), img.getHeight());
		assert(img.isValid());
	}

	frame = new MainFrame("CUDA Demo", img, wxSYSTEM_MENU | wxCLOSE_BOX | wxMINIMIZE_BOX | wxCAPTION);

	frame->Show();
	frame->Center();
	return true;
}

wxIMPLEMENT_APP(demoApp);

}
