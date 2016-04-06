/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AssetLibProxy.cpp
//! \author Kleber Garcia
//! \date   February 8 2015
//! \brief  AssetLibProxy Class.

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/AssetLib/Proxy/AssetLibProxy.h"
#include "Pegasus/AssetLib/Proxy/AssetProxy.h"
#include "Pegasus/AssetLib/Proxy/RuntimeAssetObjectProxy.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/Category.h"

using namespace Pegasus;
using namespace Pegasus::AssetLib;

AssetLibProxy::AssetLibProxy(Pegasus::AssetLib::AssetLib* assetLib)
    : mAssetLib(assetLib)
{
}

AssetLibProxy::~AssetLibProxy()
{
}


Io::IoError AssetLibProxy::LoadAsset(const char* path, IAssetProxy** assetProxy)
{
    Asset* asset = nullptr;
    Io::IoError err = mAssetLib->LoadAsset(path, true, &asset);
    *assetProxy = asset->GetProxy();
    return err;
}

IAssetProxy* AssetLibProxy::CreateAsset(const char* path, bool isStructured)
{
    return mAssetLib->CreateAsset(path, isStructured)->GetProxy();
}

Io::IoError AssetLibProxy::SaveAsset(IAssetProxy* asset)
{
    return mAssetLib->SaveAsset(static_cast<AssetProxy*>(asset)->GetObject());
}

IRuntimeAssetObjectProxy* AssetLibProxy::LoadObject(const char* path, bool* outIsNew)
{
    RuntimeAssetObjectRef obj = mAssetLib->LoadObject(path);
    if (obj == nullptr)
    {
        return nullptr;
    }

    for (unsigned int i = 0; i < mObjects.GetSize(); ++i)
    {
        if (obj == mObjects[i])
        {
            if (outIsNew != nullptr)
            {
                *outIsNew = false;
            }
            return obj->GetProxy();
        }
    }

    mObjects.PushEmpty() = obj;
    if (outIsNew != nullptr)
    {
        *outIsNew = true;
    }
    return obj->GetProxy();
}

Io::IoError AssetLibProxy::SaveObject(IRuntimeAssetObjectProxy* object)
{
    return mAssetLib->SaveObject(static_cast<AssetProxy*>(object->GetOwnerAsset())->GetObject()->GetRuntimeData());
}

IRuntimeAssetObjectProxy* AssetLibProxy::CreateObject(const char* path, const PegasusAssetTypeDesc* desc)
{
    RuntimeAssetObjectRef obj = mAssetLib->CreateObject(path, desc);
    if (obj != nullptr)
    {
        mObjects.PushEmpty() = obj;
        return obj->GetProxy();
    }
    
    return nullptr;
}

void AssetLibProxy::CloseObject(IRuntimeAssetObjectProxy* object)
{
    RuntimeAssetObjectRef obj = static_cast<AssetProxy*>(object->GetOwnerAsset())->GetObject()->GetRuntimeData();
    for (unsigned int i = 0; i < mObjects.GetSize(); ++i)
    {
        if (mObjects[i] == obj)
        {
            mObjects.Delete(i);
            return;
        }
    }
}

unsigned AssetLibProxy::GetCategoryCount() const
{
    return mAssetLib->GetCategories().GetSize();
}

ICategoryProxy* AssetLibProxy::GetCategory(unsigned i)
{
    return mAssetLib->GetCategories()[i]->GetProxy();
}

void AssetLibProxy::SetEventListener(IAssetEventListener* listener)
{
    mAssetLib->SetEventListener(listener);
}

ICategoryProxy* AssetLibProxy::FindTypeCategory(const Pegasus::PegasusAssetTypeDesc* typeDesc)
{
    return mAssetLib->FindTypeCategory(typeDesc)->GetProxy();
}

#else

PEGASUS_AVOID_EMPTY_FILE_WARNING

#endif
