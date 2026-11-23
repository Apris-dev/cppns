#pragma once

#include <cstring>
#include <sstream>
#include <string>
#include <format>

namespace Error {
	template <typename TType>
	concept Formattable = requires(std::formatter<std::remove_cvref_t<TType>, char> formatter, std::format_context& ctx, TType value) {
		formatter.format(value, ctx);
	};
}

#define CREATE_ERROR_TYPE(type, name, desc, base) \
	class type : public base { \
	public: \
		using BaseClass = base; \
		type() : BaseClass(name, desc) {} \
		template <typename... TArgs> \
		requires (Error::Formattable<TArgs> && ...) \
		type(TArgs&&... args) : BaseClass(name, std::format(desc, std::forward<TArgs>(args)...)) {} \
		/*explicit type(const std::string_view inMessage) : BaseClass(name, desc, inMessage) {} */ \
	}

namespace Error {
	class Runtime : public std::runtime_error {
	public:

		Runtime(const std::string_view inErrorName, const std::string_view inErrorDescription) : std::runtime_error(convertToMessage(inErrorName, inErrorDescription).c_str()) {}

	private:
		static std::string convertToMessage(const std::string_view inErrorName, const std::string_view inErrorDescription) {
			std::stringstream s;
			const int error = errno;
			s << "A " << inErrorName << " Occurred! " << inErrorDescription;
			if (error > 0) {
				s << " C++ Error: ";
#if USING_MSVC
				char buffer[256];
				strerror_s(buffer, sizeof(buffer), error);
				s << buffer;
#else
				s << std::strerror(error);
#endif
			}
			return s.str();
		}
	};
}

namespace Assert {
	static void True(const bool inAssertion, const std::string_view inMessage = "Undefined") {
		if (!inAssertion)
			throw Error::Runtime("Assertion Failure", std::format("Assertion failed. Reason: {}", inMessage));
	}

	static void False(const bool inAssertion, const std::string_view inMessage = "Undefined") {
		if (inAssertion)
			throw Error::Runtime("Assertion Failure", std::format("Assertion failed. Reason: {}", inMessage));
	}

	static void NoError(const int error, const std::string_view inErrorName = "Error") {
		if (error <= 0) return;
#if USING_MSVC
		char buffer[256];
		strerror_s(buffer, sizeof(buffer), error);
		const std::string msg = buffer;
#else
		const std::string msg = std::strerror(error);
#endif

		throw Error::Runtime(inErrorName, msg);
	}

	static void FileOK(FILE* inFile) {
		if (inFile == nullptr) return;
		if (const int error = ferror(inFile); error > 0) {
			NoError(error, "File Error");
		}
	}
}