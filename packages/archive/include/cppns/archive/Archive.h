#pragma once

#include <charconv>
#include <string>

#include "cppns/util/InitializerList.h"
#include "cppns/util/Pair.h"

//TODO: delimiter template?
class CBaseStringArchive {

protected:

	[[nodiscard]] virtual std::string get() const = 0;

	virtual largest write(const std::string& inValue) = 0;

	virtual largest read(std::string& outValue) = 0;

	virtual largest read(size_t amount) = 0;

public:

	virtual ~CBaseStringArchive() = default;

	friend CBaseStringArchive& operator>>(CBaseStringArchive& inArchive, std::string& inValue) {
		inArchive.read(inValue);
		return inArchive;
	}

	friend CBaseStringArchive& operator<<(CBaseStringArchive& inArchive, const std::string& inValue) {
		inArchive.write(inValue);
		return inArchive;
	}

	template <typename TType
	REQUIRES(std::is_arithmetic_v<TType>)
	friend CBaseStringArchive& operator>>(CBaseStringArchive& inArchive, TType& inValue) {
		std::string str = inArchive.get();
		const std::from_chars_result result = std::from_chars(str.data(), str.data() + str.size(), inValue);
		if (result.ec == std::errc()) {
			const size_t loc = result.ptr - str.data();
			inArchive.read(loc);
		}
		return inArchive;
	}

	template <typename TType
	REQUIRES(std::is_arithmetic_v<TType>)
	friend CBaseStringArchive& operator<<(CBaseStringArchive& inArchive, const TType& inValue) {
		inArchive << std::to_string(inValue);
		return inArchive;
	}

	template <typename TType
	REQUIRES(std::is_enum_v<TType>)
	friend CBaseStringArchive& operator>>(CBaseStringArchive& inArchive, TType& inEnum) {
		using EnumType = std::underlying_type_t<TType>;
		EnumType value;
		inArchive >> value;
		inEnum = static_cast<TType>(value);
		return inArchive;
	}

	template <typename TType
	REQUIRES(std::is_enum_v<TType>)
	friend CBaseStringArchive& operator<<(CBaseStringArchive& inArchive, const TType& inEnum) {
		using EnumType = std::underlying_type_t<TType>;
		inArchive << static_cast<EnumType>(inEnum);
		return inArchive;
	}

	template <typename TType>
	friend CBaseStringArchive& operator>>(CBaseStringArchive& inArchive, TType*& ptr) {
		size_t value;
		inArchive >> value;
		ptr = reinterpret_cast<TType*>(value);
		return inArchive;
	}

	template <typename TType>
	friend CBaseStringArchive& operator<<(CBaseStringArchive& inArchive, const TType*& ptr) {
		inArchive << reinterpret_cast<size_t>(ptr);
		return inArchive;
	}

	template <typename TKeyType, typename TValueType>
	friend CBaseStringArchive& operator<<(CBaseStringArchive& inArchive, const TPair<TKeyType, TValueType>& pair) {
		inArchive << pair.first();
		inArchive << pair.second();
		return inArchive;
	}

	// Initializer lists cannot be written to, but can be read from
	template <typename TType>
	friend CBaseStringArchive& operator<<(CBaseStringArchive& inArchive, const TInitializerList<TType>& list) {
		for (const auto& obj : list)
			inArchive << obj;
		return inArchive;
	}

protected:

};

class CStringArchive : public CBaseStringArchive {

public:

	[[nodiscard]] virtual std::string get() const override { return str; }

protected:

	virtual size_t write(const std::string& inValue) override {
		str += inValue;
		return 0;
	}

	virtual size_t read(std::string& outValue)  override {
		const auto loc = str.find(LINE_ENDING);
		outValue = str.substr(0, loc);
		return read(loc);
	}

	size_t read(const size_t amount) override {
		str.erase(0, amount);
		return 0;
	}

	std::string str;

};

class CInputArchive {

protected:

	virtual size_t read(void* inValue, const size_t inElementSize) {
		return read(inValue, inElementSize, 1);
	}

	virtual size_t read(void* inValue, size_t inElementSize, size_t inCount) = 0;

	std::string readUntil(const char terminator) {
		std::string res;
		char c;
		while (true) {
			const size_t n = read(&c, sizeof(char));
			if (n == 0 || c == terminator)
				break;
			res += c;
		}
		return res;
	}

public:

	virtual ~CInputArchive() = default;

	template <typename TType
	REQUIRES(std::is_arithmetic_v<TType>)
	friend CInputArchive& operator>>(CInputArchive& inArchive, TType& inValue) {
		inArchive.read(&inValue, sizeof(TType));
		return inArchive;
	}

	template <typename TType
	REQUIRES(std::is_enum_v<TType>)
	friend CInputArchive& operator>>(CInputArchive& inArchive, TType& inEnum) {
		using EnumType = std::underlying_type_t<TType>;
		EnumType value;
		inArchive >> value;
		inEnum = static_cast<TType>(value);
		return inArchive;
	}

	template <typename TType>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TType*& ptr) {
		size_t value;
		inArchive >> value;
		ptr = reinterpret_cast<TType*>(value);
		return inArchive;
	}

	friend CInputArchive& operator>>(CInputArchive& inArchive, std::string& inValue) {
		inValue = inArchive.readUntil('\0');
		return inArchive;
	}

	template <typename TKeyType, typename TValueType>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TPair<TKeyType, TValueType>& pair) {
		inArchive >> pair.first();
		inArchive >> pair.second();
		return inArchive;
	}
};

class COutputArchive {

protected:

	virtual size_t write(const void* inValue, const size_t inElementSize) {
		return write(inValue, inElementSize, 1);
	}

	virtual size_t write(const void* inValue, size_t inElementSize, size_t inCount) = 0;

public:

	virtual ~COutputArchive() = default;

	template <typename TType
	REQUIRES(std::is_arithmetic_v<TType>)
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TType& inValue) {
		inArchive.write(&inValue, sizeof(TType));
		return inArchive;
	}

	template <typename TType
	REQUIRES(std::is_enum_v<TType>)
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TType& inEnum) {
		using EnumType = std::underlying_type_t<TType>;
		inArchive << static_cast<EnumType>(inEnum);
		return inArchive;
	}

	template <typename TType>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TType*& ptr) {
		inArchive << reinterpret_cast<size_t>(ptr);
		return inArchive;
	}

	friend COutputArchive& operator<<(COutputArchive& inArchive, const std::string& inValue) {
		inArchive.write(inValue.data(), sizeof(std::string::value_type), inValue.size());
		constexpr static char terminator = '\0';
		inArchive.write(&terminator, sizeof(terminator));
		return inArchive;
	}

	template <typename TKeyType, typename TValueType>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TPair<TKeyType, TValueType>& pair) {
		inArchive << pair.first();
		inArchive << pair.second();
		return inArchive;
	}

	// Initializer lists cannot be written to, but can be read from
	template <typename TType>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TInitializerList<TType>& list) {
		inArchive << list.size();
		for (const auto& obj : list)
			inArchive << obj;
		return inArchive;
	}
};

class CArchive : public CInputArchive, public COutputArchive {};