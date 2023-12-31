#include "Main.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <sstream>

#include "cpr/cpr.h"

#include "DeckWriter.h"


int main(int argc, char** argv)
{
    std::string username;
    std::string deckname;
    std::filesystem::path workspace = ".";

    {
        //Poor man's CommandLine Processing
        const std::vector<std::string_view> args(argv, argv + argc);
        for(int index = 1; index < args.size(); index++)
        {
            std::string_view previous = args[index - 1];
            std::string_view current = args[index];

            if (current == "-o") continue;
            if (previous == "-o")
            {
                workspace = current;
                continue;
            }

            if (current == "-d") continue;
            if (previous == "-d")
            {
                deckname = current;
                continue;
            }

            if (username.empty())
            {
                username = current;
                continue;
            }

            previous = current;
        }
    }

    std::cout << "username  : " << username << std::endl;
    std::cout << "deckanme  : " << deckname << std::endl;
    std::cout << "workspace : " << workspace << std::endl;

    if (username.empty())
    {
        std::cerr << "Usage: MoxfieldExporter <username> [-o <path>] [-d <deckName>]" << std::endl;
        return 0;
    }

    std::map<std::string, std::string> decks;
    if (!FetchUserDecklistFromMoxfield(username, decks))
    {
        std::cerr << "Is '" << username << "' a valid Moxfield useranme ?" << std::endl;
        return 0;
    }

    std::cout << "Found " << decks.size() << " Decks on Moxfield for " << username << "\n";

    std::filesystem::create_directories(workspace);
    if (!deckname.empty())
    {
        //Fetch a specific deck
        auto it = decks.find(deckname);
        if (it == decks.end())
        {
            std::cerr << "Deck not found !" << std::endl;
            return 0;
        }

        FetchDeckFromMoxfield(it->second, workspace);
    }
    else
    {
        //Fetch all decks
        for (const auto& [_, deckID] : decks)
        {
            FetchDeckFromMoxfield(deckID, workspace);
        }
    }


}

bool FetchUserDecklistFromMoxfield(const std::string& username, std::map<std::string, std::string>& decks)
{
    uint32_t pageNumber = 1;
    uint32_t totalPages = 1;
    for (; pageNumber <= totalPages; pageNumber++)
    {
        std::string url = "https://api2.moxfield.com/v2/users/" + username + "/decks?pageNumber=" + std::to_string(pageNumber);
        cpr::Response response = cpr::Get(cpr::Url(url));

        if (response.status_code != 200)
        {
            std::cerr << "Failed to fetch Deck list from Moxfield : " << response.reason << std::endl;
            return false;
        }

        json data = json::parse(response.text);
        totalPages = data["totalPages"]; // update page limit using retrieved pagination

        const json& deckList = data["data"];
        for (const json& deckInfo : deckList)
        {
            std::string deckName = deckInfo["name"];
            std::string deckId = deckInfo["publicId"];
            decks[deckName] = deckId;
        }
    }

    return true;
}

bool FetchDeckFromMoxfield(const std::string& moxfieldDeckID, const std::filesystem::path& folder)
{
    std::string url = "https://api2.moxfield.com/v2/decks/all/" + moxfieldDeckID;
    cpr::Response response = cpr::Get(cpr::Url(url));

    if (response.status_code != 200)
    {
        std::cerr << "Failed to fetch Deck JSON from Moxfield : " << response.reason << std::endl;
        return false;
    }

    json deckData = json::parse(response.text);
    if (!deckData.is_object())
    {
        return false;
    }

    std::string name = deckData["name"];
    std::string description = deckData["description"];
    std::string filename = EscapeFilename(name) + ".cod";
    std::filesystem::path filepath = folder / filename;

    std::cout << "Writing " << filepath << std::endl;

    DeckWriter writer = DeckWriter(filepath);

    writer.BeginDeck(name, description);

    writer.BeginZone("main");
    for (const auto& [_, cardData] : deckData["mainboard"].items())
    {
        int quantity = cardData["quantity"];
        writer.Card(quantity, GetCardNameForCockatrice(cardData));
    }
    writer.EndZone();

    writer.BeginZone("side");
    for (const auto& [_, cardData] : deckData["sideboard"].items())
    {
        int quantity = cardData["quantity"];
        writer.Card(quantity, GetCardNameForCockatrice(cardData));
    }
    writer.EndZone();

    writer.EndDeck();
    return true;
}

std::string GetCardNameForCockatrice(const json& cardData)
{
    const json& card = cardData["card"];
    std::string layout = card["layout"];
    std::string name = card["name"];

    if (layout == "modal_dfc" 
    ||  layout == "transform")
    {
        const json& card_faces = card["card_faces"];
        const json& front_face = card_faces[0];
        name = front_face["name"];
    }

    return name;
}

std::string EscapeFilename(std::string_view filename)
{
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string_view::value_type c : filename)
    {
        //Escape Forbidden printable characters on Windows & Linux
        if (c == '<' || c == '>' || c == ':' || c == '"' || c == '/' || c == '\\' || c == '|' || c == '?' || c == '*')
        {
            escaped << std::uppercase;
            escaped << '%' << std::setw(2) << int((unsigned char) c);
            escaped << std::nouppercase;
            continue;
        }

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == ' ') {
            escaped << c;
            continue;
        }

        // ignore any other character. at this point I'm too afraid to ask if it's complete
    }

    return escaped.str();
}