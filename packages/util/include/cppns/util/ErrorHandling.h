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

#ifdef assert
	#undef assert
#endif

// Redefine assert with our own custom logic
#define assert(cond, ...) \
	if (!(cond)) Error::Assert(#cond, ##__VA_ARGS__)

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

	class Assert {
	public:

		Assert() {
			const int error = errno;
			errno = 0;
			assertError("C++ Error", error);
		}
		explicit Assert(const std::string_view inAssertion) {
			throw Runtime("Assertion Failure", std::format("Assertion {} failed", inAssertion));
		}
		Assert(const std::string_view inAssertion, const std::string_view inAssertionDescription) {
			throw Runtime("Assertion Failure", std::format("Assertion {} failed. Reason: {}", inAssertion, inAssertionDescription));
		}
		explicit Assert(const int error) { assertError("Error", error); }
		explicit Assert(FILE* inFile) {
			if (const int error = ferror(inFile); error > 0) {
				assertError("File Error", error);
			}
		}

	private:
		static void assertError(std::string_view errorName, const int error) {
			if (error <= 0) return;
#if USING_MSVC
			char buffer[256];
			strerror_s(buffer, sizeof(buffer), error);
			std::string msg = buffer;
#else
			std::string msg = std::strerror(error);
#endif

			throw Runtime(errorName, msg);
		}
	};
}
