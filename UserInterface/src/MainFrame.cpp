#include "MainFrame.h"
#include "wx/glcanvas.h"
#include "GLCanvas.h"
#include "Slider.h"
#include "ComboBox.h"


#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/stdpaths.h>

namespace CG2 {

static const int ID_Info           = wxID_HIGHEST + 1;
static const int ID_Open           = wxID_HIGHEST + 2;
static const int ID_ComboBox       = wxID_HIGHEST + 3;
static const int ID_KernelSlider   = wxID_HIGHEST + 4;
static const int ID_NbhdSlider     = wxID_HIGHEST + 5;

const wxSize CG2::MainFrame::minImageSize = wxSize(640,480);

MainFrame::MainFrame(const wxString& title, const Image &img, int style = wxDEFAULT_FRAME_STYLE)
	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, style)
{
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_Info, "&CUDA Device Info\tCtrl-H", "Display some basic info about the CUDA device in use.");
	menuFile->Append(ID_Open, "&Open image file\tCtrl-O", "Open and load an image into the viewport");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, "&File" );

	SetMenuBar( menuBar );

	mainSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer * botSizer    = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer * sliderSizer = new wxBoxSizer(wxVERTICAL); 

	const int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0};
	glPane = new BasicGLPane(this, img, args, 0.0f, 1);

	kSlider = new kernelSlider(this,
	                           ID_KernelSlider,
	                           0, 0, 1000,
	                           wxDefaultPosition,
	                           wxDefaultSize,
	                           wxSL_HORIZONTAL,
	                           glPane);
	kSlider->SetValue(500);
	kSlider->SetToolTip("Control blur or sharpness strength");

	neighbourSlider = new nbhdSlider(this,
	                                 ID_NbhdSlider,
	                                 5, 0, 50,
	                                 wxDefaultPosition,
	                                 wxDefaultSize,
	                                 wxSL_HORIZONTAL,
	                                 glPane);
	neighbourSlider->SetValue(1);
	neighbourSlider->SetToolTip("Control filter size");

	wxString str[2] = {"CPU", "CUDA"};
	ComboBox* combo = new ComboBox(this, ID_ComboBox,
	                               wxDefaultPosition,
	                               wxDefaultSize,
	                               2, str, wxCB_READONLY,
	                               glPane);
	combo->SetSelection(0);

	sliderSizer->Add(kSlider        , 0, wxALL | wxEXPAND, 5);
	sliderSizer->Add(neighbourSlider, 0, wxALL | wxEXPAND, 5);

	botSizer->Add(combo      , 0, wxCENTER , 5);
	botSizer->Add(sliderSizer, 1, wxCENTER | wxEXPAND , 5);

	mainSizer->Add(glPane,   0, wxEXPAND | wxALL, 5);
	mainSizer->Add(botSizer, 0, wxEXPAND, 5);

	Connect(ID_Info  , wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnMenuSelect));
	Connect(ID_Open  , wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnMenuSelect));
	Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnMenuSelect));

	CreateStatusBar();

	mainSizer->FitInside(this);
	SetSizerAndFit(mainSizer);
}

void MainFrame::OnMenuSelect(wxCommandEvent& event) {
	switch (event.GetId()) {
	case ID_Info:
		displayCudaInfo();
		break;
	case ID_Open:
		fileOperations();
		break;
	case wxID_EXIT:
		Close( true );
		break;
	}
}

void MainFrame::displayCudaInfo() {
	wxString message;
	const DeviceInfo & dev = glPane->cudaDevInfo;
	if (glPane->cudaInitialized && dev.devId >= 0) {

		const float totalMemory = float(dev.memory)/float(1024*1024*1024);
		const float sharedMemory = float(dev.sharedMemPerBlock)/float(1024);

		char buff[1024];
		message += "Current device: " + dev.name + "\n\n";
		sprintf(buff, "Compute Capability : %d.%d\n", dev.major, dev.minor);          message += buff;
		sprintf(buff, "Total global memory : %.1f GB\n", totalMemory);                message += buff;
		sprintf(buff, "Shared memory : %.1f KB\n", sharedMemory);                     message += buff;
		sprintf(buff, "Max threads per block : %d\n", dev.maxThreadsPerBlock);        message += buff;
		sprintf(buff, "Warp size : %d\n", dev.warpSize);                              message += buff;
		sprintf(buff, "Number of multiprocessors : %d\n", dev.multiProcessorCount);   message += buff;
		sprintf(buff, "Max threads per multiprocessor : %d\n", dev.maxThreadsPerMP);  message += buff;
	} else {
		message = wxString("Oops...\n No CUDA context has been initialized...\n");
	}

	wxLogMessage(message);

}

const wxString ioFileSelector = wxT("png or jpeg images (*.png;*.jpeg;*.jpg;*.bmp)|*.png;*.jpeg;*.jpg;*.bmp");

void MainFrame::fileOperations() {
	const wxStandardPaths& stdPaths = wxStandardPaths::Get();
	wxString dir = stdPaths.GetUserDir(wxStandardPaths::Dir_Pictures);
	wxFileDialog fdlg(this, wxT("Open input image file"), dir, wxT(""), ioFileSelector, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (fdlg.ShowModal() != wxID_CANCEL) {
		wxFileInputStream inputStream(fdlg.GetPath());
		if (inputStream.Ok()) {
			const Image img(std::string(fdlg.GetPath()));
			if (img.isValid()) {
				updateImage(img);
				SetTitle(img.getName());
			} else {
				SetStatusText(wxString(wxT("Could not load image file ")) + fdlg.GetPath() + wxT(". Please try another file."));
			}
		} else {
			SetStatusText(wxString(wxT("Could not open file: ")) + fdlg.GetPath());
		}
	}
}

void MainFrame::updateImage(const Image& img) {
	kSlider->SetValue(500);
	neighbourSlider->SetValue(1);

	glPane->updateImage(img, 0.0f, 1);

	mainSizer->FitInside(this);
	SetSizerAndFit(mainSizer);

	Refresh(false);
}

MainFrame::~MainFrame() {
}

}
