/*!
	@file File.cpp
	@brief Handles file loading and saving using cereal and std::fstream.
	@author PrettyBinary
	@date 20/06/2018

*******************************************************************************************************************/

#include <algorithm>
#include <cctype>
#include "FileManager.h"
#include "utilities/String.h"

	/*!
	@brief
		A function that creates a user-defined extension and stores it into the extension cache.

	@param extensionName
		The name of the extension e.g. `bin` (must contain no spaces or special characters).

	@param extensionID
		The extension's parser ID to use, see Extension class for available parsers.

	@return
		True on success, false on failure.
	*******************************************************************************************************************/
	bool FileManager::CreateExtension(std::string extensionName, const Extension::Supported& extensionID)
	{
		const auto length	= extensionName.length();
		const auto ext		= String::RemoveSpecialCharacters(extensionName);
	
		if (ext.length() != length) { COG_WARNING("{ FILE } No special characters allowed. Extension has been changed to:", ext); }
		if (ext.empty())			{ COG_ERROR("{ FILE } An extension name cannot be empty. Extension not created."); return false; }
		
		if (!m_Extensions.cache.try_emplace("." + ext, extensionID).second) { 
			COG_ERROR("{ FILE } Extension already exists:", ext); 
			return false;
		}

		m_Extensions.filters += (ext + ";");

		COG_SUCCESS("{ FILE } Extension created successfully:", ext);

		return true;
	}


	/*!
	@brief
		A function that checks if a file extension is valid.

	@param filePath
		The file path (including extension).

	@return
		The extension parser ID, or 0 if invalid extension is supplied or if no extension parser ID is found.
	*******************************************************************************************************************/
	Extension::Supported FileManager::IsValid(const std::string& filePath)
	{
		auto token = filePath.find_last_of(".");
		if (token == std::string::npos) {
			COG_ERROR("{ FILE } Invalid extension. Use format myfile.ext:", filePath);
			return Extension::Supported::ExtNull;
		}

		const auto& name	= filePath.substr(token);
		const auto& found	= m_Extensions.cache.find(name);

		if (found == m_Extensions.cache.end()) {
			COG_ERROR("{ FILE } Extension doesn't exist. Create it first:", name);
			return Extension::Supported::ExtNull;
		}

		return found->second;
	}


	/*!
	@brief
		A function that initializes startup procedures for the class, primarily loading in supported extensions.

	@param filePath
		The file path (plus `.ext`) where the extensions are located.

	@return
		True on success, false on failure.

	@see
		Shutdown()
	*******************************************************************************************************************/
	bool FileManager::Initialize(const std::string& filePath)
	{
		if (!CheckFormat(filePath)) { return false; }

		std::ifstream stream(filePath, std::ios::in);
		return Parse<cereal::JSONInputArchive>(filePath, stream, m_Extensions);
	}


	/*!
	@brief
		A function that shuts down the class, primarily saving extensions created during run-time.

	@param filePath
		The file path (plus `.ext`) where the extensions are located.

	@return
		True on success, false on failure.

	@see
		Initialize()
	*******************************************************************************************************************/
	bool FileManager::Shutdown(const std::string& filePath)
	{
		if (!CheckFormat(filePath)) { return false; }

		std::ofstream stream(filePath, std::ios::out);
		return Parse<cereal::JSONOutputArchive>(filePath, stream, m_Extensions);
	}


	/*!
	@brief
		A function that checks the extension file supplied is in the correct format.

	@param filePath
		The file path (plus `.ext`) where the extensions are located.

	@return
		True on success, false on failure.
	*******************************************************************************************************************/
	bool FileManager::CheckFormat(const std::string& filePath)
	{
		if (!String::Contains(filePath, ".ext")) {
			COG_ERROR("{ FILE } Extension files must be in myfile.ext format");
			return false;
		}

		return true;
	}