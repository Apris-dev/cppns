#pragma once

#include <cassert>

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

	static const char16_t* getLineEndingu16string(const LineEnding& inLineEnding) noexcept {
		switch (inLineEnding) {
		case LineEnding::CRLF:
			return u"\r\n";
		case LineEnding::LF:
		default:
			return u"\n";
		}
	}

	static const char32_t* getLineEndingu32string(const LineEnding& inLineEnding) noexcept {
		switch (inLineEnding) {
		case LineEnding::CRLF:
			return U"\r\n";
		case LineEnding::LF:
		default:
			return U"\n";
		}
	}
}

// An archive that can process files, uses standard c since it is faster
//TODO: use template specialization w/ opentype and format
template <File::Format TFormat, File::OpenType TOpenType, File::LineEnding TLineEnding>
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
			if constexpr (TFormat == File::Format::UTF32) {
				String::UTF32 str;
				size_t lines = 0; // Initial Line

				const size_t loc = tell();
				seekFromStart(0);

				while (readUntilLE(str) != EOF) {
					lines++;
				}

				Error::Assert{mFile};

				seekFromStart(loc);

				return lines;
			} else if constexpr (TFormat == File::Format::UTF16) {
				String::UTF16 str;
				size_t lines = 0; // Initial Line

				const size_t loc = tell();
				seekFromStart(0);

				while (readUntilLE(str) != EOF) {
					lines++;
				}

				Error::Assert{mFile};

				seekFromStart(loc);

				return lines;
			} else {
				String::Generic str;
				size_t lines = 0; // Initial Line

				const size_t loc = tell();
				seekFromStart(0);

				while (readUntilLE(str) != EOF) {
					lines++;
				}

				Error::Assert{mFile};

				seekFromStart(loc);

				return lines;
			}
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

	template <typename CharT>
	size_t readUntil(std::basic_string<CharT>& out, CharT delim) const {
		if (feof(mFile))
			return EOF;

		out.clear();

		size_t bytesRead = 0;
		while (true) {
			CharT c;

			if (fread(&c, sizeof(c), 1, mFile) != 1)
				break;

			bytesRead += sizeof(c);

			if (c == delim)
				break;

			out.push_back(c);
		}

		return bytesRead;
	}

	template <typename CharT>
	size_t readUntilLE(std::basic_string<CharT>& out) const {
		const size_t res = readUntil(out, static_cast<CharT>('\n'));

		if (out.back() == static_cast<CharT>('\r')) {
			out.pop_back();
			return res - 1;
		}

		return res;
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

		if constexpr (TFormat == File::Format::UTF32) {
			// Read the first line ending and assume other line endings are like that
			String::UTF32 str;
			while (readUntil(str, U'\n') != EOF) {
				Error::Assert{mFile};
				if (str.back() == U'\r') {
					lineEnding = File::LineEnding::CRLF;
				} else {
					lineEnding = File::LineEnding::LF;
				}
			}
		} else if constexpr (TFormat == File::Format::UTF16) {
			// Read the first line ending and assume other line endings are like that
			String::UTF16 str;
			while (readUntil(str, u'\n') != EOF) {
				Error::Assert{mFile};
				if (str.back() == u'\r') {
					lineEnding = File::LineEnding::CRLF;
				} else {
					lineEnding = File::LineEnding::LF;
				}
			}
		} else {
			// Read the first line ending and assume other line endings are like that
			String::Generic str;
			while (readUntil(str, '\n') != EOF) {
				Error::Assert{mFile};
				if (str.back() == '\r') {
					lineEnding = File::LineEnding::CRLF;
				} else {
					lineEnding = File::LineEnding::LF;
				}
			}
		}

		Error::Assert{mFile};

		seekFromStart(loc);

		return lineEnding;
	}

	long getBomOffset() const {
		// Binary does not use BOM
		if constexpr (isBinary())
			return 0;

		std::vector<unsigned char> BOM;

		// TODO: support Big Endian
		if constexpr (TFormat == File::Format::UTF32) {
			BOM = { 0xFF, 0xFE, 0x00, 0x00 };
		} else if constexpr (TFormat == File::Format::UTF16) {
			BOM = { 0xFF, 0xFE };
		} else {
			BOM = { 0xEF, 0xBB, 0xBF };
		}

		// Is being overwritten, we don't care about previous file's BOM
		// Instead we want to write it ourselves and skip over it
		if (isWrite()) {
			fwrite(BOM.data(), sizeof(BOM[0]), BOM.size(), mFile);
			return BOM.size();
		}

		seekFromStart(0);

		std::vector<unsigned char> readValue;
		readValue.resize(BOM.size());

		const size_t n = fread(readValue.data(), sizeof(readValue[0]), readValue.size(), mFile);
		Error::Assert{mFile};

		const long res = n == readValue.size() && memcmp(readValue.data(), BOM.data(), BOM.size()) == 0 ? BOM.size() : 0;

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
class CBinaryFileArchive : public CBaseFileArchive<File::Format::BINARY, TOpenType, TLineEnding>, public CBinaryArchive {

public:

	using CBaseFileArchive<File::Format::BINARY, TOpenType, TLineEnding>::CBaseFileArchive;

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
class CStringFileArchive : public CBaseFileArchive<File::Format::UTF8, TOpenType, TLineEnding>, public CSArchive {

protected:

	using CBaseFileArchive<File::Format::UTF8, TOpenType, TLineEnding>::CBaseFileArchive;

	virtual size_t read(CString& outValue) override {
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
			if (outValue.bottom() == '\n') {
				outValue.pop();

				// CRLF line endings
				if (outValue.bottom() == '\r')
					outValue.pop();

				break;
			}
		}

		Error::Assert{this->mFile};

		return outValue.getSize() * sizeof(CString::TType);
	}

	virtual size_t write(const CString& inValue) override {
		const CString line = this->isEmpty() ? inValue : File::getLineEndingString(this->lineEndings) + inValue;
		const size_t res = fwrite(line.data(), sizeof(CString::TType), line.getSize(), this->mFile);
		Error::Assert{this->mFile};
		return res;
	}
};

template <File::OpenType TOpenType, File::LineEnding TLineEnding>
class CU16StringFileArchive : public CBaseFileArchive<File::Format::UTF16, TOpenType, TLineEnding>, public CSArchive {

protected:

	using CBaseFileArchive<File::Format::UTF16, TOpenType, TLineEnding>::CBaseFileArchive;

	virtual size_t read(CString& outValue) override {
		String::UTF16 str;
		this->readUntilLE(str);

		Error::Assert{this->mFile};

		outValue = CString{str};

		return outValue.getSize() * sizeof(CString::TType);
	}

	virtual size_t write(const CString& inValue) override {
		const String::UTF16 line = this->isEmpty() ? inValue.toUTF16() : File::getLineEndingu16string(this->lineEndings) + inValue.toUTF16();
		const size_t res = fwrite(line.data(), sizeof(String::UTF16::value_type), line.size(), this->mFile);
		Error::Assert{this->mFile};
		return res;
	}
};

template <File::OpenType TOpenType, File::LineEnding TLineEnding>
class CU32StringFileArchive : public CBaseFileArchive<File::Format::UTF32, TOpenType, TLineEnding>, public CSArchive {

protected:

	using CBaseFileArchive<File::Format::UTF32, TOpenType, TLineEnding>::CBaseFileArchive;

	virtual size_t read(CString& outValue) override {
		String::UTF32 str;
		this->readUntilLE(str);

		Error::Assert{this->mFile};

		outValue = CString{str};

		return outValue.getSize() * sizeof(CString::TType);
	}

	virtual size_t write(const CString& inValue) override {
		const String::UTF32 line = this->isEmpty() ? inValue.toUTF32() : File::getLineEndingu32string(this->lineEndings) + inValue.toUTF32();
		const size_t res = fwrite(line.data(), sizeof(String::UTF32::value_type), line.size(), this->mFile);
		Error::Assert{this->mFile};
		return res;
	}
};

#if USING_CRLF
template <File::OpenType TOpenType, File::LineEnding TLineEnding = File::LineEnding::CRLF>
#else
template <File::OpenType TOpenType, File::LineEnding TLineEnding = File::LineEnding::LF>
#endif
using CFileArchive = std::conditional_t<TOpenType & File::OpenType::BINARY, CBinaryFileArchive<TOpenType, TLineEnding>, CStringFileArchive<TOpenType, TLineEnding>>;
