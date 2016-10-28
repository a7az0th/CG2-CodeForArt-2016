#include "ComboBox.h"
#include "GLCanvas.h"

namespace CG2 {

ComboBox::ComboBox(wxWindow * parent,
	               wxWindowID id,
	               const wxPoint & pos,
	               const wxSize & size,
	               int n,
	               const wxString choices[],
	               long style,
	               BasicGLPane* basicGLPane)
	: wxComboBox(parent, id, "", pos, size, n, choices, style)
{
	glPane = basicGLPane;
	Connect(id , wxEVT_COMBOBOX, wxCommandEventHandler(ComboBox::OnSelectionChange));
}

ComboBox::~ComboBox() {
	glPane = NULL;
}

void ComboBox::OnSelectionChange(wxCommandEvent& event)
{
	glPane->device = static_cast<deviceType>(GetSelection());
	glPane->Refresh();
}

}
