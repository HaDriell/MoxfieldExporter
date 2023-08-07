#pragma once

#include <filesystem>
#include <map>
#include <string>

bool FetchDeckFromMoxfield(const std::string& moxfieldDeckID, const std::filesystem::path& folder);
bool FetchUserDecklistFromMoxfield(const std::string& username, std::map<std::string, std::string>& decks);
std::string EscapeFilename(std::string_view filename);