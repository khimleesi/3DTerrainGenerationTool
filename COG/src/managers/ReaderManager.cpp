#include "ReaderManager.h"
#include "utilities/Log.h"
#include <algorithm>
#include <vector>

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
ReaderManager::ReaderManager()
{
	COG_LOG("[FILE MANAGER CONSTRUCT]", COG_LOG_EMPTY, LOG_BREAK);
}


/*******************************************************************************************************************
	Opens a text file for reading data only
*******************************************************************************************************************/
bool ReaderManager::OpenForReading(const std::string& fileLocation)
{
	m_file.open(fileLocation.c_str(), std::ifstream::in);

	if (!m_file.is_open())	{ COG_LOG("[FILE] File doesn't exist: ", fileLocation.c_str(), LOG_ERROR); return false; }
	if (!m_file.good())		{ COG_LOG("[FILE] File corrupted or a file is already open: ", fileLocation.c_str(), LOG_ERROR); return false; }

	COG_LOG("[FILE] File opened successfully: ", fileLocation.c_str(), LOG_SUCCESS);

	return true;
}


/*******************************************************************************************************************
	Opens a binary file for reading data only
*******************************************************************************************************************/
bool ReaderManager::BinaryOpenForReading(const std::string& fileLocation)
{
	m_file.open(fileLocation.c_str(), std::ifstream::in | std::ios::binary);

	if (!m_file.is_open()) { COG_LOG("[FILE] File doesn't exist: ", fileLocation.c_str(), LOG_ERROR); return false; }
	if (!m_file.good()) { COG_LOG("[FILE] File corrupted or a file is already open: ", fileLocation.c_str(), LOG_ERROR); return false; }

	COG_LOG("[FILE] File opened successfully: ", fileLocation.c_str(), LOG_SUCCESS);

	return true;
}


/*******************************************************************************************************************
	Opens a binary file for writing data only
*******************************************************************************************************************/
bool ReaderManager::BinaryOpenForWriting(const std::string& fileLocation)
{
	m_file.open(fileLocation.c_str(), std::ifstream::out | std::ios::binary);

	if (!m_file.is_open()) { COG_LOG("[FILE] File doesn't exist: ", fileLocation.c_str(), LOG_ERROR); return false; }
	if (!m_file.good()) { COG_LOG("[FILE] File corrupted or a file is already open: ", fileLocation.c_str(), LOG_ERROR); return false; }

	COG_LOG("[FILE] File opened successfully: ", fileLocation.c_str(), LOG_SUCCESS);

	return true;
}


/*******************************************************************************************************************
	Function that closes a file
*******************************************************************************************************************/
void ReaderManager::Close(const std::string& fileLocation) {

	m_file.close();

	COG_LOG("[FILE] File closed successfully: ", fileLocation.c_str(), LOG_SUCCESS);
}


/*******************************************************************************************************************
	Function that gets all the data from a file and stores it in to a string
*******************************************************************************************************************/
std::istream& ReaderManager::ExtractFileData() { return std::getline(m_file, m_fileData); }


/*******************************************************************************************************************
	Function that checks if certain data within a file exists
*******************************************************************************************************************/
size_t ReaderManager::FileDataContains(const std::string &data) { return Find(data) == 0; }


/*******************************************************************************************************************
	Function that cuts the string containing the file data, from a start to an end position
*******************************************************************************************************************/
std::string ReaderManager::CutFileData(int startPosition, int endPosition) {
	
	return m_fileData.substr(startPosition, endPosition);
}


/*******************************************************************************************************************
	Function that moves the file pointer from a starting position to an end position
*******************************************************************************************************************/
const std::istream& ReaderManager::Seek(int startPosition, int endPosition) { return m_file.seekg(startPosition, endPosition); }


/*******************************************************************************************************************
	Function that finds data within a file and returns its position as a number
*******************************************************************************************************************/
size_t ReaderManager::Find(const std::string &data) { return m_fileData.find(data); }


/*******************************************************************************************************************
	Function that returns the size of a file in bytes
*******************************************************************************************************************/
int ReaderManager::GetFileSize()
{
	Seek(0, std::ios_base::end);
	int fileSize = (int)m_file.tellg();
	Seek(0, std::ios_base::beg);

	return fileSize;
}


/*******************************************************************************************************************
	Get object data from a file, depending on the objectName passed to the function
*******************************************************************************************************************/
bool ReaderManager::GetObjectData(const std::string &objectName, std::map<std::string, std::map<std::string, std::string>>& container) {

	using namespace file_constants;

	bool objectFound = false;

	while (ExtractFileData()) {

		if (FileDataContains(TYPE_BEGIN)) {
			
			std::string fileObjectName = CutFileData(TYPE_BEGIN_OFFSET);

			if (objectName == fileObjectName) { objectFound = true; }
		} 
		else if (FileDataContains(TYPE_END) && objectFound) { m_file.close(); }
		else if (objectFound) { 
			
			if (FileDataContains(LINE_BREAK)) { continue; }
			
			int equalPosition		= Find(DIVIDER);
			std::string dataKey		= CutFileData(0, equalPosition);
			std::string dataValue	= CutFileData(equalPosition + DIVIDER_OFFSET);
	
			dataValue.erase(remove(dataValue.begin(), dataValue.end(), '\t'), dataValue.end());
			
			container[objectName].try_emplace(dataKey, dataValue);
		}
		else if (FileDataContains(END_OF_FILE) && !objectFound) {
			COG_LOG("[FILE] Object not found in file: ", objectName.c_str(), LOG_ERROR);
			m_file.close();
			return false;
		}
	}

	return true;
}


/*******************************************************************************************************************
	Prints all the game object data stored in the m_objectData map
*******************************************************************************************************************/
void ReaderManager::PrintObjectData() {
	
	if (!m_objectData.empty()) {

		COG_LOG("The following object data was stored: ", COG_LOG_EMPTY, LOG_MESSAGE);

		for (auto const& object : m_objectData) {
			COG_LOG("Object: ", object.first.c_str(), LOG_MESSAGE);
			
			for (auto const& data : object.second) {
				COG_LOG("Key: ", data.first.c_str(), LOG_MESSAGE);
				COG_LOG("Value: ", data.second.c_str(), LOG_MESSAGE);
			}
		}
	}

	else { COG_LOG("[FILE] Object data container is COG_LOG_EMPTY", COG_LOG_EMPTY, LOG_WARN); }
}