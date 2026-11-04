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
		UTF8,
		UTF16LE,
		UTF16BE,
		UTF32LE,
		UTF32BE
	};

	enum class LineEnding : uint8 {
		LF,
		CRLF,
#if USING_CRLF
		NATIVE = CRLF
#else
		NATIVE = LF
#endif
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

		if (!mFile)
			throw Error::File::Open(filePath.get().array());
	}

	virtual ~CBaseFileArchive() {
		if (!mFile) return;
		fclose(mFile);
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

	template <typename TStringType, String::Endian Endian = String::Endian::NATIVE>
	[[nodiscard]] size_t getLines_Impl() const {
		TStringType str;
		size_t lines = 0; // Initial Line

		const size_t loc = tell();
		seekFromStart(0);

		while (readUntilLE<Endian>(str) != EOF) {
			lines++;
		}

		Error::Assert{mFile};

		seekFromStart(loc);

		return lines;
	}

	template <typename TStringType, String::Endian Endian = String::Endian::NATIVE>
	File::LineEnding getLineEndingFromFile_Impl() const {
		// Read the first line ending and assume other line endings are like that
		TStringType str;
		using CharT = TStringType::value_type;
		while (readUntil<Endian>(str, static_cast<CharT>('\n')) != EOF) {
			if (str.back() == static_cast<CharT>('\r')) {
				return  File::LineEnding::CRLF;
			}
			return  File::LineEnding::LF;
		}

		// Couldn't find a line ending, assume native
		return File::LineEnding::NATIVE;
	}

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
		fseek(mFile, inOffset, SEEK_CUR);
		Error::Assert{mFile};
	}

	void seekFromStart(long inOffset) const {
		inOffset += mBomOffset;
		fseek(mFile, inOffset, SEEK_SET);
		Error::Assert{mFile};
	}

	void setStart() const {
		rewind(mFile);
		Error::Assert{mFile};
		if (mBomOffset > 0)
			seek(mBomOffset);
	}

	void seekFromEnd(const long inOffset) const {
		fseek(mFile, inOffset, SEEK_END);
		Error::Assert{mFile};
	}

	// Always use binary mode since they act the same on each platform
	const char* getOpenTypeMode(const File::OpenType& inOpenType) const {
		switch (inOpenType) {
		case File::OpenType::READ:
		case File::OpenType::BINARY_READ:
			return "rb";
		case File::OpenType::WRITE:
		case File::OpenType::BINARY_WRITE:
			return "wb";
		case File::OpenType::READWRITE:
		case File::OpenType::BINARY_READWRITE:
			return "wb+";
		default:
			throw Error::File::InvalidOpenMode(filePath.get().array());
		}
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
template <File::OpenType TOpenType, File::LineEnding TLineEnding = File::LineEnding::CRLF>
#else
template <File::OpenType TOpenType, File::LineEnding TLineEnding = File::LineEnding::LF>
#endif
class CBinaryFileArchive : public CBaseFileArchive<TOpenType>, public CBinaryArchive {

public:

	using Super = CBaseFileArchive<TOpenType>;
	using Super::Super;

	explicit CBinaryFileArchive(const CPathArchive& inFilePath):
	CBaseFileArchive<TOpenType>(inFilePath) {
		Super::init(TVector<unsigned char>{}, getLineEndingFromFile());
	}

	[[nodiscard]] virtual size_t getLines() const override {
		if constexpr (!Super::isRead()) {
			throw Error::File::InvalidOperation("getLines() requires file read!");
		} else {
			return Super::template getLines_Impl<String::Generic>();
		}
	}

protected:

	File::LineEnding getLineEndingFromFile() const {
		// Is being overwritten, we don't care about previous file's line endings
		if (Super::isWrite())
			return TLineEnding;

		// Assume platform specific line endings in case there are none in the file
		const long loc = Super::tell();
		Super::seekFromStart(0);

		const File::LineEnding res = Super::template getLineEndingFromFile_Impl<String::Generic>();

		Error::Assert{Super::mFile};
		Super::seekFromStart(loc);

		return res;
	}

	virtual size_t write(const void* inValue, const size_t inElementSize, const size_t inCount) override {
		const size_t res = fwrite(inValue, inElementSize, inCount, Super::mFile);
		Error::Assert{Super::mFile};
		return res;
	}

	virtual size_t read(void* inValue, const size_t inElementSize, const size_t inCount) override {
		const size_t res = fread(inValue, inElementSize, inCount, Super::mFile);
		Error::Assert{Super::mFile};
		return res;
	}

};

#if USING_CRLF
template <File::OpenType TOpenType, File::LineEnding TLineEnding = File::LineEnding::CRLF, File::Format TFormat = File::Format::UTF8>
#else
template <File::OpenType TOpenType, File::LineEnding TLineEnding = File::LineEnding::LF, File::Format TFormat = File::Format::UTF8>
#endif
class CStringFileArchive : public CBaseFileArchive<TOpenType>, public CSArchive {

public:

	using Super = CBaseFileArchive<TOpenType>;
	using Super::Super;

	explicit CStringFileArchive(const CPathArchive& inFilePath):
	CBaseFileArchive<TOpenType>(inFilePath) {
		Super::init(getBOMFromFile(), getLineEndingFromFile());
	}

	[[nodiscard]] virtual size_t getLines() const override {
		if constexpr (!Super::isRead()) {
			throw Error::File::InvalidOperation("getLines() requires file read!");
		} else {

			switch (mFileFormat) {
			case File::Format::UTF16LE:
				return Super::template getLines_Impl<String::UTF16, String::Endian::LITTLE>();
			case File::Format::UTF16BE:
				return Super::template getLines_Impl<String::UTF16, String::Endian::BIG>();
			case File::Format::UTF32LE:
				return Super::template getLines_Impl<String::UTF32, String::Endian::LITTLE>();
			case File::Format::UTF32BE:
				return Super::template getLines_Impl<String::UTF32, String::Endian::BIG>();
			default:
				return Super::template getLines_Impl<String::Generic>();
			}
		}
	}

protected:

	File::LineEnding getLineEndingFromFile() const {
		// Is being overwritten, we don't care about previous file's line endings
		if (Super::isWrite())
			return TLineEnding;

		// Assume platform specific line endings in case there are none in the file
		const long loc = Super::tell();
		Super::seekFromStart(0);

		// Read the first line ending and assume other line endings are like that
		File::LineEnding res = TLineEnding;

		switch (mFileFormat) {
		case File::Format::UTF16LE:
			res = Super::template getLineEndingFromFile_Impl<String::UTF16, String::Endian::LITTLE>();
		case File::Format::UTF16BE:
			res = Super::template getLineEndingFromFile_Impl<String::UTF16, String::Endian::BIG>();
			break;
		case File::Format::UTF32LE:
			res = Super::template getLineEndingFromFile_Impl<String::UTF32, String::Endian::LITTLE>();
		case File::Format::UTF32BE:
			res = Super::template getLineEndingFromFile_Impl<String::UTF32, String::Endian::BIG>();
			break;
		default:
			res = Super::template getLineEndingFromFile_Impl<String::Generic>();
			break;
		}

		Error::Assert{Super::mFile};
		Super::seekFromStart(loc);

		return res;
	}

	TVector<unsigned char> getBOMFromFile() {
		// Only care about previous BOM if it won't be overridden
		if (!Super::isWrite()) {
			// Set file to start
			fseek(Super::mFile, 0, SEEK_SET);

			unsigned char b[4] = {0, 0, 0, 0};
			const size_t n = fread(b, 1, sizeof(b), Super::mFile);

			// Assume BINARY
			mFileFormat = File::Format::UTF8;

			// Check UTF32 first, since they are similar to UTF16's BOM
			if (n >= 4 && b[0] == 0xFF && b[1] == 0xFE && b[2] == 0x00 && b[3] == 0x00) {
				mFileFormat =  File::Format::UTF32LE;
			} else if (n >= 4 && b[0] == 0x00 && b[1] == 0x00 && b[2] == 0xFE && b[3] == 0xFF) {
				mFileFormat =  File::Format::UTF32BE;
			} else if (n >= 3 && b[0] == 0xEF && b[1] == 0xBB && b[2] == 0xBF) {
				mFileFormat =  File::Format::UTF8;
			} else if (n >= 2 && b[0] == 0xFF && b[1] == 0xFE) {
				mFileFormat =  File::Format::UTF16LE;
			} else if (n >= 2 && b[0] == 0xFE && b[1] == 0xFF) {
				mFileFormat =  File::Format::UTF16BE;
			}
		}

		switch (mFileFormat) {
		case File::Format::UTF8:
			return TVector<unsigned char>{ 0xEF, 0xBB, 0xBF };
		case File::Format::UTF16LE:
			return TVector<unsigned char>{ 0xFF, 0xFE };
		case File::Format::UTF16BE:
			return TVector<unsigned char>{ 0xFE, 0xFF };
		case File::Format::UTF32LE:
			return TVector<unsigned char>{ 0xFF, 0xFE, 0x00, 0x00 };
		case File::Format::UTF32BE:
			return TVector<unsigned char>{ 0x00, 0x00, 0xFE, 0xFF };
		default:
			return TVector<unsigned char>{};
		}
	}

	virtual size_t read(CString& outValue) override {
		switch (mFileFormat) {
		case File::Format::UTF16LE:
			return read_Impl<String::UTF16, String::Endian::LITTLE>(outValue);
		case File::Format::UTF16BE:
			return read_Impl<String::UTF16, String::Endian::BIG>(outValue);
		case File::Format::UTF32LE:
			return read_Impl<String::UTF32, String::Endian::LITTLE>(outValue);
		case File::Format::UTF32BE:
			return read_Impl<String::UTF32, String::Endian::BIG>(outValue);
		default:
			return read_Impl<String::Generic>(outValue);
		}
	}

	virtual size_t write(const CString& inValue) override {
		const CString str = Super::isEmpty() ? inValue : File::getLineEndingString(Super::lineEndings) + inValue;

		switch (mFileFormat) {
		case File::Format::UTF16LE:
			return write_Impl<String::UTF16>(str.toUTF16<String::Endian::LITTLE>());
		case File::Format::UTF16BE:
			return write_Impl<String::UTF16>(str.toUTF16<String::Endian::BIG>());
		case File::Format::UTF32LE:
			return write_Impl<String::UTF32>(str.toUTF32<String::Endian::LITTLE>());
		case File::Format::UTF32BE:
			return write_Impl<String::UTF32>(str.toUTF32<String::Endian::BIG>());
		default:
			return write_Impl<String::Generic>(str);
		}
	}

	template <typename TStringType, String::Endian Endian = String::Endian::NATIVE>
	size_t read_Impl(CString& outValue) {
		TStringType str;
		Super::template readUntilLE<Endian>(str);

		Error::Assert{Super::mFile};

		outValue = CString{str};

		return outValue.getSize() * sizeof(CString::TType);
	}

	template <typename TStringType>
	size_t write_Impl(const TStringType& line) {
		const size_t res = fwrite(line.data(), sizeof(typename TStringType::value_type), line.size(), Super::mFile);
		Error::Assert{Super::mFile};
		return res;
	}

	File::Format mFileFormat = TFormat;
	String::Endian mEndian = String::Endian::NATIVE;

};

#if USING_CRLF
template <File::OpenType TOpenType, File::LineEnding TLineEnding = File::LineEnding::CRLF, File::Format TFormat = File::Format::UTF8>
#else
template <File::OpenType TOpenType, File::LineEnding TLineEnding = File::LineEnding::LF, File::Format TFormat = File::Format::UTF8>
#endif
using CFileArchive = std::conditional_t<TOpenType & File::OpenType::BINARY, CBinaryFileArchive<TOpenType, TLineEnding>, CStringFileArchive<TOpenType, TLineEnding, TFormat>>;