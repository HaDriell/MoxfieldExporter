#include "DeckWriter.h"
#include <cassert>

DeckWriter::DeckWriter(const std::filesystem::path& path)
    : m_Stream(path, std::ofstream::out)
    , m_Scope(Scope::Global)
{
    assert(m_Stream.is_open());
}

DeckWriter::~DeckWriter()
{
    CheckScope(Scope::Global);
}

void DeckWriter::BeginDeck(const std::string& deckName, const std::string& description)
{
    CheckScope(Scope::Global);
    m_Stream << R"Xml(<?xml version="1.0" encoding="UTF-8"?>)Xml" << std::endl;
    m_Stream << R"Xml(<cockatrice_deck version="1">)Xml" << std::endl;
    m_Stream << R"Xml(    <deckname>)Xml" << deckName << R"Xml(</deckname>)Xml" << std::endl;
    m_Stream << R"Xml(    <comments>)Xml" << description << R"Xml(</comments>)Xml" << std::endl;
    m_Scope = Scope::Deck;
}

void DeckWriter::EndDeck()
{
    CheckScope(Scope::Deck);
    m_Stream << R"Xml(</cockatrice_deck>)Xml";
    m_Scope = Scope::Global;
}

void DeckWriter::BeginZone(const std::string& name)
{
    CheckScope(Scope::Deck);
    m_Stream << R"Xml(    <zone name=")Xml" << name << R"Xml(">)Xml" << std::endl;
    m_Scope = Scope::Zone;
}

void DeckWriter::EndZone()
{
    CheckScope(Scope::Zone);
    m_Stream << R"Xml(    </zone>)Xml" << std::endl;
    m_Scope = Scope::Deck;
}

void DeckWriter::Card(int quantity, const std::string& name)
{
    CheckScope(Scope::Zone);
    m_Stream << R"Xml(        <card number=")Xml" << quantity << R"Xml(" name=")Xml" << name << R"Xml("/>)Xml" << std::endl;
}

void DeckWriter::CheckScope(Scope scope)
{
    assert(m_Scope == scope && "Invalid Scope !");
}