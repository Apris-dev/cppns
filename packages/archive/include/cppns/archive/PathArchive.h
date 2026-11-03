#pragma once

#include <algorithm>
#include <array>
#include <filesystem>
#include <system_error>
#include <unordered_map>

#include "Archive.h"
#include "cppns/container/Array.h"
#include "cppns/container/Vector.h"

namespace Path {
    enum class Filter : uint8 {
        ALL,
        FILES,
        DIRECTORIES
    };
}

namespace Error::Path {
    CREATE_ERROR_TYPE(PathNotCreatable, "Path Creation Exception", "Could not create or use a directory for this path! Reason: {}", Error::Runtime);
}

/* Path Convention:
 * 'data/file.txt, ./data/file.txt' - working directory (standard)
 * '/data/file.txt, C:/data/file.txt' - absolute (standard)
 * '../x, .' - standard: parent / this directory
 * '~/data/file.txt' - resource root (the executable directory in release builds)
 */

// Builds a path and guarantees that the path does exist, unless the trailing component is a file (in which it may or may not exist)
class CPathArchive : public CSArchive {

public:

    CPathArchive() = default;

    CPathArchive(const CString& initialPath) {
        CPathArchive::write(initialPath);
    }

    // Always uses '/', on every platform
    [[nodiscard]] CString get() const {
        return m_Path.generic_u8string();
    }

    // Native separators ('\' on Windows), only for APIs that insist on them
    [[nodiscard]] CString getNative() const {
        fsPath p = m_Path;
        return p.make_preferred().u8string();
    }

    // Last component of the path, whether it is a file or a directory
    [[nodiscard]] CString top() const {
        return (m_Path.has_filename() ? m_Path : m_Path.parent_path()).filename().u8string();
    }

    // False only for a file name that hasn't been created yet (or an empty path)
    [[nodiscard]] bool exists() const {
        std::error_code ec;
        return !m_Path.empty() && std::filesystem::exists(m_Path, ec);
    }

    [[nodiscard]] bool isFile() const {
        std::error_code ec;
        return !m_Path.empty() && std::filesystem::is_regular_file(m_Path, ec);
    }

    // A last component that doesn't exist and has no trailing separator is a file-to-be
    [[nodiscard]] bool isDirectory() const {
        std::error_code ec;
        return !m_Path.empty() && (!m_Path.has_filename() || std::filesystem::is_directory(m_Path, ec));
    }

    [[nodiscard]] CString getFilename() const {
        return isDirectory() ? CString{} : top();
    }

    // Moves up one component. Returns false if already at the root (or empty).
    // The result keeps a trailing separator, and the parent always exists.
    bool previous() {
        if (!m_Path.has_relative_path())
            return false;
        m_Path = (m_Path / "..").lexically_normal();
        return true;
    }

    // Names (not full paths) of the entries in this directory
    [[nodiscard]] TVector<CString> list(const Path::Filter filter = Path::Filter::ALL) const {
        TVector<CString> out;
        if (m_Path.empty())
            return out;

        std::error_code ec;
        for (std::filesystem::directory_iterator it(m_Path, ec), end; !ec && it != end; it.increment(ec)) {
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
        return get();
    }

    operator std::string() const {
        return get();
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

        // Replace windows separators with the standard separator
        String::UTF8 in = inValue.toUTF8();
        std::ranges::replace(in, u8'\\', u8'/');

        // Resolves path without touching the disk
        const fsPath target = getResolvedPath(in);

        // dir is guaranteed to be all directories and should also be guaranteed to exist
        const fsPath dir = target.parent_path();

        // The filename, only guarnteed to be a directory if it has a trailing separator '/'
        const fsPath name = target.filename();

        // Walk from the root and add components
        fsPath built = dir.root_path();
        for (const auto& part : dir.relative_path()) {
            const fsPath real = match(built, part);
            // If the real case doesnt exist keep the caller's spelling
            built /= real.empty() ? part : real;
            ensureDirectory(built);
        }

        // Add the filename
        if (!name.empty()) {
            const fsPath real = match(built, name);
            // If the real case doesnt exist keep the caller's spelling
            built /= real.empty() ? name : real;
        }

        // Add a trailing separator if it ends with a directory
        std::error_code ec;
        if (name.empty() || std::filesystem::is_directory(built, ec))
            built /= "";

        m_Path = built;
        return 0;
    }

private:

    using fsPath = std::filesystem::path;

    // Resolves the path from our convention to an absolute directory
    fsPath getResolvedPath(const String::UTF8& in) const {
        const bool fresh = m_Path.empty();

        // If a fresh archive we check for '~' to construct a path from the executable
        if (fresh) {
            if (const size_t slash = in.find(u8'/'); in.substr(0, slash) == u8"~") {
                if (slash == String::UTF8::npos)
                    return gExecutablePath;
                return (gExecutablePath / fsPath(in.substr(slash)).relative_path()).lexically_normal();
            }
        }

        // If fresh, start it from the working directory or continue from the current one
        std::error_code ec;
        const fsPath inPath(in);
        return ((fresh ? std::filesystem::current_path(ec) : m_Path) / (fresh ? inPath : inPath.relative_path())).lexically_normal();
    }

    // True if path is now an existing directory (created if it was missing).
    // False if something that isn't a directory is in the way, or creation failed.
    static void ensureDirectory(const fsPath& p) {
        std::error_code ec;

        // Isn't a directory or doesn't exist
        if (std::filesystem::is_directory(p, ec))
            return;

        // Exists and isn't a file
        ec.clear();
        if (std::filesystem::exists(p, ec))
            throw new Error::Path::PathNotCreatable("Tried to create a directory with the same name as a file!");

        // Attempt to create the directory
        ec.clear();
        std::filesystem::create_directory(p, ec);

        // Directory creation failed
        if (!ec)
            return;

        // Last resort check if the directory exists
        if (std::error_code otrEc; std::filesystem::is_directory(p, otrEc))
            return;

        // Directory could not be created and does not exist
        throw new Error::Path::PathNotCreatable(ec.message());
    }

    // Gets the true case for part in the directory.
    static fsPath match(const fsPath& dir, const fsPath& part) {
        std::error_code ec;

        // If dir is empty, search the working directory
        const fsPath searchDir = dir.empty() ? fsPath(".") : dir;

        // If case-insensitive (both regular and case swapped versions exist)
        // We can't prove that exists confirms we have the correct case
        if (std::filesystem::exists(searchDir / part, ec)) {
            const fsPath flipped = CString(part).getFlippedCase().toPath();
            ec.clear();

            // Since the flipped version doesnt exist, we are in a case-sensitive directory and can return the part that was confirmed to exist
            if (flipped.native() == part.native() || !std::filesystem::exists(searchDir / flipped, ec))
                return part;
        }
        ec.clear();

        const auto desired = CString(part).getLower().toPath().native();
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
            if (CString(name).getLower().toPath().native() == desired && (best.empty() || name.native() < best.native()))
                best = name;
        }

        // Return best if found
        if (!best.empty())
            return best;

        // The scan found nothing, but it may not be able to see everything (qemu sysroot remapping, listing denied by a sandbox).
        // Try the usual spellings by stat as a last resort.
        const CString capitalized = CString(part).getCapitalized();
        const CString lower = CString(part).getLower();
        const CString upper = CString(part).getUpper();
        for (const CString& candidate : TArray{lower, upper, capitalized}) {
            ec.clear();

            // If the candidate exists, return it
            if (std::filesystem::exists(searchDir / candidate.toPath(), ec))
                return candidate.toPath();
        }
        return {};
    }

    fsPath m_Path;
};