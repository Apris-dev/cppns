#pragma once

#include <charconv>
#include <string>

class CIArchive {

protected:

#define MAKE_READ(x) \
	virtual size_t read(x& inValue) = 0;

	/*
	 * Integral Types
	 */

	MAKE_READ(bool)
	MAKE_READ(char)

	MAKE_READ(unsigned char)
	MAKE_READ(signed char)

	MAKE_READ(unsigned short)
	MAKE_READ(signed short)

	MAKE_READ(unsigned int)
	MAKE_READ(signed int)

	MAKE_READ(unsigned long)
	MAKE_READ(signed long)

	MAKE_READ(unsigned long long)
	MAKE_READ(signed long long)

	/*
	 * Floating Point Types
	 */

	MAKE_READ(float)
	MAKE_READ(double)
	MAKE_READ(long double)
#undef MAKE_READ

	/*
	 * Strings
	 */

	virtual size_t read(std::string& inValue) = 0;
	virtual size_t read(std::wstring& inValue) = 0;

public:

	virtual ~CIArchive() = default;

	template <typename TType>
	requires std::is_arithmetic_v<TType>
	friend CIArchive& operator>>(CIArchive& inArchive, TType& inValue) {
		inArchive.read(inValue);
		return inArchive;
	}

	template <typename TType>
	requires std::is_enum_v<TType>
	friend CIArchive& operator>>(CIArchive& inArchive, TType& inEnum) {
		using EnumType = std::underlying_type_t<TType>;
		EnumType value;
		inArchive >> value;
		inEnum = static_cast<TType>(value);
		return inArchive;
	}

	template <typename TType>
	friend CIArchive& operator>>(CIArchive& inArchive, TType*& ptr) {
		size_t value;
		inArchive >> value;
		ptr = reinterpret_cast<TType*>(value);
		return inArchive;
	}

	friend CIArchive& operator>>(CIArchive& inArchive, std::string& inValue) {
		inArchive.read(inValue);
		return inArchive;
	}

	friend CIArchive& operator>>(CIArchive& inArchive, std::wstring& inValue) {
		inArchive.read(inValue);
		return inArchive;
	}
};

class COArchive {

protected:

#define MAKE_WRITE(x) \
	virtual size_t write(const x& inValue) = 0;

	/*
	 * Integral Types
	 */

	MAKE_WRITE(bool)
	MAKE_WRITE(char)

	MAKE_WRITE(unsigned char)
	MAKE_WRITE(signed char)

	MAKE_WRITE(unsigned short)
	MAKE_WRITE(signed short)

	MAKE_WRITE(unsigned int)
	MAKE_WRITE(signed int)

	MAKE_WRITE(unsigned long)
	MAKE_WRITE(signed long)

	MAKE_WRITE(unsigned long long)
	MAKE_WRITE(signed long long)

	/*
	 * Floating Point Types
	 */

	MAKE_WRITE(float)
	MAKE_WRITE(double)
	MAKE_WRITE(long double)
#undef MAKE_WRITE

	/*
	 * Strings
	 */

	virtual size_t write(const char* inValue) { return write(std::string(inValue)); }
	virtual size_t write(const wchar_t* inValue) { return write(std::wstring(inValue)); }

	virtual size_t write(const std::string& inValue) = 0;
	virtual size_t write(const std::wstring& inValue) = 0;

public:

	virtual ~COArchive() = default;

	template <typename TType>
	requires std::is_arithmetic_v<TType>
	friend COArchive& operator<<(COArchive& inArchive, const TType& inValue) {
		inArchive.write(inValue);
		return inArchive;
	}

	template <typename TType>
	requires std::is_enum_v<TType>
	friend COArchive& operator<<(COArchive& inArchive, const TType& inEnum) {
		using EnumType = std::underlying_type_t<TType>;
		inArchive << static_cast<EnumType>(inEnum);
		return inArchive;
	}

	template <typename TType>
	friend COArchive& operator<<(COArchive& inArchive, const TType* ptr) {
		inArchive << reinterpret_cast<size_t>(ptr);
		return inArchive;
	}

	friend COArchive& operator<<(COArchive& inArchive, const char* inValue) {
		inArchive << std::string(inValue);
		return inArchive;
	}

	friend COArchive& operator<<(COArchive& inArchive, const std::string& inValue) {
		inArchive.write(inValue);
		return inArchive;
	}

	friend COArchive& operator<<(COArchive& inArchive, const std::wstring& inValue) {
		inArchive.write(inValue);
		return inArchive;
	}
};

class CBinaryIArchive : public CIArchive {

protected:

	virtual size_t read(void* inValue, const size_t inElementSize) {
		return read(inValue, inElementSize, 1);
	}

	virtual size_t read(void* inValue, size_t inElementSize, size_t inCount) = 0;

#define MAKE_READ(x) \
	virtual size_t read(x& inValue) final override { return read(&inValue, sizeof(inValue)); }

	/*
	 * Integral Types
	 */

	MAKE_READ(bool)
	MAKE_READ(char)

	MAKE_READ(unsigned char)
	MAKE_READ(signed char)

	MAKE_READ(unsigned short)
	MAKE_READ(signed short)

	MAKE_READ(unsigned int)
	MAKE_READ(signed int)

	MAKE_READ(unsigned long)
	MAKE_READ(signed long)

	MAKE_READ(unsigned long long)
	MAKE_READ(signed long long)

	/*
	 * Floating Point Types
	 */

	MAKE_READ(float)
	MAKE_READ(double)
	MAKE_READ(long double)
#undef MAKE_READ

	/*
	 * Strings
	 */

	virtual size_t read(std::string& inValue) final override {
		inValue.clear();
		std::string::value_type c;
		while (read(&c, sizeof(c)) != 0 && c != '\0') {
			inValue += c;
		}
		return inValue.size() * sizeof(c);
	}

	virtual size_t read(std::wstring& inValue) final override {
		inValue.clear();
		std::wstring::value_type c;
		while (read(&c, sizeof(c)) != 0 && c != L'\0') {
			inValue += c;
		}
		return inValue.size() * sizeof(c);
	}
};

class CBinaryOArchive : public COArchive {

protected:

	virtual size_t write(const void* inValue, const size_t inElementSize) {
		return write(inValue, inElementSize, 1);
	}

	virtual size_t write(const void* inValue, size_t inElementSize, size_t inCount) = 0;

#define MAKE_WRITE(x) \
	virtual size_t write(const x& inValue) final override { return write(&inValue, sizeof(inValue)); }

	/*
	 * Integral Types
	 */

	MAKE_WRITE(bool)
	MAKE_WRITE(char)

	MAKE_WRITE(unsigned char)
	MAKE_WRITE(signed char)

	MAKE_WRITE(unsigned short)
	MAKE_WRITE(signed short)

	MAKE_WRITE(unsigned int)
	MAKE_WRITE(signed int)

	MAKE_WRITE(unsigned long)
	MAKE_WRITE(signed long)

	MAKE_WRITE(unsigned long long)
	MAKE_WRITE(signed long long)

	/*
	 * Floating Point Types
	 */

	MAKE_WRITE(float)
	MAKE_WRITE(double)
	MAKE_WRITE(long double)
#undef MAKE_WRITE

	/*
	 * Strings
	 */

	virtual size_t write(const std::string& inValue) final override {
		write(inValue.data(), sizeof(std::string::value_type), inValue.size());
		constexpr static char terminator = '\0';
		write(&terminator, sizeof(terminator));
		return inValue.size() * sizeof(std::string::value_type);
	}

	virtual size_t write(const std::wstring& inValue) final override {
		write(inValue.data(), sizeof(std::wstring::value_type), inValue.size());
		constexpr static wchar_t terminator = L'\0';
		write(&terminator, sizeof(terminator));
		return inValue.size() * sizeof(std::wstring::value_type);
	}
};

class CBinaryArchive : public CBinaryIArchive, public CBinaryOArchive {};

class CSIArchive : public CIArchive {

protected:

	// Inherit string read but don't override
	using CIArchive::read;

#define MAKE_READ(x) \
	virtual size_t read(x& inValue) final override { \
		std::string str = get(); \
		const std::from_chars_result result = std::from_chars(str.data(), str.data() + str.size(), inValue); \
		if (result.ec == std::errc()) { \
			const size_t loc = result.ptr - str.data(); \
			read(loc); \
		} \
		return 0; \
	}

	/*
	 * Integral Types
	 */

	virtual size_t read(bool& inValue) final override { \
		return 0;
	}

	MAKE_READ(char)

	MAKE_READ(unsigned char)
	MAKE_READ(signed char)

	MAKE_READ(unsigned short)
	MAKE_READ(signed short)

	MAKE_READ(unsigned int)
	MAKE_READ(signed int)

	MAKE_READ(unsigned long)
	MAKE_READ(signed long)

	MAKE_READ(unsigned long long)
	MAKE_READ(signed long long)

	/*
	 * Floating Point Types
	 */

	MAKE_READ(float)
	MAKE_READ(double)
	MAKE_READ(long double)
#undef MAKE_READ

	[[nodiscard]] virtual std::string get() const = 0;

	virtual largest read(size_t amount) = 0;

};

class CSOArchive : public COArchive {

protected:

	// Inherit string write but don't override
	using COArchive::write;

#define MAKE_WRITE(x) \
	virtual size_t write(const x& inValue) final override { \
		return write(std::to_string(inValue)); \
	}

	/*
	 * Integral Types
	 */

	MAKE_WRITE(bool)
	MAKE_WRITE(char)

	MAKE_WRITE(unsigned char)
	MAKE_WRITE(signed char)

	MAKE_WRITE(unsigned short)
	MAKE_WRITE(signed short)

	MAKE_WRITE(unsigned int)
	MAKE_WRITE(signed int)

	MAKE_WRITE(unsigned long)
	MAKE_WRITE(signed long)

	MAKE_WRITE(unsigned long long)
	MAKE_WRITE(signed long long)

	/*
	 * Floating Point Types
	 */

	MAKE_WRITE(float)
	MAKE_WRITE(double)
	MAKE_WRITE(long double)
#undef MAKE_WRITE

};

class CSArchive : public CSIArchive, public CSOArchive {};
