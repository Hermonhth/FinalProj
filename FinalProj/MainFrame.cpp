#include "MainFrame.h"
#include <wx/wx.h>
#include <vector>
#include <string>
#include "NutritionApp.h"
#include <curl/curl.h>
#include "json.hpp"
#include <wx/mstream.h>

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title)
{
	CreateControls();
	BindEventHandlers();
	AddSavedRequests();
}
size_t MainFrame::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}



void MainFrame::CreateControls()
{
	wxFont headlineFont(wxFontInfo(wxSize(0, 26)).Bold());


	panel = new wxPanel(this);


	headlineText = new wxStaticText(panel, wxID_ANY, "Requests", wxPoint(0, 11), wxSize(800, -1), wxALIGN_CENTER_HORIZONTAL);
	headlineText->SetFont(headlineFont);

	input = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(100, 80), wxSize(495, 35), wxTE_PROCESS_ENTER);
	searchButton = new wxButton(panel, wxID_ANY, "Search", wxPoint(600, 80), wxSize(100, 35));
	ListBox = new wxListBox(panel, wxID_ANY, wxPoint(600, 500), wxSize(260, 200));
	clearButton = new wxButton(panel, wxID_ANY, "Clear", wxPoint(880, 590), wxSize(75, 35));
	label = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(100, 200), wxSize(450, 500), wxTE_MULTILINE | wxTE_READONLY);
    pic = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(wxSize(350, 250)), wxPoint(600, 200));

}

void MainFrame::BindEventHandlers()
{
	searchButton->Bind(wxEVT_BUTTON, &MainFrame::OnSearchButtonClicked, this);
	input->Bind(wxEVT_TEXT_ENTER, &MainFrame::OnInputEnter, this);
	clearButton->Bind(wxEVT_BUTTON, &MainFrame::OnClearButton, this);
	this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnWindowClosed, this);
}

void MainFrame::AddSavedRequests()
{
	std::vector<App> requests = loadRequestsFromFile("requests.txt");
	for (const App& request : requests) {
		int index = ListBox->GetCount();
		ListBox->Insert(request.description, index);
	}
}

void MainFrame::OnSearchButtonClicked(wxCommandEvent& evt)
{
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    std::string apiKey = "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";  // Securely manage your API key
    std::string foodName = input->GetValue().ToStdString();

    nlohmann::json jsonPayload = {
        {"model", "gpt-4o-mini"},
        {"messages", {
            {{"role", "user"}, {"content", "Provide the nutritional information and breifly potential benefical and harmful effects of " + foodName}}
        }}
    };

    std::string apiUrl = "https://api.openai.com/v1/chat/completions";
    std::string jsonData = jsonPayload.dump();

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            // Print the raw API response for debugging purposes
            std::cout << "Raw API Response:\n" << readBuffer << std::endl;

            // Enhanced error handling for unexpected JSON structure
            try {
                auto jsonResponse = nlohmann::json::parse(readBuffer);

                // Check if "choices" and "message" exist in the response
                if (jsonResponse.contains("choices") && !jsonResponse["choices"].empty() &&
                    jsonResponse["choices"][0].contains("message") &&
                    jsonResponse["choices"][0]["message"].contains("content")) {

                    std::string nutritionInfo = jsonResponse["choices"][0]["message"]["content"];
                    label->SetValue(nutritionInfo);
                }
                else {
                    // Handle unexpected response structure
                    label->SetValue("Unexpected API response structure: " + readBuffer);
                }
            }
            catch (const nlohmann::json::exception& e) {
                label->SetValue("Error parsing JSON: " + std::string(e.what()));
            }
        }
        else {
            std::string errorMessage = "Failed to fetch data from ChatGPT API: ";
            errorMessage += curl_easy_strerror(res);
            label->SetValue(errorMessage);
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    else {
        label->SetValue("Failed to initialize cURL.");
    }
}

void MainFrame::GenerateImage(const std::string& foodName)
{
    CURL* curl;
    CURLcode res;
    std::string imageResponse;
    std::string apiKey = "sk-proj-USKa-pXVVF4wB5XdHY4AEer7VT6e80ewXz6UE3s9KZ0h_RKdyAPTC_EOVaqDn0VkpjE2Z78kheT3BlbkFJsE1fgf8jeg-l2fCCoI79m-7jO6dA1wgmByl0D6qQQ_R3YyiLwpBElhBZoxFXjCXI5HRRXITuYAY";  // Replace with your OpenAI API key

    // Construct the JSON payload for image generation
    nlohmann::json jsonImagePayload = {
        {"prompt", "Generate a realistic image of " + foodName},
        {"n", 1},
        {"size", "1024x1024"}
    };

    std::string apiUrl = "https://api.openai.com/v1/images/generations";
    std::string jsonData = jsonImagePayload.dump();

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &imageResponse);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res == CURLE_OK) {
            try {
                // Parse JSON response to get image URL
                auto jsonResponse = nlohmann::json::parse(imageResponse);
                std::string imageUrl = jsonResponse["data"][0]["url"];

                // Call function to download and display the image
                DisplayImageFromUrl(imageUrl);
            }
            catch (const nlohmann::json::exception& e) {
                label->SetValue("Error parsing image JSON: " + std::string(e.what()));
            }
        }
        else {
            label->SetValue("Failed to generate image.");
        }
        }
    };

void MainFrame::DisplayImageFromUrl(const std::string& imageUrl) {
    CURL* curl;
    CURLcode res;
    std::string imageData;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, imageUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &imageData);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            // Convert imageData string into a wxMemoryInputStream for wxImage
            wxMemoryInputStream memStream(imageData.c_str(), imageData.size());
            wxImage image(memStream, wxBITMAP_TYPE_PNG);  // or PNG depending on format

            if (image.IsOk()) {
                // Set the image to display in wxStaticBitmap (assuming icon is the wxStaticBitmap)
                pic->SetBitmap(wxBitmap(image));
                panel->Layout();
            }
            else {
                label->SetValue("Failed to load image data.");
            }
        }
        else {
            label->SetValue("Failed to download image.");
        }
    }
}



void MainFrame::OnInputEnter(wxCommandEvent& evt)
{
	AddRequestFromInput();
}

void MainFrame::OnClearButton(wxCommandEvent& evt)
{
	if (ListBox->IsEmpty()) {
		return;
	}
	else {
		ListBox->Clear();
	}
}

void MainFrame::OnWindowClosed(wxCloseEvent& evt)
{
	std::vector<App> requests;

	for (int i = 0; i < ListBox->GetCount(); i++) {
		App request;
		request.description = ListBox->GetString(i);
		requests.push_back(request);
	}

	saveRequestsToFile(requests, "requests.txt");
	evt.Skip();
}







void MainFrame::AddRequestFromInput()
{
	wxString description = input->GetValue();

	if (!description.IsEmpty()) {
		ListBox->Insert(description, ListBox->GetCount());
		input->Clear();
	}
	input->SetFocus();
}