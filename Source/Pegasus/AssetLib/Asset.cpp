/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Asset.cpp
//! \author Kleber Garcia
//! \date   February 8 2015
//! \brief  Asset Class.

#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/ASTree.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/ByteStream.h"

#include <stdio.h>

using namespace Pegasus;
using namespace Pegasus::AssetLib;

namespace AssetPrivate
{
    void DumpElementToStream(int tabCount, const Object* obj, Utils::ByteStream& stream);

    void Tabulate(Utils::ByteStream& stream, int tabSz)
    {
        const char* TAB = "    ";
        const int TABSZ = 4;
        for (int i = 0; i < tabSz; ++i)  stream.Append(TAB, TABSZ);
    }
    
    void DumpElementToStream(const char* str, Utils::ByteStream& stream)
    {
        const char* Q = "\"";
        int strlen = Utils::Strlen(str);
        bool containsWhites = false;
        for (int i = 0; i < strlen && !containsWhites; ++i) containsWhites = str[i] == ' ' || str[i] == '\t';
        if (containsWhites) stream.Append(Q, 1);
        stream.Append(str, strlen);
        if (containsWhites) stream.Append(Q, 1);
    }

    void DumpElementToStream(int i, Utils::ByteStream& stream)
    {
        char accum[256];
        //TODO: replace sprintf with own implementation
        sprintf_s(accum, "%i", i, 256);
        DumpElementToStream(accum, stream);
    }

    void DumpElementToStream(float i, Utils::ByteStream& stream)
    {
        char accum[256];
        //TODO: replace sprintf with own implementation
        sprintf_s(accum, "%f", i, 256);
        DumpElementToStream(accum, stream);
    }

    void DumpElementToStream(int tabSize, Array* a, Utils::ByteStream& stream)
    {
        const char* LBRAC = "[";
        const int   LBRACSZ = 1;
        const char* RBRAC = "]";
        const int   RBRACSZ = 1;
        const char* COMMA = ", ";
        const int   COMMASZ = 2;

        stream.Append(LBRAC, LBRACSZ);
        switch(a->GetType())
        {
        case Array::AS_TYPE_STRING:
            for (int i = 0; i < a->GetSize(); ++i)
            {
                AssetPrivate::DumpElementToStream(a->GetElement(i).s, stream);
                if ( i != a->GetSize() - 1) stream.Append(COMMA, COMMASZ);
            }
            break;
        case Array::AS_TYPE_INT:
            for (int i = 0; i < a->GetSize(); ++i)
            {
                AssetPrivate::DumpElementToStream(a->GetElement(i).i, stream);
                if ( i != a->GetSize() - 1) stream.Append(COMMA, COMMASZ);
            }
            break;
        case Array::AS_TYPE_FLOAT:
            for (int i = 0; i < a->GetSize(); ++i)
            {
                AssetPrivate::DumpElementToStream(a->GetElement(i).f, stream);
                if ( i != a->GetSize() - 1) stream.Append(COMMA, COMMASZ);
            }
            break;
        case Array::AS_TYPE_OBJECT:
            for (int i = 0; i < a->GetSize(); ++i)
            {
                AssetPrivate::DumpElementToStream(tabSize + 1, a->GetElement(i).o, stream);
                if ( i != a->GetSize() - 1) stream.Append(COMMA, COMMASZ);
            }
            break;
        case Array::AS_TYPE_ARRAY:
            for (int i = 0; i < a->GetSize(); ++i)
            {
                AssetPrivate::DumpElementToStream(tabSize + 1, a->GetElement(i).a, stream);
                if ( i != a->GetSize() - 1) stream.Append(COMMA, COMMASZ);
            }
            break;
        }
        stream.Append(RBRAC, RBRACSZ);
    }

    void DumpElementToStream(int tabCount, const Object* obj, Utils::ByteStream& stream)
    {
        const char* L_BRAC = "{\n";
        const int L_BRACSZ = 2;

        const char* R_BRAC = "}";
        const int R_BRACSZ = 1;

        const char* COL = " : ";
        const int   COLZ = 3;

        const char* COMMA = ",";
        const int   COMMASZ = 1;

        const char* COMMANL = ",\n";
        const int   COMMANLSZ = 2;

        const char* NL = "\n";
        const int   NLSZ = 1;

        stream.Append(L_BRAC, L_BRACSZ);
        int totalEls = obj->GetIntCount() + obj->GetFloatCount() + obj->GetStringCount() + obj->GetArrayCount() + obj->GetObjectCount() - 1;
        int c = 0;

        for (int i = 0; i < obj->GetIntCount(); ++i, ++c)
        {
            AssetPrivate::Tabulate(stream, tabCount + 1);
            AssetPrivate::DumpElementToStream(obj->GetIntName(i),stream);
            stream.Append(COL, COLZ);
            AssetPrivate::DumpElementToStream(obj->GetInt(i),stream);
            if (c != totalEls) stream.Append(COMMANL, COMMANLSZ);
            else stream.Append(NL, NLSZ);
        }

        for (int i = 0; i < obj->GetFloatCount(); ++i, ++c)
        {
            AssetPrivate::Tabulate(stream, tabCount + 1);
            AssetPrivate::DumpElementToStream(obj->GetFloatName(i), stream);
            stream.Append(COL, COLZ);
            AssetPrivate::DumpElementToStream(obj->GetFloat(i), stream);
            if (c != totalEls) stream.Append(COMMANL, COMMANLSZ);
            else stream.Append(NL, NLSZ);
        }

        for (int i = 0; i < obj->GetStringCount(); ++i, ++c)
        {
            AssetPrivate::Tabulate(stream, tabCount + 1);
            AssetPrivate::DumpElementToStream(obj->GetStringName(i), stream);
            stream.Append(COL, COLZ);
            AssetPrivate::DumpElementToStream(obj->GetString(i), stream);
            if (c != totalEls) stream.Append(COMMANL, COMMANLSZ);
            else stream.Append(NL, NLSZ);
        }

        for (int i = 0; i < obj->GetArrayCount(); ++i, ++c)
        {
            AssetPrivate::Tabulate(stream, tabCount + 1);
            AssetPrivate::DumpElementToStream(obj->GetArrayName(i), stream);
            stream.Append(COL, COLZ);
            AssetPrivate::DumpElementToStream(tabCount + 1, obj->GetArray(i), stream);
            if (c != totalEls) stream.Append(COMMANL, COMMANLSZ);
            else stream.Append(NL, NLSZ);
        }

        for (int i = 0; i < obj->GetObjectCount(); ++i, ++c)
        {
            AssetPrivate::Tabulate(stream, tabCount + 1);
            AssetPrivate::DumpElementToStream(obj->GetObjectName(i), stream);
            stream.Append(COL, COLZ);
            AssetPrivate::DumpElementToStream(tabCount + 1, obj->GetObject(i), stream);
            if (c != totalEls) stream.Append(COMMANL, COMMANLSZ);
            else stream.Append(NL, NLSZ);
        }
    
        AssetPrivate::Tabulate(stream ,tabCount);
        stream.Append(R_BRAC, R_BRACSZ);
    }
}

Asset::Asset(Object* obj)
: mRoot(obj)
{
}

Asset::~Asset()
{
}

void Asset::SetPath(const char* path)
{
    int sz = Utils::Strlen(path) + 1; 
    PG_ASSERT(sz <= MAX_ASSET_PATH_STRING);
    mPathString[0] = '\0';
    Utils::Strcat(mPathString, path);
}

void Asset::DumpToStream(Utils::ByteStream& stream)
{
    if (mRoot != nullptr)
    {
        AssetPrivate::DumpElementToStream(0, mRoot, stream);
    }
}
