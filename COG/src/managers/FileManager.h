#pragma once
/*!
	@file File.h
	@brief Handles file loading and saving using cereal and std::fstream.
	@author PrettyBinary
	@date 20/06/2018

	@class Pretty::Loaders::_FileManager
	@brief A singleton class that allows for file loading and saving.
	
	@warning __NO DIRECT ACCESS__
	- This class should not be accessed directly. Call Pretty::File::Instance() for access.

	Encapsulates all cereal and std::fstream procedures and methods, allowing ease of use.
	When used in conjunction with the Pretty Engine, full error checking is provided
	within the class. Additional support has been added for GLM.

	To serialize data, the templated function Serialize must be declared within a class/struct as follows:
	
		template <class Archive>
		void Serialize(Archive& archive)
		{
			archive(PRETTY_NVP(variable));
		}
	
	You can pass any number of arguments into the `archive`. The PRETTY_NVP(T) macro saves the variable to the file
	with the string representation of the variable name as its tag.
	
	> __Construction and Destruction__

	Call function:

	    Initialize(const std::string& extensionsFilePath)
	
	passing in a string to the file path where the list of supported extensions are stored.

	@warning Remember to clean up!
	Call function:

		Shutdown(const std::string& extensionsFilePath)
	
	before closing the program. Not doing so will mean that any extensions you have created
	during run-time will not be saved.

	> __Loading and Saving__

	To save data call:
	
		Save(const std::string& filePath, Args&&... args)

	`filePath` being the file where you wish to save the data (including extension), and `args` is any number of
	arguments storing the contents to be saved to the file passed in.

	The same procedure follows for loading data.
	
	To load data call:
	
		Load(const std::string& filePath, Args&&... args)	

	> __Creating Extensions__

	The Pretty Engine allows the user to add an infinite
	number of extensions of their choice i.e. .bin, .resource, .myAwesomeExtensionName, etc.

	To add an extension to the list of supported extensions call:

		CreateExtension(const std::string& extensionName, const Extension::Supported& extensionID)

	`extensionName` being the name you wish to use for the extension (no special characters or spaces), and
	`extensionID` being the parser ID you wish to use for this particular extension. A list of available parser ID's
	can be found in the Extension class.
	
	As an example, a binary parser would be:
		
		Extension::bin

	@note The cereal library supports standard parsers for XML, JSON and binary files. You may add further parsers
	if you wish - this has been left for the user to decide. The cereal library supports POD (plain old data),
	such as int, float, etc. Some additional supported types include std::string and std::vector. Raw pointers are
	NOT supported.
	Any polymorphic (user-defined) classes you wish to save must be registered first.

	For more information visit the [cereal](https://uscilab.github.io/cereal/) documentation.

*******************************************************************************************************************/

/*! @brief Creates a name value pair for the variable T with the same name as the variable. */
#define COG_NVP(T) CEREAL_NVP(T)

#include <fstream>
#include <pretty_cereal/includes.hpp>

#include "nfd/nfd.h"
#include "utilities/Singleton.h"
#include "utilities/Log.h"
#include "utilities/NoCopyOrAssignment.h"
#include "utilities/Tools.h"
	
/*! @brief Class responsible for listing supported extensions. */
class Extension : private NoCopyOrAssignment {

	friend class FileManager;

	Extension()		= default;
	~Extension()	= default;

	/*! @brief Internal enum only accessible by friend classes. */
	enum class Supported : unsigned int { ExtNull = 0, ExtBin, ExtJson, ExtXml };

public:
	static constexpr Supported bin	= Supported::ExtBin;	/*!< Binary parser ID. */
	static constexpr Supported json = Supported::ExtJson;	/*!< JSON parser ID. */
	static constexpr Supported xml	= Supported::ExtXml;	/*!< XML parser ID. */
};

	class FileManager : private NoCopyOrAssignment {

		friend class Singleton<FileManager>;

	public:
		bool Initialize(const std::string& extensionsFilePath);
		bool Shutdown(const std::string& extensionsFilePath);
		bool CreateExtension(std::string extensionName, const Extension::Supported& extensionID);

	public:
		template <typename... Args> bool Load(const std::string& filePath, Args&&... args);
		template <typename... Args> bool Save(const std::string& filePath, Args&&... args);
		template <typename... Args> bool OpenDialog(Args&&... args);
		template <typename... Args> bool SaveDialog(Args&&... args);

	private:
		/*! @brief { Serialized } A struct that stores the cache of supported extensions. */
		struct srExtensions {
			/*! @brief Allows cereal access to Serialize(Archive& archive). */
			friend class cereal::access;

			/*! @brief For ease of use. Stores the extension names and parser ID's. */
			typedef std::map<std::string, Extension::Supported> ExtensionCache;

			ExtensionCache cache;	/*!< A map of extension names and their corresponding parser ID's. */
			std::string filters;	/*!< A string of extension filters, used for the OS dialog. */

		private:
			/*! @brief Function that contains serialization data for loading and saving. */
			template <class Archive>
			void Serialize(Archive& archive)
			{
				archive(COG_NVP(cache),
						COG_NVP(filters));
			}
		};

	private:
		FileManager() = default;

	private:
		Extension::Supported	IsValid(const std::string& filePath);
		bool					CheckFormat(const std::string& filePath);

	private:
		template <typename ArchiveType, typename Stream, typename... Args>
		bool Parse(const std::string& filePath, Stream& stream, Args&&... args);

	private:
		srExtensions m_Extensions; /*!< An instance of struct srExtensions, stores the supported extensions. */
	};


	/*!
	@brief
		A function that loads data from a file.

	@param filePath
		The file path (including extension) you want to load from.

	@param args
		The variable(s) you wish to store the data loaded in from the file.

	@return
		True on success, false on failure.

	@see
		Save()
	*******************************************************************************************************************/
	template <typename... Args>
	bool FileManager::Load(const std::string& filePath, Args&&... args)
	{
		if constexpr (sizeof...(Args) > 0) {

			const auto& extension = IsValid(filePath);
			if (!EnumToType(extension)) { return false; }

			if (extension == Extension::bin) {
				std::ifstream stream(filePath, std::ios::binary);
				return Parse<cereal::BinaryInputArchive>(filePath, stream, args...);
			}
			else {
				std::ifstream stream(filePath, std::ios::in);
				if (extension == Extension::json)		{ return Parse<cereal::JSONInputArchive>(filePath, stream, args...); }
				else if (extension == Extension::xml)	{ return Parse<cereal::XMLInputArchive>(filePath, stream, args...); }
			}
		}
		
		COG_ERROR("{ FILE } Argument list empty when trying to load file:", filePath);

		return false;
	}


	/*!
	@brief
		A function that saves data to a file.

	@param filePath
		The file path (including extension) of where you want the data to be saved.

	@param args
		The variable(s) you wish to save to the file.

	@return
		True on success, false on failure.

	@see
		Load()
	*******************************************************************************************************************/
	template <typename... Args>
	bool FileManager::Save(const std::string& filePath, Args&&... args)
	{
		if constexpr (sizeof...(Args) > 0) {

			const auto& extension = IsValid(filePath);
			if (!EnumToType(extension)) { return false; }

			if (extension == Extension::bin) {
				std::ofstream stream(filePath, std::ios::binary);
				return Parse<cereal::BinaryOutputArchive>(filePath, stream, args...);
			}
			else {
				std::ofstream stream(filePath, std::ios::out);
				if (extension == Extension::json)		{ return Parse<cereal::JSONOutputArchive>(filePath, stream, args...); }
				else if (extension == Extension::xml)	{ return Parse<cereal::XMLOutputArchive>(filePath, stream, args...); }
			}
		}
			
		COG_ERROR("{ FILE } Argument list empty when trying to save file:", filePath);
			
		return false;
	}


	/*!
	@brief
		A function that loads data from a file using the OS file dialog.

	@param args
		The variable(s) you wish to store the data loaded in from the file.

	@return
		True on success, false on failure.

	@see
		SaveDialog()
	*******************************************************************************************************************/
	template <typename... Args>
	bool FileManager::OpenDialog(Args&&... args)
	{
		nfdchar_t* file = nullptr;
		nfdresult_t result = NFD_OpenDialog(m_Extensions.filters.c_str(), nullptr, &file);

		if (result == NFD_OKAY) {
			if (!Load(file, args...)) { free(file); return false; }
			free(file);
		}
		else if (result == NFD_CANCEL) { free(file); }
		else { COG_ERROR("{ File } Loading error:", NFD_GetError()); free(file); return false; }

		return true;
	}


	/*!
	@brief
		A function that saves data to a file using the OS file dialog.

	@param args
		The variable(s) you wish to save to the file.

	@return
		True on success, false on failure.

	@see
		OpenDialog()
	*******************************************************************************************************************/
	template <typename... Args>
	bool FileManager::SaveDialog(Args&&... args)
	{
		nfdchar_t *file = nullptr;
		nfdresult_t result = NFD_SaveDialog(m_Extensions.filters.c_str(), nullptr, &file);

		if (result == NFD_OKAY) {
			if (!Save(file, args...)) { free(file); return false; }
			free(file);
		}
		else if (result == NFD_CANCEL) { free(file); }
		else { COG_ERROR("{ File } Loading error:", NFD_GetError()); free(file); return false; }

		return true;
	}


	/*!
	@brief
		A helper function that creates the relevant cereal archive object and parses data.

	@param filePath
		The user supplied file path of where data should be serialized/deserialized.

	@param stream
		The stream object, either std::ifstream (for loading) or std::ofstream (for saving).

	@param args
		The user supplied variable(s) to be parsed.

	@return
		True on success, false on failure.
	*******************************************************************************************************************/
	template <typename ArchiveType, typename Stream, typename... Args>
	bool FileManager::Parse(const std::string& filePath, Stream& stream, Args&&... args)
	{
		if (!stream || !stream.is_open() || !stream.good()) {
			COG_ERROR("{ FILE } File path not found:", filePath);
			return false;
		}

		{
			ArchiveType archive(stream); archive(args...);
		}

		stream.close();

		COG_SUCCESS("{ FILE } File parsed successfully:", filePath);

		return true;
	}
	
	typedef Singleton<FileManager> File;