#pragma once

#include <codecvt>
#include "cppns/util/fast_float.h"

#include "cppns/string/String.h"
#include "cppns/util/Pair.h"

class CIArchive {

protected:

	virtual size_t read(void* inValue, const size_t inElementSize) {
		return read(inValue, inElementSize, 1);
	}

	virtual size_t read(void* inValue, size_t inElementSize, size_t inCount) = 0;

public:

	virtual ~CIArchive() = default;

	template <typename TType>
	requires std::is_arithmetic_v<TType>
	friend CIArchive& operator>>(CIArchive& inArchive, TType& inValue) {
		inArchive.read(&inValue, sizeof(inValue));
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
		inValue.clear();
		CString::TType c;
		while (inArchive.read(&c, sizeof(c)) != 0 && c != '\0') {
			inValue += c;
		}
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

	virtual size_t write(const void* inValue, const size_t inElementSize) {
		return write(inValue, inElementSize, 1);
	}

	virtual size_t write(const void* inValue, size_t inElementSize, size_t inCount) = 0;

public:

	virtual ~COArchive() = default;

	template <typename TType>
	requires std::is_arithmetic_v<TType>
	friend COArchive& operator<<(COArchive& inArchive, const TType& inValue) {
		inArchive.write(&inValue, sizeof(inValue));
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
		inArchive.write(inValue.data(), sizeof(CString::TType), inValue.getSize());
		constexpr static char terminator = '\0';
		inArchive.write(&terminator, sizeof(terminator));
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

class CArchive : public CIArchive, public COArchive {};

class CSIArchive {

public:

	virtual ~CSIArchive() = default;

	template <typename TType>
	requires Convert::is_convertible_v<TType, CString>
	friend CSIArchive& operator>>(CSIArchive& inArchive, TType& v) {
		CString str;
		inArchive.read(str);
		v = Convert::to<TType>(str);
		return inArchive;
	}

protected:

	virtual void read(CString& inValue) = 0;

};

class CSOArchive {

public:

	virtual ~CSOArchive() = default;

	template <typename TType>
	requires Convert::is_convertible_v<CString, TType>
	friend CSOArchive& operator<<(CSOArchive& inArchive, const TType& v) {
		inArchive.write(Convert::to<CString>(v));
		return inArchive;
	}

protected:

	virtual void write(const CString& inValue) = 0;

};

class CSArchive : public CSIArchive, public CSOArchive {};