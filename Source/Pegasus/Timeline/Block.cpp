/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Block.cpp
//! \author	Karolyn Boulanger
//! \date	09th November 2013
//! \brief	Timeline block, describing the instance of an effect on the timeline

#include "Pegasus/Timeline/TimelineManager.h"
#include "Pegasus/Application/RenderCollection.h"
#include "Pegasus/Timeline/Block.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/ASTree.h"

namespace Pegasus {
namespace Timeline {

BEGIN_IMPLEMENT_PROPERTIES(Block)
    IMPLEMENT_PROPERTY(Block, Color)
    IMPLEMENT_PROPERTY(Block, Beat)
    IMPLEMENT_PROPERTY(Block, Duration)
END_IMPLEMENT_PROPERTIES(Block)

Block::Block(Alloc::IAllocator * allocator, Core::IApplicationContext * appContext)
:   mAllocator(allocator)
,   mAppContext(appContext)
,   mLane(nullptr)
,   mTimelineScript(nullptr)
,   mVmState(nullptr)
,   mScriptVersion(-1)
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
,   mBlockScriptObserver(this)
#endif  // PEGASUS_ENABLE_PROXIES
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to a timeline Block object");
    PG_ASSERTSTR(appContext != nullptr, "Invalid application context given to a timeline Block object");
    
    BEGIN_INIT_PROPERTIES(Block)
        INIT_PROPERTY(Color)
        INIT_PROPERTY(Beat)
        INIT_PROPERTY(Duration)
    END_INIT_PROPERTIES()
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    static unsigned sNextGuid = 0;
    mGuid = sNextGuid++;
    mCategory.SetUserData(mGuid);
#endif
}

//----------------------------------------------------------------------------------------

Block::~Block()
{
#if PEGASUS_ENABLE_PROXIES
    if (mTimelineScript != nullptr)
    {
        mTimelineScript->UnregisterObserver(&mBlockScriptObserver);
    }
#endif
}

//----------------------------------------------------------------------------------------

void Block::Initialize()
{
    // No default implementation
}

//----------------------------------------------------------------------------------------

void Block::Shutdown()
{
    if (mTimelineScript != nullptr)
    {
        mTimelineScript->CallGlobalScopeDestroy(mVmState);
        mTimelineScript = nullptr;
        mRuntimeListener.Shutdown();

        if (mVmState->GetUserContext() != nullptr)
        {
            Application::RenderCollection* userCtx = static_cast<Application::RenderCollection*>( mVmState->GetUserContext() );
            mAppContext->GetRenderCollectionFactory()->DeleteRenderCollection(userCtx);
        }
        PG_DELETE(mAllocator, mVmState);
        mVmState = nullptr;
    }
}

//----------------------------------------------------------------------------------------

void Block::InitializeScript()
{
    if (mScriptVersion != mTimelineScript->GetSerialVersion() && mTimelineScript->IsScriptActive())
    {
        mScriptVersion = mTimelineScript->GetSerialVersion();

        if (mVmState->GetUserContext() != nullptr)
        {
            Application::RenderCollection* nodeContaier = static_cast<Application::RenderCollection*>(mVmState->GetUserContext());
            nodeContaier->Clean();
        }
        mVmState->Reset();

        //just listen for runtime events on the global scope initialization
        mVmState->SetRuntimeListener(&mRuntimeListener);
        //re-initialize everything!
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
        mCategory.RemoveAssets();
        mAppContext->GetAssetLib()->BeginCategory(&mCategory);
#endif
        mTimelineScript->CallGlobalScopeInit(mVmState, this); 

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
        mAppContext->GetAssetLib()->EndCategory();
#endif

        // remove the listener. No need to listen for more events.
        mVmState->SetRuntimeListener(nullptr);
    }
}

//----------------------------------------------------------------------------------------

void Block::UninitializeScript()
{
    //TODO: remove this global scope destroy stuff
    if (mTimelineScript != nullptr && mTimelineScript->IsDirty())
    {
        mTimelineScript->CallGlobalScopeDestroy(mVmState);
    }
}

//----------------------------------------------------------------------------------------

void Block::UpdateViaScript(float beat, Wnd::Window* window)
{
    if (mTimelineScript != nullptr)
    {
        InitializeScript(); //in case a dirty compilation has been carried on.
        Application::RenderCollection* nodeContainer = static_cast<Application::RenderCollection*>(mVmState->GetUserContext());
        nodeContainer->SetWindow(window);
        mTimelineScript->CallUpdate(beat, mVmState);
    }
}

//----------------------------------------------------------------------------------------

void Block::NotifyInternalObjectPropertyUpdated(unsigned int index)
{
    if (mTimelineScript != nullptr && mTimelineScript->IsScriptActive())
    {
        BlockRuntimeScriptListener::UpdateType updateType = mRuntimeListener.FlushProperty(*mVmState, index);
        if (updateType == BlockRuntimeScriptListener::RERUN_GLOBALS)
        {
            mScriptVersion = -1; //invalidate the script version, only when running globals
        }
        PEGASUS_EVENT_DISPATCH(this, Pegasus::PropertyGrid::PropertyGridRenderRequest);
    }
}

//----------------------------------------------------------------------------------------

void Block::RenderViaScript(float beat, Wnd::Window* window)
{
    if (mTimelineScript != nullptr)
    {
        Application::RenderCollection* nodeContainer = static_cast<Application::RenderCollection*>(mVmState->GetUserContext());
        nodeContainer->SetWindow(window);
        mTimelineScript->CallRender(beat, mVmState);
    }
}

//----------------------------------------------------------------------------------------

void Block::AttachScript(TimelineScriptInOut script)
{
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES    
    mCategory.RegisterAsset(script->GetOwnerAsset());    
#endif
    if (mTimelineScript == nullptr)
    {
        mTimelineScript = script;

        if (mVmState == nullptr)
        {
            mVmState = PG_NEW(mAllocator, -1, "Vm State", Pegasus::Alloc::PG_MEM_PERM) BlockScript::BsVmState();
            mVmState->Initialize(mAllocator);
            Application::RenderCollection* userContext = mAppContext->GetRenderCollectionFactory()->CreateRenderCollection();
            mVmState->SetUserContext(userContext);
        }
    }
    else
    {
#if PEGASUS_ENABLE_PROXIES
        mTimelineScript->UnregisterObserver(&mBlockScriptObserver);
#endif
        mTimelineScript = script;
        Application::RenderCollection* userCtx = static_cast<Application::RenderCollection*>( mVmState->GetUserContext() );
        if (userCtx != nullptr)
        {
            userCtx->Clean();
        }
        mVmState->Reset();
    }       
#if PEGASUS_ENABLE_PROXIES
    script->RegisterObserver(&mBlockScriptObserver);
#endif
    mScriptVersion = -1;  
    mRuntimeListener.Initialize(this, mTimelineScript->GetBlockScript());
    script->Compile();
    InitializeScript();
}

//----------------------------------------------------------------------------------------

void Block::ShutdownScript()
{
    if (mTimelineScript != nullptr)
    {
#if PEGASUS_ENABLE_PROXIES
        mTimelineScript->UnregisterObserver(&mBlockScriptObserver);
#endif
        if (mVmState != nullptr)
        {
            if (mVmState->GetUserContext() != nullptr)
            {
                Application::RenderCollection* nodeContaier = static_cast<Application::RenderCollection*>(mVmState->GetUserContext());
                nodeContaier->Clean();
            }
            mVmState->Reset();
        }
        mTimelineScript = nullptr;
    }
    
    mRuntimeListener.Shutdown();
}

//----------------------------------------------------------------------------------------

void Block::SetLane(Lane * lane)
{
    PG_ASSERTSTR(lane != nullptr, "Invalid lane associated with a block");
    mLane = lane;
}

bool Block::OnReadObject(Pegasus::AssetLib::AssetLib* lib, AssetLib::Asset* owner, AssetLib::Object* root) 
{
    int typeId = root->FindString("type");
    if (typeId == -1 || Utils::Strcmp(root->GetString(typeId), GetClassName()))
    {
        return false;
    }

    int propsId = root->FindObject("props");
    if (propsId != -1)
    {
        AssetLib::Object* propsObj = root->GetObject(propsId);
        PropertyGrid::PropertyGridObject::ReadFromObject(owner, propsObj);

        int scriptId = propsObj->FindAsset("script");
        if (scriptId != -1 && propsObj->GetAsset(scriptId)->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == Pegasus::ASSET_TYPE_BLOCKSCRIPT.mTypeGuid)
        {
            TimelineScriptRef script = static_cast<TimelineScript*>(&(*propsObj->GetAsset(scriptId)));
            if (script != nullptr)
            {
                AttachScript(script);
            }
        }
    }
    else
    {
        return false;
    }


    return true;

}

void Block::OnWriteObject(Pegasus::AssetLib::AssetLib* lib, AssetLib::Asset* owner, AssetLib::Object* root) 
{
    root->AddString("type", GetClassName());
    AssetLib::Object* prop = owner->NewObject();
    root->AddObject("props", prop);
    if (mTimelineScript != nullptr && mTimelineScript->GetOwnerAsset() != nullptr)
    {
        prop->AddAsset("script", mTimelineScript);
    }
    
    PropertyGrid::PropertyGridObject::WriteToObject(owner, prop);
}

#if PEGASUS_ENABLE_PROXIES
void Block::BlockScriptObserver::OnCompilationBegin()
{
    //try to initialize the script. Compile wont call this observer stuff again since it is not dirty.
    mBlock->UninitializeScript();
}
void Block::BlockScriptObserver::OnCompilationEnd()
{
    //try to initialize the script. Compile wont call this observer stuff again since it is not dirty.
    mBlock->InitializeScript();
}
#endif

}   // namespace Timeline
}   // namespace Pegasus
