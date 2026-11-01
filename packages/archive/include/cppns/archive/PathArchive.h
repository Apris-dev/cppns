#pragma once

#include <algorithm>

#include "Archive.h"
#include "cppns/container/Vector.h"

namespace Path {
    enum class Filter : uint8 {
        ALL,
        FILES,
        DIRECTORIES
    };
}

namespace Error::Path {
    CREATE_ERROR_TYPE(PathNotCreatable, "Path Creation Exception", "Could not create or use a directory for this path!", Error::Runtime);
}

// Builds a path and guarantees that the path does exist, unless the trailing component is a file (in which it may or may not exist)
class CPathArchive : public CSArchive {

public:

    CPathArchive() = default;

    CPathArchive(const CString& initialPath) {
        CPathArchive::write(initialPath);
    }

    [[nodiscard]] CString get() const {
        return str;
    }

    // Last component of the path, whether it is a file or a directory
    [[nodiscard]] CString top() const {
        CString temp = str;
        // Make sure we dont include the path separator at the end of the string
        if (!temp.isEmpty() && temp.bottom() == PATH_SEPARATOR)
            temp.pop();
        const auto loc = temp.findLast(PATH_SEPARATOR);
        return (loc == CString::npos) ? temp : temp.substr(loc + 1);
    }

    // False only for a file name that hasn't been created yet (or an empty path)
    [[nodiscard]] bool exists() const {
        if (str.isEmpty())
            return false;
        std::error_code ec;
        return std::filesystem::exists(getPath(), ec);
    }

    [[nodiscard]] bool isFile() const {
        if (str.isEmpty())
            return false;
        std::error_code ec;
        return std::filesystem::is_regular_file(getPath(), ec);
    }

    // A last component that doesn't exist and has no trailing separator is a file-to-be, so no guessing is needed
    [[nodiscard]] bool isDirectory() const {
        if (str.isEmpty())
            return false;
        // Trailing separator means directory
        if (str.bottom() == PATH_SEPARATOR)
            return true;
        std::error_code ec;
        return std::filesystem::is_directory(getPath(), ec);
    }

    [[nodiscard]] CString getFilename() const {
        return isDirectory() ? CString{} : top();
    }

    // Moves up one component. Returns false if already at the root (or empty).
    // The parent always exists, and the result keeps its trailing separator.
    bool previous() {
        if (str.isEmpty())
            return false;

        // If there is a trailing separator, ignore it
        size_t end = str.getSize();
        if (str.get(end - 1) == PATH_SEPARATOR)
            --end;

        // Don't walk above the root ("/" or "C:\")
        if (const size_t rootLen = getPath().root_path().native().size(); end <= rootLen)
            return false;

        const auto loc = str.substr(0, end).findLast(PATH_SEPARATOR);
        // If there is no path separator then the path is a single relative component
        //TODO: handle relative paths (from executable)
        if (loc == CString::npos) {
            str.clear();
            return true;
        }

        // Keep the separator so roots stay valid
        str.eraseAfter(loc + 1);
        return true;
    }

    // Names (not full paths) of the entries in this directory
    [[nodiscard]] TVector<CString> list(const Path::Filter filter = Path::Filter::ALL) const {
        TVector<CString> out;
        if (str.isEmpty())
            return out;

        std::error_code ec;
        for (std::filesystem::directory_iterator it(getPath(), ec), end; !ec && it != end; it.increment(ec)) {
            std::error_code typeEc;
            const bool isDir = it->is_directory(typeEc);
            if (filter == Path::Filter::FILES && isDir)
                continue;
            if (filter == Path::Filter::DIRECTORIES && !isDir)
                continue;
            // Guarantee UTF8
            out.push(it->path().filename().u8string());
        }
        out.sort();
        return out;
    }

    operator CString() const {
        return str;
    }

    operator std::string() const {
        return str;
    }

protected:

    // Takes the last component off the path and gives it back
    virtual size_t read(CString& outValue) override {
        outValue = top();
        previous();
        return 0;
    }
    
    // Appends 'inValue' to the path, creating directories as needed.
    // Throws Error::Path::PathNotCreatable if a file is in the way or a directory can't be created.
    // On failure the stored path is unchanged (directories created before the failure stay, like mkdir -p).
    virtual size_t write(const CString& inValue) override {
        // If in value is empty, we don't need to write anything
        if (inValue.isEmpty())
            return 0;

        CString temp = inValue;
        // Force Replacement of proper path separator
#if PATH_SEPARATOR == '\\'
        //TODO: string replaceall
        std::replace(temp.begin().itr, temp.end().itr, '/', '\\');
#elif PATH_SEPARATOR == '/'
        std::replace(temp.begin().itr, temp.end().itr, '\\', '/');
#endif

        const auto inPath = std::filesystem::path(temp.toUTF8());

        // Has trailing path separator
        const bool isDirectory = temp.bottom() == PATH_SEPARATOR;

        std::filesystem::path path;
        if (str.isEmpty()) {
            // Starting fresh: an absolute input keeps its root
            // PathArchive has no path in it, so the path should start off with root, like 'C:/'
            path = inPath.root_path();
        } else {
            path = getPath();

            // If currently on a file that exists on disk, we cannot create a path
            // Otherwise, we interpret previous input as a directory instead of a file
            if (str.bottom() != PATH_SEPARATOR && !ensureDirectory(path))
                throw Error::Path::PathNotCreatable();
        }

        // Split the path into multiple parts by separator
        TVector<std::filesystem::path> parts;
        for (const auto& part : inPath.relative_path()) {
            if (!part.empty())
                parts.push(part);
        }

        // Loop through each part and write their real cases to str
        for (size_t i = 0; i < parts.getSize(); ++i) {
            // Everything but the last part must be a directory. The last part only is if the input ended in a separator.
            const bool mustBeDir = (i + 1 < parts.getSize()) || isDirectory;
            const fsPath part = parts[i];
            if (part == ".")
                continue;
            if (part == "..") {
                path /= part;
                continue;
            }
            // Gets the real case for part, if it exists
            const fsPath real = match(path, part);
            // If the real path does not exist, keep the original case
            path /= real.empty() ? part : real;

            // If it doesn't need to be a directory, don't create it
            if (!mustBeDir)
                continue;

            // Create the directory, throw error if we couldn't
            if (!ensureDirectory(path))
                throw Error::Path::PathNotCreatable();
        }

        // Input was only "."
        if (path.empty())
            path = ".";

        CString out = path.u8string();
        // fsPath drops the trailing separator, so put it back
        if (isDirectory && out.bottom() != PATH_SEPARATOR)
            out += PATH_SEPARATOR;

        str = out;

        return 0;
    }

private:

    using fsPath = std::filesystem::path;

    [[nodiscard]] fsPath getPath() const {
        return fsPath(str.toUTF8());
    }

    // True if path is now an existing directory (created if it was missing).
    // False if something that isn't a directory is in the way, or creation failed.
    static bool ensureDirectory(const fsPath& path) {
        std::error_code ec;
        // True if it is an existing directory on disk
        if (std::filesystem::is_directory(path, ec))
            return true;
        ec.clear();
        // Exists but is not a directory
        if (std::filesystem::exists(path, ec))
            return false;
        ec.clear();

        // Creates the directory if nothing is there
        std::filesystem::create_directory(path, ec);

        // Return true if the directory was created or if it already existed (somehow)
        return !ec || std::filesystem::is_directory(path, ec);
    }

    // Swaps the case of ASCII letters
    static fsPath swapCase(const fsPath& path) {
        String::UTF8 s = path.u8string();
        for (auto& c : s) {
            if (c >= u8'a' && c <= u8'z')
                c = static_cast<char8>(c - ('a' - 'A'));
            else if (c >= u8'A' && c <= u8'Z')
                c = static_cast<char8>(c + ('a' - 'A'));
        }
        return fsPath(s);
    }

    // Gets the true case for part in the directory.
    static fsPath match(const fsPath& dir, const fsPath& part) {
        std::error_code ec;

        // If given no directory, search the current one
        const fsPath searchDir =
            dir.empty() ? fsPath(".") : dir;

        // If case-insensitive (both regular and case swapped versions exist)
        // We can't prove that exists confirms we have the correct case
        if (std::filesystem::exists(searchDir / part, ec)) {
            const fsPath flipped = swapCase(part);
            ec.clear();

            // Since the flipped version doesnt exist, we are in a case-sensitive directory and can return the part that was confirmed to exist
            if (flipped.native() == part.native() || !std::filesystem::exists(searchDir / flipped, ec))
                return part;
        }
        ec.clear();

        const CString desired = CString(part.u8string()).getLower();
        fsPath best;

        // Search directory for the proper casing
        for (std::filesystem::directory_iterator it(searchDir, ec), end; !ec && it != end; it.increment(ec)) {
            const fsPath name = it->path().filename();
            // If case-insensitive, we return name if they are equal, since name will have the real case
            // If case-sensitive, this will only ever return the real path (and not one with different case)
            if (name.native() == part.native())
                return name;

            // This occurs in the edge case where two paths exist that only differ in the case (Like '/root' and '/Root')
            // This can only occur in a case-sensitive fs (like linux)
            // In this scenario, we pick the one that matches part the best
            if (CString(name.u8string()).getLower() == desired && (best.empty() || name.native() < best.native()))
                best = name;
        }
        return best;
    }

    CString str;

};