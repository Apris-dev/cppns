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
		case File::OpenType::BINARY:
			return "b";
		case File::OpenType::READ:
			return "rb";
		case File::OpenType::WRITE:
			return "wb";
		case File::OpenType::READWRITE:
			return "wb+";
		}
		throw Error::File::InvalidOpenMode(filePath.get().array());
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

	// TODO: support Big Endian
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

			while (this->readUntilLE(str) != EOF) {
				lines++;
			}

			Error::Assert{this->mFile};

			this->seekFromStart(loc);

			return lines;
		}
	}

protected:

	File::LineEnding getLineEndingFromFile() const {
		// Is being overwritten, we don't care about previous file's line endings
		if (this->isWrite())
			return TLineEnding;

		// Assume platform specific line endings in case there are none in the file
		const long loc = this->tell();
		this->seekFromStart(0);

		// Read the first line ending and assume other line endings are like that
		String::Generic str;
		while (this->readUntil(str, '\n') != EOF) {
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

			while (this->readUntilLE(str) != EOF) {
				lines++;
			}

			Error::Assert{this->mFile};

			this->seekFromStart(loc);

			return lines;
		}
	}

protected:

	File::LineEnding getLineEndingFromFile() const {
		// Is being overwritten, we don't care about previous file's line endings
		if (this->isWrite())
			return TLineEnding;

		// Assume platform specific line endings in case there are none in the file
		const long loc = this->tell();
		this->seekFromStart(0);

		// Read the first line ending and assume other line endings are like that
		String::Generic str;
		while (this->readUntil(str, '\n') != EOF) {
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
		const CString line = this->isEmpty() ? inValue : File::getLineEndingString(this->lineEndings) + inValue;
		const size_t res = fwrite(line.data(), sizeof(CString::TType), line.getSize(), this->mFile);
		Error::Assert{this->mFile};
		return res;
	}
};

template <File::OpenType TOpenType, File::LineEnding TLineEnding>
class CFileArchive<TOpenType, File::Format::UTF16, TLineEnding> : public CBaseFileArchive<TOpenType>, public CSArchive {

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

			while (this->readUntilLE(str) != EOF) {
				lines++;
			}

			Error::Assert{this->mFile};

			this->seekFromStart(loc);

			return lines;
		}
	}

protected:

	File::LineEnding getLineEndingFromFile() const {
		// Is being overwritten, we don't care about previous file's line endings
		if (this->isWrite())
			return TLineEnding;

		// Assume platform specific line endings in case there are none in the file
		const long loc = this->tell();
		this->seekFromStart(0);

		// Read the first line ending and assume other line endings are like that
		String::UTF16 str;
		while (this->readUntil(str, u'\n') != EOF) {
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

#if USING_CRLF
template <File::OpenType TOpenType, File::LineEnding TLineEnding>
#else
template <File::OpenType TOpenType, File::LineEnding TLineEnding>
#endif
class CFileArchive<TOpenType, File::Format::UTF32, TLineEnding> : public CBaseFileArchive<TOpenType>, public CSArchive {

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

			while (this->readUntilLE(str) != EOF) {
				lines++;
			}

			Error::Assert{this->mFile};

			this->seekFromStart(loc);

			return lines;
		}
	}

protected:

	File::LineEnding getLineEndingFromFile() const {
		// Is being overwritten, we don't care about previous file's line endings
		if (this->isWrite())
			return TLineEnding;

		// Assume platform specific line endings in case there are none in the file
		const long loc = this->tell();
		this->seekFromStart(0);

		// Read the first line ending and assume other line endings are like that
		String::UTF32 str;
		while (this->readUntil(str, U'\n') != EOF) {
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