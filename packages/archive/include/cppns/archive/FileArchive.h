#pragma once

#include <cassert>
#include <map>

#include "PathArchive.h"
#include "cppns/util/ErrorHandling.h"

#include "cppns/container/Vector.h"

#include "Archive.h"

namespace Error::File {
		CREATE_ERROR_TYPE(Open, "File Open Exception", "Couldn't open File {}!", Error::Runtime);
		CREATE_ERROR_TYPE(InvalidOpenMode, "Invalid Open Mode Exception", "Invalid Open Mode provided when trying to open file {}!", Error::Runtime);
		CREATE_ERROR_TYPE(InvalidOperation, "Invalid Operation Exception", "Invalid File Operation, Error: {}", Error::Runtime);
		CREATE_ERROR_TYPE(UnsupportedLineEndings, "Unsupported Line Endings Exception", "These line endings are unsupported, switch to LF or CRLF!", Error::Runtime);
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

	enum class Format : uint8 {
		BINARY,
		UTF8,
		// TODO: support below types
		UTF16,
		UTF32
	};

	enum class LineEnding : uint8 {
		LF,
		CRLF
	};

	static const char* getLineEndingString(const LineEnding& inLineEnding) noexcept {
		switch (inLineEnding) {
		case LineEnding::CRLF:
			return "\r\n";
		case LineEnding::LF:
		default:
			return "\n";
		}
	}
}

// An archive that can process files, uses standard c since it is faster
template <File::OpenType TOpenType, File::LineEnding TLineEnding>
class CBaseFileArchive {

public:

	explicit CBaseFileArchive(const CPathArchive& inFilePath): CBaseFileArchive(inFilePath.get()) {}

	explicit CBaseFileArchive(const std::string& inFilePath): filePath(inFilePath) {
		const char* mode = getOpenTypeMode(TOpenType);

		#if USING_MSVC
			fopen_s(&mFile, filePath.c_str(), mode);
		#else
			mFile = fopen(filePath.c_str(), mode);
		#endif

		Error::Assert{mFile};

		if (mFile == nullptr)
			throw Error::File::Open(filePath);

		lineEndings = getLineEndingFromFile();
		mBomOffset = getBomOffset();
	}

	virtual ~CBaseFileArchive() {
		fclose(mFile);
		Error::Assert{mFile};
	}

	[[nodiscard]] constexpr static bool isBinary() { return TOpenType & File::OpenType::BINARY; }

	[[nodiscard]] constexpr static bool isRead() { return TOpenType & File::OpenType::READ; }

	[[nodiscard]] constexpr static bool isWrite() { return TOpenType & File::OpenType::WRITE; }

	[[nodiscard]] constexpr static bool isReadWrite() { return isRead() && isWrite(); }

	[[nodiscard]] constexpr static bool isReadOnly() { return isRead() && !isWrite(); }

	[[nodiscard]] constexpr static bool isWriteOnly() { return isWrite() && !isRead(); }

	[[nodiscard]] bool isEnd() const { return feof(mFile); }

	[[nodiscard]] bool isEmpty() const { return getFileSize() <= 0; }

	[[nodiscard]] size_t getFileSize() const {
		const auto loc = tell();
		seekFromEnd(0);
		const auto res = tell();
		seekFromStart(loc);
		return res;
	}

	[[nodiscard]] size_t getLines() const {
		if constexpr (!isRead()) {
			throw Error::File::InvalidOperation("getLines() requires file read!");
		} else {
			char buffer[256];
			size_t lines = 1; // Initial Line

			const size_t loc = tell();
			seekFromStart(0);

			// fgets stops at buffer end or at newline.  Check for newline or continue reading
			while (fgets(buffer, sizeof(buffer), mFile)) {
				const size_t len = std::strlen(buffer);

				if (len == 0)
					continue;

				// Assume CRLF, read next char and add to lines if \n
				if (buffer[len - 1] == '\r') {
					if (const int next = fgetc(mFile); next == '\n') {
						lines++;
					}
				}

				if (buffer[len - 1] == '\n')
					lines++;
			}

			Error::Assert{mFile};

			seekFromStart(loc);

			return lines;
		}
	}

protected:

	long tell() const {
		return ftell(mFile) - mBomOffset;
	}

	void seek(const long inOffset) const {
		fseek(this->mFile, inOffset, SEEK_CUR);
		Error::Assert{mFile};
	}

	void seekFromStart(long inOffset) const {
		inOffset += mBomOffset;
		fseek(this->mFile, inOffset, SEEK_SET);
		Error::Assert{mFile};
	}

	void setStart() const {
		rewind(mFile);
		Error::Assert{mFile};
		if (mBomOffset > 0)
			seek(mBomOffset);
	}

	void seekFromEnd(const long inOffset) const {
		fseek(this->mFile, inOffset, SEEK_END);
		Error::Assert{mFile};
	}

	// Always use binary mode since they act the same on each platform
	const char* getOpenTypeMode(const File::OpenType& inOpenType) {
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
			return "wb+";
		}
		throw Error::File::InvalidOpenMode(filePath);
	}

	File::LineEnding getLineEndingFromFile() const {
		// Is being overwritten, we don't care about previous file's line endings
		if (isWrite())
			return TLineEnding;

		// Assume platform specific line endings in case there are none in the file
		auto lineEnding = TLineEnding;

		const long loc = tell();
		seekFromStart(0);

		int prev = EOF;
		int c;

		// Read the first line ending and assume other line endings are like that
		while ((c = fgetc(mFile)) != EOF) {
			Error::Assert{mFile};
			if (c == '\n') {
				lineEnding = prev == '\r' ? File::LineEnding::CRLF : File::LineEnding::LF;
				break;
			}
			if (prev == '\r') {
				throw Error::File::UnsupportedLineEndings();
			}
			prev = c;
		}

		Error::Assert{mFile};

		seekFromStart(loc);

		return lineEnding;
	}

	long getBomOffset() const {
		// Binary does not use BOM
		if (isBinary())
			return 0;

		constexpr static unsigned char BOM[] = { 0xEF, 0xBB, 0xBF };

		// Is being overwritten, we don't care about previous file's BOM
		// Instead we want to write it ourselves and skip over it
		if (isWrite()) {
			fwrite(BOM, 1, sizeof(BOM), mFile);
			return sizeof(BOM);
		}

		seekFromStart(0);

		unsigned char readValue[sizeof(BOM)];

		const size_t n = fread(readValue, 1, sizeof(readValue), mFile);
		Error::Assert{mFile};

		const long res = n == sizeof(readValue) && memcmp(readValue, BOM, sizeof(BOM)) == 0 ? sizeof(BOM) : 0;

		// Set to after BOM
		seekFromStart(res);

		return res;
	}

	File::LineEnding lineEndings;
	std::string filePath;
	FILE* mFile = nullptr;
	long mBomOffset = 0;

};

// An archive that can process files, uses standard c since it is faster
template <File::OpenType TOpenType, File::LineEnding TLineEnding>
class CBinaryFileArchive : public CBaseFileArchive<TOpenType, TLineEnding>, public CBinaryArchive {

public:

	using CBaseFileArchive<TOpenType, TLineEnding>::CBaseFileArchive;

protected:

	virtual size_t write(const void* inValue, const size_t inElementSize, const size_t inCount) override {
		const size_t res = fwrite(inValue, inElementSize, inCount, this->mFile);
		Error::Assert{this->mFile};
		return res;
	}

	virtual size_t read(void* inValue, const size_t inElementSize, const size_t inCount) override {
		const size_t res = fread(inValue, inElementSize, inCount, this->mFile);
		Error::Assert{this->mFile};
		return res;
	}

};

template <File::OpenType TOpenType, File::LineEnding TLineEnding>
class CStringFileArchive : public CBaseFileArchive<TOpenType, TLineEnding>, public CSArchive {

protected:

	using CBaseFileArchive<TOpenType, TLineEnding>::CBaseFileArchive;

	virtual size_t read(std::string& outValue) override {
		outValue.clear();
		char buffer[256];

		// Read 256 bytes and check for newline
		// fgets reads until a newline, so we dont have to worry about missing one
		while (fgets(buffer, sizeof(buffer), this->mFile)) {
			const std::size_t len = std::strlen(buffer);

			if (len == 0)
				continue;

			outValue.append(buffer, len);

			// LF line endings
			if (outValue.back() == '\n') {
				outValue.pop_back();

				// CRLF line endings
				if (outValue.back() == '\r')
					outValue.pop_back();

				break;
			}
		}

		Error::Assert{this->mFile};

		return outValue.size() * sizeof(std::string::value_type);
	}

	//TODO: wstring support?
	virtual size_t read(std::wstring& outValue) override {
		return 0;
	}

	virtual size_t write(const std::string& inValue) override {
		const std::string line = this->isEmpty() ? inValue : File::getLineEndingString(this->lineEndings) + inValue;
		const size_t res = fwrite(line.data(), sizeof(std::string::value_type), line.size(), this->mFile);
		Error::Assert{this->mFile};
		return res;
	}

	virtual size_t write(const std::wstring& inValue) override {
		return 0;
	}
};

#if USING_CRLF
template <File::OpenType TOpenType, File::LineEnding TLineEnding = File::LineEnding::CRLF>
#else
template <File::OpenType TOpenType, File::LineEnding TLineEnding = File::LineEnding::LF>
#endif
using CFileArchive = std::conditional_t<TOpenType & File::OpenType::BINARY, CBinaryFileArchive<TOpenType, TLineEnding>, CStringFileArchive<TOpenType, TLineEnding>>;
