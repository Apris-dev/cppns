#pragma once

#include <algorithm>

#include "Archive.h"

class CPathArchive : public CBaseStringArchive {

public:

    CPathArchive() = default;

    explicit CPathArchive(const std::string& initialPath) {
        CPathArchive::write(initialPath);
    }

    [[nodiscard]] virtual std::string get() const override {
        return str;
    }

    [[nodiscard]] std::string getFilename() const {
        const auto loc = str.find_last_of(PATH_SEPARATOR, str.size() - 2);
        if (loc == std::string::npos) {
            return "";
        }
        return str.substr(loc + 1, str.size() - loc - 1);
    }

    void previous() {
        const auto loc = str.find_last_of(PATH_SEPARATOR, str.size() - 2);
        if (loc == std::string::npos) {
            return;
        }
        str.erase(loc + 1);
    }
protected:

    virtual size_t read(std::string& outValue) override {
        const auto loc = str.find_last_of(PATH_SEPARATOR, str.size() - 2);
        if (loc == std::string::npos) {
            return 0;
        }
        outValue = str.substr(loc + 1, str.size() - loc - 2);
        str.erase(loc + 1);

        return 0;
    }

    size_t read(const size_t amount) override {
	    str.erase(0, amount);
        return 0;
    }

    virtual size_t write(const std::string& inValue) override {
        if (inValue.empty()) return 0;

        str += inValue;
        // Force Replacement of proper path separator
#if PATH_SEPARATOR == '\\'
        std::replace(str.begin(), str.end(), '/', '\\');
#elif PATH_SEPARATOR == '/'
        std::replace(str.begin(), str.end(), '\\', '/');
#endif
        if (str.find('.') == std::string::npos && str.back() != PATH_SEPARATOR) {
            str += PATH_SEPARATOR;
        }
        return 0;
    }

private:

    std::string str;

};