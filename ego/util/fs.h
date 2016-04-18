#pragma once

#include <ego/base/base.h>

namespace NEgo {

	bool FileExists(const TString& name);
    
    bool DirExists(const TString& name);

    void CreateDir(const TString& name);

    TVector<TString> ListDir(TString path);

    class TFsPath {
    public:
        static const TString Sep;

        TFsPath(TString path);

        TFsPath operator/(const TFsPath p);

        operator TString() const;

        friend std::ostream& operator<<(std::ostream& stream, const TFsPath& path);

        bool IsFile() const;

        TString AsString() const;

        TFsPath& operator+(const char *suffix);

    private:
        TString Path;
    };

} // namespace NEgo