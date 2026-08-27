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

namespace Error {
	class Runtime : std::exception {
	public:

		virtual ~Runtime() = default;

		Runtime() = delete;

		explicit Runtime(const std::string_view inErrorName, const std::string_view inErrorDescription, const std::string_view inError) : std::exception(convertToMessage(inErrorName, inErrorDescription, inError).c_str()) {
			std::cerr << exception::what() << "\n";
		}

		explicit Runtime(const std::string_view inErrorName, const std::string_view inErrorDescription) : Runtime(inErrorName, inErrorDescription, "Unknown Error") {}

	private:
		static std::string convertToMessage(const std::string_view inErrorName, const std::string_view inErrorDescription, const std::string_view inError) {
			std::stringstream s;
			s << "A " << inErrorName << " Occurred! " << inErrorDescription << " Error: " << inError << "\n";
			return s.str();
		}
	};
}
