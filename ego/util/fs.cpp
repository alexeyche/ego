#include "fs.h"

#include <ego/util/string.h>

#include <fstream>
#include <dirent.h>
#include <sys/stat.h>


namespace NEgo {

    bool FileExists(const TString& name) {
        std::ifstream f(name.c_str());
        if (f.good()) {
            f.close();
            return true;
        } else {
            f.close();
            return false;
        }
    }
    
    bool DirExists(const TString& name) {
        struct stat info;

        if (stat(name.c_str(), &info ) != 0) {
            return false;
        } else 
        if(info.st_mode & S_IFDIR) {
            return true;
        } else {
            return false;
        }
    }

    void CreateDir(const TString& name) {
        const int dir_err = mkdir(name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        ENSURE(dir_err != -1, "Failed to create directory " << name);
    }

    TVector<TString> ListDir(TString path) {
        DIR *dpdf;
        struct dirent *epdf;
        class stat st;

        TVector<TString> files;
        dpdf = opendir(path.c_str());
        if (dpdf != NULL){
            while (true) {
                epdf = readdir(dpdf);
                if (!epdf) {
                    break;
                }

                const TString filename = TString(epdf->d_name);
                const TFsPath fullFilename = TFsPath(path) / TFsPath(filename);
                if (filename[0] == '.') {
                    continue;
                }
                if (stat(fullFilename.AsString().c_str(), &st) == -1) {
                    continue;
                }

                files.push_back(filename);
            }
        }
        closedir(dpdf);
        return files;
    }

    const TString TFsPath::Sep = "/";

    TFsPath::TFsPath(TString path)
        : Path(path)
    {
    }

    TFsPath TFsPath::operator/(const TFsPath p) {
        return NStr::Trim(Path, TFsPath::Sep) + TFsPath::Sep + NStr::Trim(p.Path, TFsPath::Sep);
    }

    TFsPath::operator TString() const {
        return Path;
    }

    std::ostream& operator<<(std::ostream& stream, const TFsPath& path) {
        stream << path.Path;
        return stream;
    }

    bool TFsPath::IsFile() const {
        class stat st;
        ENSURE(stat(Path.c_str(), &st) != -1, "Failed to read stat for file " << Path);
        return ((st.st_mode & S_IFREG) != 0) || ((st.st_mode & S_IFLNK) != 0);
    }

    TString TFsPath::AsString() const {
        return this->operator TString();
    }

    TFsPath& TFsPath::operator+(const char *suffix) {
        Path = Path + TString(suffix);
        return *this;
    }
} // namespace NEgo


