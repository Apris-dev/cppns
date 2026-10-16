#pragma once

#include <algorithm>

#include "Archive.h"

class CPathArchive : public CSArchive {

public:

    CPathArchive() = default;

    explicit CPathArchive(const CString& initialPath) {
        CPathArchive::write(initialPath);
    }

    [[nodiscard]] std::string get() const {
        return str;
    }

    [[nodiscard]] bool exists() const {
        return std::filesystem::exists((std::string)str);
    }

    [[nodiscard]] std::string getFilename() const {
        const auto loc = str.findLast(PATH_SEPARATOR);
        if (loc == std::string::npos) {
            return "";
        }
        return str.substr(loc + 1, str.getSize() - loc - 1);
    }

    void previous() {
        const auto loc = str.findLast(PATH_SEPARATOR);
        if (loc == std::string::npos) {
            return;
        }
        str.eraseAfter(loc + 1);
    }
protected:

    virtual size_t read(CString& outValue) override {
        const auto loc = str.findLast(PATH_SEPARATOR);
        if (loc == std::string::npos) {
            return 0;
        }
        outValue = str.substr(loc + 1, str.getSize() - loc - 2);
        str.eraseAfter(loc + 1);

        return 0;
    }

    virtual size_t write(const CString& inValue) override {
        if (inValue.isEmpty()) return 0;

        str += inValue;
        // Force Replacement of proper path separator
#if PATH_SEPARATOR == '\\'
        //TODO: string replaceall
        std::replace(str.begin().itr, str.end().itr, '/', '\\');
#elif PATH_SEPARATOR == '/'
        std::replace(str.begin().itr, str.end().itr, '\\', '/');
#endif
        if (str.find('.') == std::string::npos && str.bottom() != PATH_SEPARATOR) {
            str += PATH_SEPARATOR;
        }
        return 0;
    }

private:

    CString str;

};