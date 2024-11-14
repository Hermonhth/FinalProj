#pragma once
#include <wx/wx.h>

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);
private:
	void CreateControls();
	void BindEventHandlers();
	void AddSavedRequests();

	void OnSearchButtonClicked(wxCommandEvent& evt);
	void OnInputEnter(wxCommandEvent& evt);
	void OnClearButton(wxCommandEvent& evt);
	void OnWindowClosed(wxCloseEvent& evt);
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
	void GenerateImage(const std::string& foodName);
	void DisplayImageFromUrl(const std::string& imageUrl);


	void AddRequestFromInput();

	wxPanel* panel;
	wxStaticText* headlineText;
	wxTextCtrl* input;
	wxTextCtrl* label;
	wxButton* searchButton;
	wxListBox* ListBox;
	wxButton* clearButton;
	wxBoxSizer* sizer;
	wxStaticBitmap* pic;


};