#pragma once

#include <sstream>
#include <string>

#define CREATE_ERROR_TYPE(type, name, desc, base) \
	class type : public base { \
	public: \
		using BaseClass = base; \
		type() : BaseClass(name, desc) {} \
		explicit type(const std::string_view inMessage) : BaseClass(name, desc, inMessage) {} \
	}

#define throw_if(condition, error) \
	if (!(condition)) throw error

namespace Error {
	class Runtime : public std::exception {
	public:

		virtual ~Runtime() = default;

		Runtime(const std::string_view inErrorName, const std::string_view inErrorDescription, const std::string_view inError) : std::exception(convertToMessage(inErrorName, inErrorDescription, inError).c_str()) {
			std::cerr << exception::what() << "\n";
		}

		Runtime(const std::string_view inErrorName, const std::string_view inErrorDescription) : Runtime(inErrorName, inErrorDescription, getGenericMessage()) {}

	private:
		static std::string convertToMessage(const std::string_view inErrorName, const std::string_view inErrorDescription, const std::string_view inError) {
			std::stringstream s;
			s << "A " << inErrorName << " Occurred! " << inErrorDescription << " Error: " << inError;
			return s.str();
		}

		static std::string getGenericMessage() {
			const int error = errno;
			if (error <= 0)
				return "Unknown Error Occurred!";
#ifdef _MSC_VER
			char buffer[256];
			strerror_s(buffer, sizeof(buffer), error);
			return buffer;
#else
			return strerror(error);
#endif
		}
	};
}
