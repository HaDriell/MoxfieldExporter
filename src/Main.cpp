#include <iostream>

#include "cpr/cpr.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

json FetchDeckFromMoxfield(const std::string& moxfieldDeckID)
{
    std::string url = "https://api2.moxfield.com/v2/decks/all/" + moxfieldDeckID;
    cpr::Response response = cpr::Get(cpr::Url(url));
    return json(response.text);
}

int main()
{
    json deckInfo = FetchDeckFromMoxfield("JmfIh2Z5MEK2DlK6WfdBww");
    std::cout << "Hello World" << deckInfo << std::endl;
}