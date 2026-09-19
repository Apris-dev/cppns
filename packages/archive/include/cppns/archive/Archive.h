#pragma once

#include <charconv>
#include <string>

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

	template <typename TType>
	requires std::is_arithmetic_v<TType>
	friend CBaseStringArchive& operator>>(CBaseStringArchive& inArchive, TType& inValue) {
		std::string str = inArchive.get();
		const std::from_chars_result result = std::from_chars(str.data(), str.data() + str.size(), inValue);
		if (result.ec == std::errc()) {
			const size_t loc = result.ptr - str.data();
			inArchive.read(loc);
		}
		return inArchive;
	}

	template <typename TType>
	requires std::is_arithmetic_v<TType>
	friend CBaseStringArchive& operator<<(CBaseStringArchive& inArchive, const TType& inValue) {
		inArchive << std::to_string(inValue);
		return inArchive;
	}

	template <typename TType>
	requires std::is_enum_v<TType>
	friend CBaseStringArchive& operator>>(CBaseStringArchive& inArchive, TType& inEnum) {
		using EnumType = std::underlying_type_t<TType>;
		EnumType value;
		inArchive >> value;
		inEnum = static_cast<TType>(value);
		return inArchive;
	}

	template <typename TType>
	requires std::is_enum_v<TType>
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
		inArchive.read(&inValue, sizeof(TType));
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
		inValue.clear();
		std::string::value_type c;
		while (inArchive.read(&c, sizeof(c)) != 0 && c != '\0') {
			inValue += c;
		}
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
		inArchive.write(&inValue, sizeof(TType));
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

	friend COArchive& operator<<(COArchive& inArchive, const std::string& inValue) {
		inArchive.write(inValue.data(), sizeof(std::string::value_type), inValue.size());
		constexpr static char terminator = '\0';
		inArchive.write(&terminator, sizeof(terminator));
		return inArchive;
	}
};

class CArchive : public CIArchive, public COArchive {};