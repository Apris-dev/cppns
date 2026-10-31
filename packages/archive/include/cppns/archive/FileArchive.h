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
		READWRITE = READ | WRITE,
	};

	constexpr bool operator&(const OpenType& fst, const OpenType& snd) noexcept {
		return (static_cast<uint8>(fst) & static_cast<uint8>(snd)) != 0;
	}

	enum class Format : uint8 {
		BINARY,
		UTF8,
		UTF16LE,
		UTF16BE,
		UTF32LE,
		UTF32BE
	};

	enum class LineEnding : uint8 {
		LF,
		CRLF
	};

	inline bool isUTF8(FILE* inFile) {

		unsigned char buf[4096];
		const size_t n = fread(buf, 1, sizeof(buf), inFile);

		size_t i = 0;

		while (i < n) {
			const unsigned char c = buf[i];
			if (c == 0) return false;
			if (c < 0x80) { ++i; continue; }

			size_t len;
			unsigned char lo = 0x80, hi = 0xBF;

			if (c >= 0xC2 && c <= 0xDF) len = 2;
			else if (c == 0xE0) { len = 3; lo = 0xA0; }
			else if (c == 0xED) { len = 3; hi = 0x9F; }
			else if (c >= 0xE1 && c <= 0xEF) len = 3;
			else if (c == 0xF0) { len = 4; lo = 0x90; }
			else if (c == 0xF4) { len = 4; hi = 0x8F; }
			else if (c >= 0xF1 && c <= 0xF3) len = 4;
			else return false;

			for (size_t k = 1; k < len; ++k) {
				if (i + k >= n) {
					// Ran out of bytes mid-character.
					return n == sizeof(buf);
				}
				const unsigned char b = buf[i + k];
				const unsigned char l = (k == 1) ? lo : 0x80;
				if (const unsigned char h = (k == 1) ? hi : 0xBF; b < l || b > h) return false;
			}
			i += len;
		}
		return true;
	}

	inline Format getFormatFromFile(size_t& outSize, FILE* inFile) {
		// Set file to start
		const long loc = ftell(inFile);
		fseek(inFile, 0, SEEK_SET);

		unsigned char b[4] = {0, 0, 0, 0};
		const size_t n = fread(b, 1, sizeof(b), inFile);

		// Assume BINARY with no BOM
		auto format = Format::BINARY;
		outSize = 0;

		// Check UTF32 first, since they are similar to UTF16's BOM
		if (n >= 4 && b[0] == 0xFF && b[1] == 0xFE && b[2] == 0x00 && b[3] == 0x00) {
			format = Format::UTF32LE;
			outSize = 4;
		} else if (n >= 4 && b[0] == 0x00 && b[1] == 0x00 && b[2] == 0xFE && b[3] == 0xFF) {
			format = Format::UTF32BE;
			outSize = 4;
		} else if (n >= 3 && b[0] == 0xEF && b[1] == 0xBB && b[2] == 0xBF) {
			format = Format::UTF8;
			outSize = 3;
		} else if (n >= 2 && b[0] == 0xFF && b[1] == 0xFE) {
			format = Format::UTF16LE;
			outSize = 2;
		} else if (n >= 2 && b[0] == 0xFE && b[1] == 0xFF) {
			format = Format::UTF16BE;
			outSize = 2;
		} else {
			// No BOM, since BOM in UTF8 is optional we need a more in-depth check
			fseek(inFile, 0, SEEK_SET);
			if (isUTF8(inFile)) {
				format = Format::UTF8;
			}
		}

		// Reset the file back to the original location
		fseek(inFile, loc, SEEK_SET);
		return format;
	}
}

// An archive that can process files, uses standard c since it is faster
template <File::OpenType TOpenType>
class CBaseFileArchive {

public:

	explicit CBaseFileArchive(const CPathArchive& inFilePath):
	filePath(inFilePath) {
		const char* mode = getOpenTypeMode(TOpenType);

		#if USING_MSVC
			fopen_s(&mFile, filePath.get().array(), mode);
		#else
			mFile = fopen(filePath.get().array(), mode);
		#endif

		Error::Assert{mFile};

		if (mFile == nullptr)
			throw Error::File::Open(filePath.get().array());
	}

	virtual ~CBaseFileArchive() {
		fclose(mFile);
		Error::Assert{mFile};
	}

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

	[[nodiscard]] virtual size_t getLines() const = 0;

protected:

	void init(const TVector<unsigned char>& inBOM, const File::LineEnding inLineEndings) {
		if (mFile == nullptr)
			throw Error::File::InvalidOperation("Tried to init before opening file!");
		mBomOffset = getBomOffset(inBOM);
		lineEndings = inLineEndings;
	}

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
	const char* getOpenTypeMode(const File::OpenType& inOpenType) const {
		switch (inOpenType) {
		case File::OpenType::READ:
			return "rb";
		case File::OpenType::WRITE:
			return "wb";
		case File::OpenType::READWRITE:
			return "wb+";
		}
		throw Error::File::InvalidOpenMode(filePath.get().array());
	}

	template <String::Endian Endian, typename CharT>
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

			c = String::toEndian<Endian>(c);

			if (c == delim)
				break;

			out.push_back(c);
		}

		return bytesRead;
	}

	template <String::Endian Endian, typename CharT>
	size_t readUntilLE(std::basic_string<CharT>& out) const {
		const size_t res = readUntil<Endian>(out, static_cast<CharT>('\n'));

		if (!out.empty() && out.back() == static_cast<CharT>('\r')) {
			out.pop_back();
			return res - 1;
		}

		return res;
	}

	long getBomOffset(TVector<unsigned char> BOM) const {
		if (BOM.isEmpty())
			return 0;

		// Is being overwritten, we don't care about previous file's BOM
		// Instead we want to write it ourselves and skip over it
		if (isWrite()) {
			fwrite(BOM.data(), sizeof(BOM[0]), BOM.getSize(), mFile);
			return BOM.getSize();
		}

		seekFromStart(0);

		std::vector<unsigned char> readValue;
		readValue.resize(BOM.getSize());

		const size_t n = fread(readValue.data(), sizeof(readValue[0]), readValue.size(), mFile);
		Error::Assert{mFile};

		const long res = n == readValue.size() && memcmp(readValue.data(), BOM.data(), BOM.getSize()) == 0 ? BOM.getSize() : 0;

		// Set to after BOM
		seekFromStart(res);

		return res;
	}

	File::LineEnding lineEndings;
	CPathArchive filePath;
	FILE* mFile = nullptr;
	long mBomOffset = 0;

};

#if USING_CRLF
template <File::OpenType TOpenType, File::Format TFileFormat = File::Format::UTF8, File::LineEnding TLineEnding = File::LineEnding::CRLF>
#else
template <File::OpenType TOpenType, File::Format TFileFormat = File::Format::UTF8, File::LineEnding TLineEnding = File::LineEnding::LF>
#endif
class CFileArchive : public CBaseFileArchive<TOpenType>, public CSArchive {
	using CBaseFileArchive<TOpenType>::CBaseFileArchive;
};

// An archive that can process files, uses standard c since it is faster
template <File::OpenType TOpenType, File::LineEnding TLineEnding>
class CFileArchive<TOpenType, File::Format::BINARY, TLineEnding> : public CBaseFileArchive<TOpenType>, public CBinaryArchive {
public:

	explicit CFileArchive(const CPathArchive& inFilePath):
	CBaseFileArchive<TOpenType>(inFilePath) {
		this->init(TVector<unsigned char>{}, getLineEndingFromFile());
	}

	[[nodiscard]] virtual size_t getLines() const override {
		if constexpr (!this->isRead()) {
			throw Error::File::InvalidOperation("getLines() requires file read!");
		} else {
			String::Generic str;
			size_t lines = 0; // Initial Line

			const size_t loc = this->tell();
			this->seekFromStart(0);

			while (this->template readUntilLE<String::Endian::NATIVE>(str) != EOF) {
				lines++;
			}

			Error::Assert{this->mFile};

			this->seekFromStart(loc);

			return lines;
		}
	}

protected:

	static const char* getLineEndingString(const File::LineEnding& inLineEnding) noexcept {
		switch (inLineEnding) {
		case File::LineEnding::CRLF:
			return "\r\n";
		case File::LineEnding::LF:
		default:
			return "\n";
		}
	}

	File::LineEnding getLineEndingFromFile() const {
		// Is being overwritten, we don't care about previous file's line endings
		if (this->isWrite())
			return TLineEnding;

		// Assume platform specific line endings in case there are none in the file
		const long loc = this->tell();
		this->seekFromStart(0);

		// Read the first line ending and assume other line endings are like that
		String::Generic str;
		while (this->template readUntil<String::Endian::NATIVE>(str, '\n') != EOF) {
			this->seekFromStart(loc);
			if (str.back() == '\r') {
				return  File::LineEnding::CRLF;
			}
			return  File::LineEnding::LF;
		}

		Error::Assert{this->mFile};
		this->seekFromStart(loc);

		// Couldn't find a line ending
		return TLineEnding;
	}

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
class CFileArchive<TOpenType, File::Format::UTF8, TLineEnding> : public CBaseFileArchive<TOpenType>, public CSArchive {

public:

	explicit CFileArchive(const CPathArchive& inFilePath):
	CBaseFileArchive<TOpenType>(inFilePath) {
		this->init(TVector<unsigned char>{ 0xEF, 0xBB, 0xBF }, getLineEndingFromFile());
	}

	[[nodiscard]] virtual size_t getLines() const override {
		if constexpr (!this->isRead()) {
			throw Error::File::InvalidOperation("getLines() requires file read!");
		} else {
			String::Generic str;
			size_t lines = 0; // Initial Line

			const size_t loc = this->tell();
			this->seekFromStart(0);

			while (this->template readUntilLE<String::Endian::NATIVE>(str) != EOF) {
				lines++;
			}

			Error::Assert{this->mFile};

			this->seekFromStart(loc);

			return lines;
		}
	}

protected:

	static const char* getLineEndingString(const File::LineEnding& inLineEnding) noexcept {
		switch (inLineEnding) {
		case File::LineEnding::CRLF:
			return "\r\n";
		case File::LineEnding::LF:
		default:
			return "\n";
		}
	}

	File::LineEnding getLineEndingFromFile() const {
		// Is being overwritten, we don't care about previous file's line endings
		if (this->isWrite())
			return TLineEnding;

		// Assume platform specific line endings in case there are none in the file
		const long loc = this->tell();
		this->seekFromStart(0);

		// Read the first line ending and assume other line endings are like that
		String::Generic str;
		while (this->template readUntil<String::Endian::NATIVE>(str, '\n') != EOF) {
			Error::Assert{this->mFile};
			this->seekFromStart(loc);
			if (str.back() == '\r') {
				return  File::LineEnding::CRLF;
			}
			return  File::LineEnding::LF;
		}

		Error::Assert{this->mFile};
		this->seekFromStart(loc);

		// Couldn't find a line ending
		return TLineEnding;
	}

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
		const CString line = this->isEmpty() ? inValue : getLineEndingString(this->lineEndings) + inValue;
		const size_t res = fwrite(line.data(), sizeof(CString::TType), line.getSize(), this->mFile);
		Error::Assert{this->mFile};
		return res;
	}
};

template <File::OpenType TOpenType, File::LineEnding TLineEnding>
class CFileArchive<TOpenType, File::Format::UTF16LE, TLineEnding> : public CBaseFileArchive<TOpenType>, public CSArchive {

public:

	explicit CFileArchive(const CPathArchive& inFilePath):
	CBaseFileArchive<TOpenType>(inFilePath) {
		this->init(TVector<unsigned char>{ 0xFF, 0xFE }, getLineEndingFromFile());
	}

	[[nodiscard]] virtual size_t getLines() const override {
		if constexpr (!this->isRead()) {
			throw Error::File::InvalidOperation("getLines() requires file read!");
		} else {
			String::UTF16 str;
			size_t lines = 0; // Initial Line

			const size_t loc = this->tell();
			this->seekFromStart(0);

			while (this->template readUntilLE<String::Endian::LITTLE>(str) != EOF) {
				lines++;
			}

			Error::Assert{this->mFile};

			this->seekFromStart(loc);

			return lines;
		}
	}

protected:

	static const char16_t* getLineEndingString(const File::LineEnding& inLineEnding) noexcept {
		switch (inLineEnding) {
		case File::LineEnding::CRLF:
			return u"\r\n";
		case File::LineEnding::LF:
		default:
			return u"\n";
		}
	}

	File::LineEnding getLineEndingFromFile() const {
		// Is being overwritten, we don't care about previous file's line endings
		if (this->isWrite())
			return TLineEnding;

		// Assume platform specific line endings in case there are none in the file
		const long loc = this->tell();
		this->seekFromStart(0);

		// Read the first line ending and assume other line endings are like that
		String::UTF16 str;
		while (this->template readUntil<String::Endian::LITTLE>(str, u'\n') != EOF) {
			Error::Assert{this->mFile};
			if (str.back() == u'\r') {
				return File::LineEnding::CRLF;
			}
			return File::LineEnding::LF;
		}

		Error::Assert{this->mFile};
		this->seekFromStart(loc);

		// Couldn't find a line ending
		return TLineEnding;
	}

	virtual size_t read(CString& outValue) override {
		String::UTF16 str;
		this->template readUntilLE<String::Endian::LITTLE>(str);

		Error::Assert{this->mFile};

		outValue = CString{str};

		return outValue.getSize() * sizeof(CString::TType);
	}

	virtual size_t write(const CString& inValue) override {
		const CString str = this->isEmpty() ? inValue : getLineEndingString(this->lineEndings) + inValue;
		const String::UTF16& line = str.toUTF16<String::Endian::LITTLE>();
		const size_t res = fwrite(line.data(), sizeof(String::UTF16::value_type), line.size(), this->mFile);
		Error::Assert{this->mFile};
		return res;
	}
};

template <File::OpenType TOpenType, File::LineEnding TLineEnding>
class CFileArchive<TOpenType, File::Format::UTF16BE, TLineEnding> : public CBaseFileArchive<TOpenType>, public CSArchive {

public:

	explicit CFileArchive(const CPathArchive& inFilePath):
	CBaseFileArchive<TOpenType>(inFilePath) {
		this->init(TVector<unsigned char>{ 0xFE, 0xFF }, getLineEndingFromFile());
	}

	[[nodiscard]] virtual size_t getLines() const override {
		if constexpr (!this->isRead()) {
			throw Error::File::InvalidOperation("getLines() requires file read!");
		} else {
			String::UTF16 str;
			size_t lines = 0; // Initial Line

			const size_t loc = this->tell();
			this->seekFromStart(0);

			while (this->template readUntilLE<String::Endian::BIG>(str) != EOF) {
				lines++;
			}

			Error::Assert{this->mFile};

			this->seekFromStart(loc);

			return lines;
		}
	}

protected:

	static const char16_t* getLineEndingString(const File::LineEnding& inLineEnding) noexcept {
		switch (inLineEnding) {
		case File::LineEnding::CRLF:
			return u"\r\n";
		case File::LineEnding::LF:
		default:
			return u"\n";
		}
	}

	File::LineEnding getLineEndingFromFile() const {
		// Is being overwritten, we don't care about previous file's line endings
		if (this->isWrite())
			return TLineEnding;

		// Assume platform specific line endings in case there are none in the file
		const long loc = this->tell();
		this->seekFromStart(0);

		// Read the first line ending and assume other line endings are like that
		String::UTF16 str;
		while (this->template readUntil<String::Endian::BIG>(str, u'\n') != EOF) {
			Error::Assert{this->mFile};
			if (str.back() == u'\r') {
				return File::LineEnding::CRLF;
			}
			return File::LineEnding::LF;
		}

		Error::Assert{this->mFile};
		this->seekFromStart(loc);

		// Couldn't find a line ending
		return TLineEnding;
	}

	virtual size_t read(CString& outValue) override {
		String::UTF16 str;
		this->template readUntilLE<String::Endian::BIG>(str);

		Error::Assert{this->mFile};

		outValue = CString{str};

		return outValue.getSize() * sizeof(CString::TType);
	}

	virtual size_t write(const CString& inValue) override {
		const CString str = this->isEmpty() ? inValue : getLineEndingString(this->lineEndings) + inValue;
		const String::UTF16& line = str.toUTF16<String::Endian::BIG>();
		const size_t res = fwrite(line.data(), sizeof(String::UTF16::value_type), line.size(), this->mFile);
		Error::Assert{this->mFile};
		return res;
	}
};

#if USING_CRLF
template <File::OpenType TOpenType, File::LineEnding TLineEnding>
#else
template <File::OpenType TOpenType, File::LineEnding TLineEnding>
#endif
class CFileArchive<TOpenType, File::Format::UTF32LE, TLineEnding> : public CBaseFileArchive<TOpenType>, public CSArchive {

public:

	explicit CFileArchive(const CPathArchive& inFilePath):
	CBaseFileArchive<TOpenType>(inFilePath) {
		this->init(TVector<unsigned char>{ 0xFF, 0xFE, 0x00, 0x00 }, getLineEndingFromFile());
	}

	[[nodiscard]] virtual size_t getLines() const override {
		if constexpr (!this->isRead()) {
			throw Error::File::InvalidOperation("getLines() requires file read!");
		} else {
			String::UTF32 str;
			size_t lines = 0; // Initial Line

			const size_t loc = this->tell();
			this->seekFromStart(0);

			while (this->template readUntilLE<String::Endian::LITTLE>(str) != EOF) {
				lines++;
			}

			Error::Assert{this->mFile};

			this->seekFromStart(loc);

			return lines;
		}
	}

protected:

	static const char32_t* getLineEndingString(const File::LineEnding& inLineEnding) noexcept {
		switch (inLineEnding) {
		case File::LineEnding::CRLF:
			return U"\r\n";
		case File::LineEnding::LF:
		default:
			return U"\n";
		}
	}

	File::LineEnding getLineEndingFromFile() const {
		// Is being overwritten, we don't care about previous file's line endings
		if (this->isWrite())
			return TLineEnding;

		// Assume platform specific line endings in case there are none in the file
		const long loc = this->tell();
		this->seekFromStart(0);

		// Read the first line ending and assume other line endings are like that
		String::UTF32 str;
		while (this->template readUntil<String::Endian::LITTLE>(str, U'\n') != EOF) {
			Error::Assert{this->mFile};
			this->seekFromStart(loc);
			if (str.back() == U'\r') {
				return File::LineEnding::CRLF;
			}
			return File::LineEnding::LF;
		}

		Error::Assert{this->mFile};
		this->seekFromStart(loc);

		// Couldn't find a line ending
		return TLineEnding;
	}

	virtual size_t read(CString& outValue) override {
		String::UTF32 str;
		this->template readUntilLE<String::Endian::LITTLE>(str);

		Error::Assert{this->mFile};

		outValue = CString{str};

		return outValue.getSize() * sizeof(CString::TType);
	}

	virtual size_t write(const CString& inValue) override {
		const CString str = this->isEmpty() ? inValue : getLineEndingString(this->lineEndings) + inValue;
		const String::UTF32& line = str.toUTF32<String::Endian::LITTLE>();
		const size_t res = fwrite(line.data(), sizeof(String::UTF32::value_type), line.size(), this->mFile);
		Error::Assert{this->mFile};
		return res;
	}
};

#if USING_CRLF
template <File::OpenType TOpenType, File::LineEnding TLineEnding>
#else
template <File::OpenType TOpenType, File::LineEnding TLineEnding>
#endif
class CFileArchive<TOpenType, File::Format::UTF32BE, TLineEnding> : public CBaseFileArchive<TOpenType>, public CSArchive {

public:

	explicit CFileArchive(const CPathArchive& inFilePath):
	CBaseFileArchive<TOpenType>(inFilePath) {
		this->init(TVector<unsigned char>{ 0x00, 0x00, 0xFE, 0xFF }, getLineEndingFromFile());
	}

	[[nodiscard]] virtual size_t getLines() const override {
		if constexpr (!this->isRead()) {
			throw Error::File::InvalidOperation("getLines() requires file read!");
		} else {
			String::UTF32 str;
			size_t lines = 0; // Initial Line

			const size_t loc = this->tell();
			this->seekFromStart(0);

			while (this->template readUntilLE<String::Endian::BIG>(str) != EOF) {
				lines++;
			}

			Error::Assert{this->mFile};

			this->seekFromStart(loc);

			return lines;
		}
	}

protected:

	static const char32_t* getLineEndingString(const File::LineEnding& inLineEnding) noexcept {
		switch (inLineEnding) {
		case File::LineEnding::CRLF:
			return U"\r\n";
		case File::LineEnding::LF:
		default:
			return U"\n";
		}
	}

	File::LineEnding getLineEndingFromFile() const {
		// Is being overwritten, we don't care about previous file's line endings
		if (this->isWrite())
			return TLineEnding;

		// Assume platform specific line endings in case there are none in the file
		const long loc = this->tell();
		this->seekFromStart(0);

		// Read the first line ending and assume other line endings are like that
		String::UTF32 str;
		while (this->template readUntil<String::Endian::BIG>(str, U'\n') != EOF) {
			Error::Assert{this->mFile};
			this->seekFromStart(loc);
			if (str.back() == U'\r') {
				return File::LineEnding::CRLF;
			}
			return File::LineEnding::LF;
		}

		Error::Assert{this->mFile};
		this->seekFromStart(loc);

		// Couldn't find a line ending
		return TLineEnding;
	}

	virtual size_t read(CString& outValue) override {
		String::UTF32 str;
		this->template readUntilLE<String::Endian::BIG>(str);

		Error::Assert{this->mFile};

		outValue = CString{str};

		return outValue.getSize() * sizeof(CString::TType);
	}

	virtual size_t write(const CString& inValue) override {
		const CString str = this->isEmpty() ? inValue : getLineEndingString(this->lineEndings) + inValue;
		const String::UTF32& line = str.toUTF32<String::Endian::BIG>();
		const size_t res = fwrite(line.data(), sizeof(String::UTF32::value_type), line.size(), this->mFile);
		Error::Assert{this->mFile};
		return res;
	}
};