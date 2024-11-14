#include "Appx.h"
#include "MainFrame.h"

wxIMPLEMENT_APP(Appx);

bool Appx::OnInit()
{
	MainFrame* mainFrame = new MainFrame("Nutrition!");
	mainFrame->SetClientSize(1000, 800);
	mainFrame->Center();
	mainFrame->Show();

	return true;
}

