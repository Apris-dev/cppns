#pragma once

#include <algorithm>

#include "Archive.h"

class CPathArchive : public CSArchive {

public:

    CPathArchive() = default;

    CPathArchive(const CString& initialPath) {
        CPathArchive::write(initialPath);
    }

    [[nodiscard]] CString get() const {
        return str;
    }

    [[nodiscard]] CString top() const {
        CString temp = str;
        // Make sure we dont include the path separator at the end of the string
        if (temp.bottom() == PATH_SEPARATOR) {
            temp.pop();
        }
        const auto loc = temp.findLast(PATH_SEPARATOR);
        if (loc == CString::npos) {
            return "";
        }
        return temp.substr(loc + 1, temp.getSize() - loc - 1);
    }

    [[nodiscard]] bool exists() const {
        return std::filesystem::exists((std::string)str);
    }

    //TODO: only return if a file
    [[nodiscard]] CString getFilename() const {
        // If this is a path, return an empty string
        if (str.bottom() == PATH_SEPARATOR) {
            return "";
        }
        const auto loc = str.findLast(PATH_SEPARATOR);
        if (loc == CString::npos) {
            return "";
        }
        return str.substr(loc + 1, str.getSize() - loc - 1);
    }

    void previous() {
        CString temp = str;
        // Make sure we dont include the path separator at the end of the string
        if (temp.bottom() == PATH_SEPARATOR) {
            temp.pop();
        }
        const auto loc = temp.findLast(PATH_SEPARATOR);
        if (loc == CString::npos) {
            return;
        }
        str.eraseAfter(loc + 1);
    }

    operator CString() const {
        return str;
    }

    operator std::string() const {
        return str;
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