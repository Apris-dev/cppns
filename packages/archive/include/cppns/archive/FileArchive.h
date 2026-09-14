#pragma once

#include <cassert>
#include <map>

#include "PathArchive.h"
#include "cppns/util/ErrorHandling.h"

#include "cppns/container/Vector.h"

#include "Archive.h"

namespace Error::File {
		CREATE_ERROR_TYPE(OpenException, "File Open Exception", "Couldn't open File!", Error::Runtime);
}

namespace File {
	enum class OpenType : uint8_t {
		READ = 1,
		WRITE = 2,
		BINARY = 4,
		READWRITE = READ | WRITE,
		BINARY_READ = READ | BINARY,
		BINARY_WRITE = WRITE | BINARY,
		BINARY_READWRITE = READ | WRITE | BINARY
	};

	// Faster than if statements or memory access (like std::map)
	static const char* getOpenTypeMode(const OpenType& inOpenType) noexcept {
		switch (inOpenType) {
		case OpenType::READ:
			return "r";
		case OpenType::WRITE:
			return "w";
		case OpenType::BINARY:
			return "b";
		case OpenType::READWRITE:
			return "rw";
		case OpenType::BINARY_READ:
			return "rb";
		case OpenType::BINARY_WRITE:
			return "wb";
		case OpenType::BINARY_READWRITE:
			return "rwb";
		}
		return "";
	}

	constexpr bool operator&(const OpenType& fst, const OpenType& snd) noexcept {
		return (static_cast<uint8_t>(fst) & static_cast<uint8_t>(snd)) != 0;
	}
}

// Simple File wrapper, turns the C type into an auto-closing version
// FILE is significantly faster than c++ file manipulation
//TODO: always skip BOM, always include BOM in write
struct SFile {

	explicit SFile(const std::string& inFilePath, const File::OpenType& inOpenType) {
		const char* mode = File::getOpenTypeMode(inOpenType);

#if USING_MSVC
		fopen_s(&mFile, inFilePath.c_str(), mode);
#else
		mFile = fopen(inFilePath.c_str(), mode);
#endif
		if (mFile == nullptr)
			throw Error::File::OpenException();
	}

	~SFile() {
		fclose(mFile);
	}

	[[nodiscard]] bool isEnd() const { return feof(mFile); }

	// Reads file without changing cursor location
	[[nodiscard]] virtual std::string get() const {
		const auto loc = ftell(this->mFile);
		auto res = this->readFile();
		fseek(this->mFile, loc, SEEK_SET);
		return res;
	}

	[[nodiscard]] size_t getFileSize() const {
		const auto loc = ftell(mFile);
		seekFromEnd(0);
		const auto res = ftell(mFile);
		fseek(mFile, loc, SEEK_SET);
		return res;
	}

	// Read into char vector, then reinterpret to a string
	[[nodiscard]] std::string readFile() const {
		const auto fileSize = this->getFileSize();
		seek(0);

		std::string str;
		str.resize(fileSize / sizeof(std::string::value_type));
		fread(str.data(), sizeof(std::string::value_type), str.size(), this->mFile);

		return str;
	}

	void writeFile(const std::string& inString) const {
		fwrite(inString.data(), sizeof(std::string::value_type), inString.size(), this->mFile);
	}

	[[nodiscard]] std::string read() const {
		std::string line;
		char buffer[256];

		while (fgets(buffer, sizeof(buffer), this->mFile)) {
			line += buffer;

			// Stop if we read a full line
			if (line.back() == '\n')
				break;
		}

		this->removeEOL(line.data(), line.size());

		return line;
	}

	// Char has size of 1
	size_t write(const std::string& string) const {
		const std::string line = string + LINE_ENDING;
		return append(line);
	}

	size_t append(const std::string& inString) const {
		return fwrite(inString.data(), sizeof(std::string::value_type), inString.size(), this->mFile);
	}

private:

	void seek(int inOffset) const {
		inOffset += mBomOffset;
		fseek(this->mFile, inOffset, SEEK_SET);
	}

	void seekFromEnd(const int inOffset) const {
		fseek(this->mFile, inOffset, SEEK_END);
	}

	long getBomOffset() const {
		const long current = ftell(mFile);
		if (current < 0)
			return -1;

		constexpr static unsigned char BOM[] = { 0xEF, 0xBB, 0xBF };
		unsigned char readValue[sizeof(BOM)];

		fseek(mFile, 0, SEEK_SET);
		const size_t n = fread(readValue, 1, sizeof(readValue), mFile);
		fseek(mFile, current, SEEK_SET);

		return n == sizeof(readValue) && memcmp(readValue, BOM, sizeof(BOM)) == 0 ? sizeof(BOM) : 0;
	}

	// The BOM might cause issues with certain interpreters
	template <typename TDataType>
	static void removeBOM(TDataType* inData, const size_t inSize) {
		if (inSize <= 3) return;
		constexpr static unsigned char BOM[] = { 0xEF, 0xBB, 0xBF };
		if (!memcmp(inData, BOM, 3))
			memset(inData, ' ', 3);
	}

	// Remove the end of this line, assumes only a single line was given
	template <typename TDataType>
	static void removeEOL(TDataType* inData, const size_t inSize) {
		if (inSize == 0) return;
		constexpr static unsigned char EOL[] = LINE_ENDING;
		if (!memcmp(inData + inSize - 1, EOL, 1))
			memset(inData + inSize - 1, ' ', 1);
	}

	FILE* mFile = nullptr;
	size_t mBomOffset = getBomOffset();
};

//TODO: always skip BOM, never include BOM in write
struct SBinaryFile {

	explicit SBinaryFile(const std::string& inFilePath, const File::OpenType& inOpenType) {
		const char* mode = File::getOpenTypeMode(inOpenType);

#if USING_MSVC
		fopen_s(&mFile, inFilePath.c_str(), mode);
#else
		mFile = fopen(inFilePath.c_str(), mode);
#endif
		if (mFile == nullptr)
			throw Error::File::OpenException();
	}

	~SBinaryFile() {
		fclose(mFile);
	}

	[[nodiscard]] bool isEnd() const { return feof(mFile); }

	// Reads file without changing cursor location
	[[nodiscard]] virtual std::string get() const {
		const auto loc = ftell(this->mFile);
		auto res = this->readFile();
		fseek(this->mFile, loc, SEEK_SET);
		return res;
	}

	[[nodiscard]] size_t getFileSize() const {
		const auto loc = ftell(mFile);
		fseek(mFile, 0, SEEK_END);
		const auto res = ftell(mFile);
		fseek(mFile, loc, SEEK_SET);
		return res;
	}

	// Function to read from entire file with any type
	template <typename TType>
	[[nodiscard]] TVector<TType> readFile(const bool inRemoveBOM = false) const {
		const auto fileSize = this->getFileSize();
		fseek(this->mFile, 0, SEEK_SET);

		TVector<TType> vector(fileSize / sizeof(TType));
		const size_t bytesRead = fread(vector.data(), sizeof(TType), vector.getSize(), this->mFile);

		if (inRemoveBOM) this->removeBOM(vector.data(), bytesRead);

		return vector;
	}

	// Function to write to entire file with any type
	template <typename TType>
	void writeFile(TVector<TType> vector) const {
		fwrite(vector.data(), sizeof(TType), vector.getSize(), this->mFile);
	}

	size_t read(void* inValue, size_t const inElementSize, const size_t inCount) const {
		return fread(inValue, inElementSize, inCount, this->mFile);
	}

	size_t write(const void* inValue, const size_t inElementSize, const size_t inCount) const {
		return fwrite(inValue, inElementSize, inCount, this->mFile);
	}

private:

	// The BOM might cause issues with certain interpreters
	template <typename TDataType>
	static void removeBOM(TDataType* inData, const size_t inSize) {
		if (inSize <= 3) return;
		constexpr static unsigned char BOM[] = { 0xEF, 0xBB, 0xBF };
		if (!memcmp(inData, BOM, 3))
			memset(inData, ' ', 3);
	}

	FILE* mFile = nullptr;
};

// An archive that can process files, uses standard c since it is faster
template <File::OpenType TOpenType>
class CBaseFileArchive {

public:

	explicit CBaseFileArchive(const CPathArchive& inFilePath): CBaseFileArchive(inFilePath.get()) {}

	explicit CBaseFileArchive(const std::string& inFilePath): mFile(inFilePath, TOpenType) {}

	[[nodiscard]] constexpr static bool isBinary() { return TOpenType & File::OpenType::BINARY; }

	[[nodiscard]] constexpr static bool isRead() { return TOpenType & File::OpenType::READ; }

	[[nodiscard]] constexpr static bool isWrite() { return TOpenType & File::OpenType::WRITE; }

	[[nodiscard]] constexpr static bool isReadWrite() { return isRead() && isWrite(); }

	[[nodiscard]] constexpr static bool isReadOnly() { return isRead() && !isWrite(); }

	[[nodiscard]] constexpr static bool isWriteOnly() { return isWrite() && !isRead(); }

	[[nodiscard]] bool isEnd() const { return mFile.isEnd(); }

	[[nodiscard]] size_t getFileSize() const {
		return mFile.getFileSize();
	}

	// Function to read from entire file with any type
	template <typename TType>
	TVector<TType> readFile(const bool inRemoveBOM = false) const {
		return mFile.readFile(inRemoveBOM);
	}

	// Read into char vector, then reinterpret to a string
	[[nodiscard]] std::string readFile(const bool inRemoveBOM = false) const {
		TVector<char> vector = readFile<char>(inRemoveBOM);
		return {vector.data(), vector.getSize()};
	}

	// Function to write to entire file with any type
	template <typename TType>
	void writeFile(TVector<TType> vector) {
		fwrite(vector.data(), sizeof(TType), vector.getSize(), this->mFile);
	}

protected:

	SFile mFile;

};

// An archive that can process files, uses standard c since it is faster
template <File::OpenType TOpenType>
class CBinaryFileArchive : public CBaseFileArchive<TOpenType>, public CArchive {

public:

	using CBaseFileArchive<TOpenType>::CBaseFileArchive;

protected:

	virtual size_t write(const void* inValue, const size_t inElementSize, const size_t inCount) override {
		return this->mFile.write(inValue, inElementSize, inCount);
	}

	virtual size_t read(void* inValue, size_t const inElementSize, const size_t inCount) override {
		return this->mFile.read(inValue, inElementSize, inCount);
	}

};

template <File::OpenType TOpenType>
class CStringFileArchive : public CBaseFileArchive<TOpenType>, public CBaseStringArchive {

public:

	using CBaseFileArchive<TOpenType>::CBaseFileArchive;



	[[nodiscard]] std::string readLine(const bool inRemoveBOM = true) const {
		return this->mFile.readLine(inRemoveBOM);
	}

	// Char has size of 1
	void writeLine(const std::string& string) const {
		this->mFile.write(string);
	}

protected:

	[[nodiscard]] virtual std::string get() const override {
		return this->mFile.get();
	}

	virtual size_t read(std::string& outValue) override {
		outValue = readLine();
		return 0;
	}

	size_t read(const size_t amount) override {
		fseek(this->mFile, amount, SEEK_SET);
		return 0;
	}

	virtual size_t write(const std::string& inValue) override {
		return fwrite(inValue.data(), sizeof(std::string::value_type), inValue.size(), this->mFile);
	}
};

template <File::OpenType TOpenType>
using CFileArchive = std::conditional_t<TOpenType & File::OpenType::BINARY, CBinaryFileArchive<TOpenType>, CStringFileArchive<TOpenType>>;
