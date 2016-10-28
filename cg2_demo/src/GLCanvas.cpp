#include "MainFrame.h"
#include "GLCanvas.h"
#include "Timer.h"

namespace CG2 {

Timer timer;
GLuint glTexture, glBuffer;
char statusText[1024];

void convertColorsToInts(const Image &bitmap, uint32* buf, int width, int height) {
#pragma omp parallel for
	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			uint32& c = buf[width*h + w];
			c = bitmap.getPixel(w, h).toUINT32();
		}
	}
}

BasicGLPane::BasicGLPane(MainFrame* frm, const Image &img, const int* args, float mixVal, int nbhd) :
    mixVal(mixVal), device(CPU), cudaInitialized(1), frame(frm), kernelNeighbourhood(nbhd),
    buffer(NULL), glContext(NULL), width(0), height(0), 
    wxGLCanvas(frm, wxID_ANY, args, wxPoint(0,0), wxDefaultSize, wxFULL_REPAINT_ON_RESIZE) {

	glContext = new wxGLContext(this);

	Connect(wxEVT_SIZE,  wxSizeEventHandler(BasicGLPane::OnResize), NULL, this);
	Connect(wxEVT_PAINT, wxPaintEventHandler(BasicGLPane::OnRender), NULL, this);

	//This should be called before binding textures
	SetCurrent(*glContext);
	SetBackgroundStyle(wxBG_STYLE_PAINT);

	glewInit();
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &glTexture);
	glBindTexture(GL_TEXTURE_2D, glTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenBuffers(1, &glBuffer);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, glBuffer);
	glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, 1, NULL, GL_STREAM_COPY);

	cudaDevInfo.devId = -1;
	int res = initCuda(glBuffer, cudaDevInfo);
	if (res) {
		cudaInitialized = 0;
		sprintf(statusText, "Unable to initialize CUDA context!\nError returned by cuda = %d\nFall back to CPU.", res);
		wxMessageBox(statusText);
	}
	updateImage(img, mixVal, kernelNeighbourhood);
}

BasicGLPane::~BasicGLPane() {
	if (cudaInitialized) deinitCuda();
	cudaInitialized = 0;
	delete glContext; glContext = NULL;
	delete [] buffer; buffer = NULL;
}

void BasicGLPane::OnResize(wxSizeEvent& evt) {
	Refresh();
}

void BasicGLPane::updateImage(const Image &newImage, const float mix, const int nbhd) {
	const uint64 requiredMem = newImage.getMemUsage();
	const uint64 availableMem = image.getMemUsage();

	image  = newImage;
	width  = newImage.getWidth();
	height = newImage.getHeight();
	kernelNeighbourhood = nbhd;
	this->mixVal = mix;

	const uint64 bufferSize = width * height * sizeof(uint32);

	if (requiredMem > availableMem) {
		delete [] buffer;
		buffer = new uint32[width * height];
	}
	convertColorsToInts(image, buffer, width, height);

	if (requiredMem > availableMem) {
		glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, bufferSize, NULL, GL_STREAM_COPY);
		//Since the GL buffer has been reallocated, update the Cuda-to-GL pointer.
		resizeCudaBuffer(bufferSize);
		remapCudaBuffer(glBuffer);
	} else {
		glBufferSubData(GL_PIXEL_UNPACK_BUFFER_ARB, 0, bufferSize, NULL);
	}
	uploadCudaBuffer(buffer, bufferSize);

	initFilters(kernelNeighbourhood);
	newImageLoaded(image);

	const int screenX = wxSystemSettings::GetMetric ( wxSYS_SCREEN_X );
	const int screenY = wxSystemSettings::GetMetric ( wxSYS_SCREEN_Y );

	const wxSize maxSize = wxSize(screenX, screenY) * 0.8f;

	int viewWidth = width;
	int viewHeight = height;
	//Calculate OpenGL viewport size so that it fits the wxApp window
	if (viewWidth > maxSize.x || viewHeight > maxSize.y) {
		const float aspect = viewHeight/ float(viewWidth);

		if (viewWidth > maxSize.x) {
			viewWidth = maxSize.x;
			viewHeight = viewWidth*aspect;
		}

		if (viewHeight > maxSize.y) {
			viewHeight = maxSize.y;
			viewWidth  = viewHeight / aspect;
		}
	}

	SetMinSize(wxSize(viewWidth, viewHeight));
	SetSize(viewWidth, viewHeight);

	glViewport(0, 0, viewWidth, viewHeight);
}

void BasicGLPane::OnRender( wxPaintEvent& evt ) {
	if(!IsShown()) {
		return;
	}
	/// If cuda is not initialized, fall back to CPU, no matter user input
	if (!cudaInitialized) {
		device = CPU;
	}
	wxPaintDC(this);

	timer.start();
	const int res = filterImage(device, buffer, image, mixVal, kernelNeighbourhood);
	if (cudaInitialized && res != 0) {
		cudaInitialized = 0;
		device = CPU;
		sprintf(statusText, "Error occurred while executing CUDA kernel!\nError returned by cuda = %d\nFall back to CPU.", res);
		wxMessageBox(statusText);
	}
	if (device == CPU) {
		// Update the OpenGL buffer data with the processed buffer
		// This is only done for CPU as CUDA has access to that buffer on the GPU
		// and works on it directly
		glBufferSubData(GL_PIXEL_UNPACK_BUFFER_ARB, 0, width*height*sizeof(uint32), buffer);
	}

	// ------------- draw some 2D ----------------
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0); {
		glBegin(GL_QUADS); {
			glTexCoord2f(1.0f, 0.0f); glVertex2f(+1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, +1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(+1.0f, +1.0f);
		}
		glEnd();
	}
	glFinish();

	timer.stop();
	const int filterSize = kernelNeighbourhood*2 + 1;
	sprintf(statusText, "Rendered in %.3fs. Filter size is %dx%d pixels.", 
		timer.elapsedMilliseconds(), filterSize, filterSize);
	frame->SetStatusText(statusText);

	this->SwapBuffers();
}

}
