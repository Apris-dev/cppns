#pragma once

#include <cassert>

#include "PathArchive.h"

#ifdef USING_CPPNS_CONTAINER
#include "cppns/container/Vector.h"
#else
#include <vector>
#endif

#include "Archive.h"

enum class EOpenType : uint8_t {
	READ = 1,
	WRITE = 2,
	BINARY = 4,
	READWRITE = READ | WRITE,
	BINARY_READ = READ | BINARY,
	BINARY_WRITE = WRITE | BINARY,
	BINARY_READWRITE = READ | WRITE | BINARY
};

constexpr bool operator&(const EOpenType& fst, const EOpenType& snd) {
	return (static_cast<uint8_t>(fst) & static_cast<uint8_t>(snd)) != 0;
}

// An archive that can process files, uses standard c since it is faster
template <EOpenType TOpenType>
class CBaseFileArchive {

public:

	// If archive goes out of scope, close the file
	virtual ~CBaseFileArchive() {
		close();
	}

	explicit CBaseFileArchive(const CPathArchive& inFilePath): CBaseFileArchive(inFilePath.get()) {}

	explicit CBaseFileArchive(const std::string& inFilePath) {
		std::string mode;
		if constexpr (isRead()) { mode += "r"; }
		if constexpr (isWrite()) { mode += "w"; }
		if constexpr (isBinary()) { mode += "b"; }

#ifdef _MSC_VER
		fopen_s(&mFile, inFilePath.c_str(), mode.c_str());
#else
		mFile = fopen(inFilePath.c_str(), mode.c_str());
#endif
		if (mFile) mIsOpen = true;
	}

	[[nodiscard]] constexpr static bool isBinary() { return TOpenType & EOpenType::BINARY; }

	[[nodiscard]] constexpr static bool isRead() { return TOpenType & EOpenType::READ; }

	[[nodiscard]] constexpr static bool isWrite() { return TOpenType & EOpenType::WRITE; }

	[[nodiscard]] constexpr static bool isReadWrite() { return isRead() && isWrite(); }

	[[nodiscard]] constexpr static bool isReadOnly() { return isRead() && !isWrite(); }

	[[nodiscard]] constexpr static bool isWriteOnly() { return isWrite() && !isRead(); }

	[[nodiscard]] bool isOpen() const { return mIsOpen; }

	[[nodiscard]] bool isEnd() const { return feof(mFile); }

	[[nodiscard]] size_t getFileSize() const {
		const auto loc = ftell(mFile);
		fseek(mFile, 0, SEEK_END);
		const auto res = ftell(mFile);
		fseek(mFile, loc, SEEK_SET);
		return res;
	}

	// Function to read from entire file with any type
	template <typename TType, class TAlloc = std::allocator<TType>>
#ifdef USING_CPPNS_CONTAINER
	TVector<TType> readFile(const bool inRemoveBOM = false) const {
#else
	std::vector<TType, TAlloc> readFile(const bool inRemoveBOM = false) {
#endif
		assert(this->isOpen());

		const auto fileSize = this->getFileSize();
		fseek(this->mFile, 0, SEEK_SET);

		std::vector<TType, TAlloc> vector(fileSize / sizeof(TType));
		const size_t bytesRead = fread(vector.data(), sizeof(TType), vector.size(), this->mFile);

		if (inRemoveBOM) this->removeBOM(vector.data(), bytesRead);

		return vector;
	}

	// Read into char vector, then reinterpret to a string
	[[nodiscard]] std::string readFile(const bool inRemoveBOM = false) const {
#ifdef USING_CPPNS_CONTAINER
		TVector<char> vector = readFile<char>(inRemoveBOM);
		return {vector.data(), vector.getSize()};
#else
		std::vector<char> vector = readFile<char>(inRemoveBOM);
		return {vector.data(), vector.size()};
#endif
	}

#ifdef USING_CPPNS_CONTAINER
	// Function to write to entire file with any type
	template <typename TType>
	void writeFile(TVector<TType> vector) {
		assert(this->isOpen());
		fwrite(vector.data(), sizeof(TType), vector.getSize(), this->mFile);
	}
#else
	// Function to write to entire file with any type
	template <typename TType, class TAlloc = std::allocator<TType>>
	void writeFile(std::vector<TType, TAlloc> vector) {
		assert(this->isOpen());
		fwrite(vector.data(), sizeof(TType), vector.size(), this->mFile);
	}
#endif

	void close() {
		if (isOpen()) {
			fclose(mFile);
			mIsOpen = false;
		}
	}

protected:

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
	bool mIsOpen = false;

};

// An archive that can process files, uses standard c since it is faster
template <EOpenType TOpenType>
class CBinaryFileArchive : public CBaseFileArchive<TOpenType>, public CArchive {

public:

	using CBaseFileArchive<TOpenType>::CBaseFileArchive;

protected:

	virtual size_t write(const void* inValue, const size_t inElementSize, const size_t inCount) override {
		assert(this->isOpen());
		return fwrite(inValue, inElementSize, inCount, this->mFile);
	}

	virtual size_t read(void* inValue, size_t const inElementSize, const size_t inCount) override {
		assert(this->isOpen());
		return fread(inValue, inElementSize, inCount, this->mFile);
	}

};

template <EOpenType TOpenType>
class CStringFileArchive : public CBaseFileArchive<TOpenType>, public CBaseStringArchive {

public:

	using CBaseFileArchive<TOpenType>::CBaseFileArchive;

	[[nodiscard]] virtual std::string get() const override {
		auto loc = ftell(this->mFile);
		auto res = this->readFile();
		fseek(this->mFile, loc, SEEK_SET);
		return res;
	}

	[[nodiscard]] std::string readLine(const bool inRemoveBOM = true) const {
		assert(this->isOpen());

		std::string line;
		char buffer[256];

		while (fgets(buffer, sizeof(buffer), this->mFile)) {
			line += buffer;

			// Stop if we read a full line
			if (line.back() == '\n')
				break;
		}

		if (inRemoveBOM) this->removeBOM(line.data(), line.size());

		this->removeEOL(line.data(), line.size());

		return line;
	}

	// Char has size of 1
	void writeLine(const std::string& string) const {
		assert(this->isOpen());
		const std::string line = string + LINE_ENDING;
		fwrite(line.data(), 1, line.size(), this->mFile);
	}

protected:

	virtual size_t read(std::string& outValue) override {
		assert(this->isOpen());
		outValue = readLine();
		return 0;
	}

	size_t read(const size_t amount) override {
		assert(this->isOpen());
		fseek(this->mFile, amount, SEEK_SET);
		return 0;
	}

	virtual size_t write(const std::string& inValue) override {
		assert(this->isOpen());
		return fwrite(inValue.data(), sizeof(std::string::value_type), inValue.size(), this->mFile);
	}
};

template <EOpenType TOpenType>
using CFileArchive = std::conditional_t<TOpenType & EOpenType::BINARY, CBinaryFileArchive<TOpenType>, CStringFileArchive<TOpenType>>;
