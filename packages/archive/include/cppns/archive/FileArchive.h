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
	enum class OpenType : uint8 {
		READ = 1,
		WRITE = 2,
		BINARY = 4,
		READWRITE = READ | WRITE,
		BINARY_READ = READ | BINARY,
		BINARY_WRITE = WRITE | BINARY,
		BINARY_READWRITE = READ | WRITE | BINARY
	};

	constexpr bool operator&(const OpenType& fst, const OpenType& snd) noexcept {
		return (static_cast<uint8>(fst) & static_cast<uint8>(snd)) != 0;
	}

	enum class LineEnding : uint8 {
		LF,
		CR,
		CRLF
	};

	static const char* getLineEndingString(const LineEnding& inLineEnding) noexcept {
		switch (inLineEnding) {
		case LineEnding::CRLF:
			return "\r\n";
		case LineEnding::CR:
			return "\r";
		case LineEnding::LF:
		default:
			return "\n";
		}
	}
}

// An archive that can process files, uses standard c since it is faster
template <File::OpenType TOpenType>
class CBaseFileArchive {

public:

	explicit CBaseFileArchive(const CPathArchive& inFilePath): CBaseFileArchive(inFilePath.get()) {}

	explicit CBaseFileArchive(const std::string& inFilePath) {
		const char* mode = getOpenTypeMode(TOpenType);

#if USING_MSVC
		fopen_s(&mFile, inFilePath.c_str(), mode);
#else
		mFile = fopen(inFilePath.c_str(), mode);
#endif
		if (mFile == nullptr)
			throw Error::File::OpenException();

		lineEndings = getLineEndingFromFile();
		mBomOffset = getBomOffset();

		// Put pointer after BOM
		seekFromStart(0);
	}

	virtual ~CBaseFileArchive() {
		fclose(mFile);
	}

	[[nodiscard]] constexpr static bool isBinary() { return TOpenType & File::OpenType::BINARY; }

	[[nodiscard]] constexpr static bool isRead() { return TOpenType & File::OpenType::READ; }

	[[nodiscard]] constexpr static bool isWrite() { return TOpenType & File::OpenType::WRITE; }

	[[nodiscard]] constexpr static bool isReadWrite() { return isRead() && isWrite(); }

	[[nodiscard]] constexpr static bool isReadOnly() { return isRead() && !isWrite(); }

	[[nodiscard]] constexpr static bool isWriteOnly() { return isWrite() && !isRead(); }

	[[nodiscard]] bool isEnd() const { return feof(mFile); }

	[[nodiscard]] size_t getFileSize() const {
		const auto loc = tell();
		seekFromEnd(0);
		const auto res = tell();
		seekFromStart(loc);
		return res;
	}

protected:

	long tell() const {
		return ftell(mFile) - mBomOffset;
	}

	void seek(const int inOffset) const {
		fseek(this->mFile, inOffset, SEEK_CUR);
	}

	void seekFromStart(int inOffset) const {
		inOffset += mBomOffset;
		fseek(this->mFile, inOffset, SEEK_SET);
	}

	void setStart() const {
		rewind(mFile);
		if (mBomOffset > 0)
			seek(mBomOffset);
	}

	void seekFromEnd(const int inOffset) const {
		fseek(this->mFile, inOffset, SEEK_END);
	}

	// Always use binary mode since they act the same on each platform
	static const char* getOpenTypeMode(const File::OpenType& inOpenType) noexcept {
		switch (inOpenType) {
		case File::OpenType::BINARY:
			return "b";
		case File::OpenType::READ:
		case File::OpenType::BINARY_READ:
			return "rb";
		case File::OpenType::WRITE:
		case File::OpenType::BINARY_WRITE:
			return "wb";
		case File::OpenType::READWRITE:
		case File::OpenType::BINARY_READWRITE:
			return "rwb";
		}
		return "";
	}

	File::LineEnding getLineEndingFromFile() const {

		const long loc = tell();
		setStart();

		int prev = EOF;
		int c;

		// Assume platform specific line endings in case there are non in the file
#if USING_CRLF
		auto lineEnding = File::LineEnding::CRLF;
#else
		auto lineEnding = File::LineEnding::LF;
#endif

		// Read the first line ending and assume other line endings are like that
		while ((c = fgetc(mFile)) != EOF) {
			if (c == '\n') {
				lineEnding = prev == '\r' ? File::LineEnding::CRLF : File::LineEnding::LF;
				break;
			}
			if (prev == '\r') {
				lineEnding = File::LineEnding::CR;
				break;
			}
			prev = c;
		}

		// If file ends with a trailing lone '\r' and nothing after it assume CR line endings
		if (c == EOF && prev == '\r') {
			lineEnding = File::LineEnding::CR;
		}

		seekFromStart(loc);

		return lineEnding;
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

	File::LineEnding lineEndings;
	FILE* mFile = nullptr;
	size_t mBomOffset;

};

// An archive that can process files, uses standard c since it is faster
template <File::OpenType TOpenType>
class CBinaryFileArchive : public CBaseFileArchive<TOpenType>, public CBinaryArchive {

public:

	using CBaseFileArchive<TOpenType>::CBaseFileArchive;

protected:

	virtual size_t write(const void* inValue, const size_t inElementSize, const size_t inCount) override {
		return fwrite(inValue, inElementSize, inCount, this->mFile);
	}

	virtual size_t read(void* inValue, const size_t inElementSize, const size_t inCount) override {
		return fread(inValue, inElementSize, inCount, this->mFile);
	}

};

template <File::OpenType TOpenType>
class CStringFileArchive : public CBaseFileArchive<TOpenType>, public CSArchive {

protected:

	using CBaseFileArchive<TOpenType>::CBaseFileArchive;

	// Reads file without changing cursor location
	[[nodiscard]] virtual std::string get() const override {
		const auto loc = ftell(this->mFile);
		auto res = this->readFile();
		seekFromStart(loc);
		return res;
	}

	virtual size_t read(std::string& outValue) override {
		outValue.clear();
		char buffer[256];

		// Read 256 bytes and check for line terminator
		// fgets reads until a line terminator, so we dont have to worry about over-reading
		while (fgets(buffer, sizeof(buffer), this->mFile)) {
			std::size_t len = std::strlen(buffer);

			if (len > 0 && buffer[len - 1] == '\n') {
				len--; // drop \n
				if (len > 0 && buffer[len - 1] == '\r') {
					len--; // If CRLF drop \r
				}
				outValue.append(buffer, len);
				break;
			}

			outValue.append(buffer, len);
		}

		return outValue.size() * sizeof(std::string::value_type);
	}

	virtual size_t write(const std::string& inValue) override {
		const std::string line = inValue + this->lineEndings;
		return fwrite(line.data(), sizeof(std::string::value_type), line.size(), this->mFile);
	}
};

template <File::OpenType TOpenType>
using CFileArchive = std::conditional_t<TOpenType & File::OpenType::BINARY, CBinaryFileArchive<TOpenType>, CStringFileArchive<TOpenType>>;
