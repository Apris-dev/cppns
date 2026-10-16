#pragma once

#include <charconv>
#include <codecvt>

//TODO: circular include
#include "cppns/string/String.h"
#include "cppns/util/Pair.h"

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

	virtual size_t read(CString& inValue) = 0;

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

	friend CIArchive& operator>>(CIArchive& inArchive, CString& inValue) {
		inArchive.read(inValue);
		return inArchive;
	}

	template <typename TContainerType>
	friend CIArchive& operator>>(CIArchive& inArchive, TSequenceContainer<TContainerType>& inValue) {
		using TType = TSequenceContainer<TContainerType>::TType;

		size_t size;
		inArchive >> size;
		inValue.resize(size, [&](size_t) {
			TType obj;
			inArchive >> obj;
			return obj;
		});
		return inArchive;
	}

	template <typename TContainerType>
	friend CIArchive& operator>>(CIArchive& inArchive, TAssociativeContainer<TContainerType>& inValue) {
		using TKeyType = TAssociativeContainer<TContainerType>::TKeyType;
		using TValueType = TAssociativeContainer<TContainerType>::TValueType;

		size_t size;
		inArchive >> size;
		inValue.resize(size, [&] {
			TPair<TKeyType, TValueType> pair;
			inArchive >> pair;
			return pair;
		});
		return inArchive;
	}

	template <typename TContainerType>
	friend CIArchive& operator>>(CIArchive& inArchive, TSelfAssociativeContainer<TContainerType>& inValue) {
		using TType = TSelfAssociativeContainer<TContainerType>::TType;

		size_t size;
		inArchive >> size;
		inValue.resize(size, [&] {
			TType obj;
			inArchive >> obj;
			return obj;
		});
		return inArchive;
	}

	template <typename TKeyType, typename TValueType>
	friend CIArchive& operator>>(CIArchive& inArchive, TPair<TKeyType, TValueType>& pair) {
		inArchive >> pair.first();
		inArchive >> pair.second();
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

	virtual size_t write(const char* inValue) { return write(CString(inValue)); }

	virtual size_t write(const CString& inValue) = 0;

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
		inArchive << CString(inValue);
		return inArchive;
	}
	
	friend COArchive& operator<<(COArchive& inArchive, const CString& inValue) {
		inArchive.write(inValue);
		return inArchive;
	}

	template <typename TContainerType>
	friend COArchive& operator<<(COArchive& inArchive, const TSequenceContainer<TContainerType>& inValue) {
		inArchive << inValue.getSize();
		for (const auto& obj : inValue) {
			inArchive << obj;
		}
		return inArchive;
	}

	template <typename TContainerType>
	friend COArchive& operator<<(COArchive& inArchive, const TAssociativeContainer<TContainerType>& inValue) {
		inArchive << inValue.getSize();
		for (const auto& pair : inValue) {
			inArchive << pair;
		}
		return inArchive;
	}

	template <typename TContainerType>
	friend COArchive& operator<<(COArchive& inArchive, const TSelfAssociativeContainer<TContainerType>& inValue) {
		inArchive << inValue.getSize();
		for (const auto& obj : inValue) {
			inArchive << obj;
		}
		return inArchive;
	}

	template <typename TKeyType, typename TValueType>
	friend COArchive& operator<<(COArchive& inArchive, const TPair<TKeyType, TValueType>& pair) {
		inArchive << pair.first();
		inArchive << pair.second();
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

	virtual size_t read(CString& inValue) final override {
		inValue.clear();
		CString::TType c;
		while (read(&c, sizeof(c)) != 0 && c != '\0') {
			inValue += c;
		}
		return inValue.getSize() * sizeof(c);
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

	virtual size_t write(const CString& inValue) final override {
		write(inValue.data(), sizeof(CString::TType), inValue.getSize());
		constexpr static char terminator = '\0';
		write(&terminator, sizeof(terminator));
		return inValue.getSize() * sizeof(CString::TType);
	}
};

class CBinaryArchive : public CBinaryIArchive, public CBinaryOArchive {};

class CSIArchive : public CIArchive {

protected:

	// Inherit string read but don't override
	using CIArchive::read;

#define MAKE_READ(x) \
	virtual size_t read(x& inValue) final override { \
		CString str; \
		const size_t loc = read(str); \
		std::from_chars(str.data(), str.data() + str.getSize(), inValue); \
		return loc; \
	}

	/*
	 * Integral Types
	 */

	virtual size_t read(bool& inValue) final override {
		CString str;
		const size_t loc = read(str);
		if (str == "true") {
			inValue = true;
		} else {
			inValue = false;
		}
		return loc;
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

};

class CSOArchive : public COArchive {

protected:

	// Inherit string write but don't override
	using COArchive::write;

	//TODO: string conversions
#define MAKE_WRITE(x) \
	virtual size_t write(const x& inValue) final override { \
		return write(CString(std::to_string(inValue))); \
	}

	/*
	 * Integral Types
	 */

	virtual size_t write(const bool& inValue) final override { \
		return write(inValue ? "true" : "false"); \
	}

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