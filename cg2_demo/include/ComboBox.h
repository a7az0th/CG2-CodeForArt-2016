#pragma once

#include "wx/wx.h"

namespace CG2 {

class BasicGLPane;

class ComboBox : public wxComboBox {
public:
	ComboBox(wxWindow * parent,
	         wxWindowID id,
	         const wxPoint & pos,
	         const wxSize & size,
	         int n,
	         const wxString choices[],
	         long style,
	         BasicGLPane* glPane);
	~ComboBox();

private:
	void OnSelectionChange(wxCommandEvent& event);
	BasicGLPane* glPane;
};

}
