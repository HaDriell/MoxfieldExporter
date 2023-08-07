#pragma once
#include <fstream>
#include <filesystem>

class DeckWriter
{
public:
    DeckWriter(const std::filesystem::path& path);
    ~DeckWriter();

    void BeginDeck(const std::string& deckName, const std::string& description = "");
    void EndDeck();

    void BeginZone(const std::string& name);
    void EndZone();

    void Card(int quantity, const std::string& name);

private:
    enum class Scope
    {
        Global,         // Outside any scope of XML
        Deck,           // Inside a deck. Can Begin/End zones
        Zone,           // Inside a Zone. Can Add cards
    };

    void CheckScope(Scope scope);

private:
    std::ofstream m_Stream;
    Scope m_Scope;
};
