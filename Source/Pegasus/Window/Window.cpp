/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Window.cpp
//! \author David Worsham
//! \date   04th July 2013
//! \brief  Class for a single window in a Pegasus application.

#include "Pegasus/Window/Window.h"
#include "Pegasus/Render/RenderContext.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "../Source/Pegasus/Window/IWindowImpl.h"
#include "Pegasus/Sound/Sound.h"
#include "Pegasus/Timeline/TimelineManager.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Window/WindowProxy.h"
#endif  // PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Wnd {

//! Message handler object for a Window
class WindowMessageHandler : public IWindowMessageHandler
{
public:
    explicit WindowMessageHandler(Window* parent);
    virtual ~WindowMessageHandler();

    // IWindowMessageHandler interface
    virtual void OnCreate(Os::WindowHandle handle);
    virtual void OnDestroy();
    virtual void OnRepaint();
    virtual void OnResize(unsigned int width, unsigned int height);
#if PEGASUS_ENABLE_PROXIES
    //! Handles events on mouse clicks and such.
    virtual void OnMouseEvent(IWindowComponent::MouseButton button, bool isDown, float x, float y);
#endif

private:
    // No copies allowed
    PG_DISABLE_COPY(WindowMessageHandler);

    bool mUseBasicContext;
    Window* mParent;
};

//----------------------------------------------------------------------------------------

WindowMessageHandler::WindowMessageHandler(Window* parent)
 : mParent(parent)
{
}

//----------------------------------------------------------------------------------------

WindowMessageHandler::~WindowMessageHandler()
{
}

//----------------------------------------------------------------------------------------

void WindowMessageHandler::OnCreate(Os::WindowHandle handle)
{
    Render::ContextConfig contextConfig;

    // Create context
    contextConfig.mAllocator = mParent->mRenderAllocator;
    contextConfig.mOwnerWindowHandle = handle;
    contextConfig.mDevice = mParent->GetRenderDevice();
    mParent->GetDimensions(contextConfig.mWidth, contextConfig.mHeight);
    mParent->mRenderContext = PG_NEW(mParent->mRenderAllocator, -1, "Render::Context", Pegasus::Alloc::PG_MEM_PERM) Render::Context(contextConfig);
    mParent->mContextCreated = true;
}

//----------------------------------------------------------------------------------------

void WindowMessageHandler::OnDestroy()
{
     // Destroy context
    PG_DELETE(mParent->mRenderAllocator, mParent->mRenderContext);
    mParent->mContextCreated = false;
}

//----------------------------------------------------------------------------------------

void WindowMessageHandler::OnRepaint()
{
    if (mParent->mContextCreated)
    {
        mParent->Draw();
    }
}

//----------------------------------------------------------------------------------------

void WindowMessageHandler::OnResize(unsigned int width, unsigned int height)
{
    mParent->mWidth = width;
    mParent->mHeight = height;
    mParent->mRatio = (height > 0 ? static_cast<float>(width) / static_cast<float>(height) : 1.0f);
    mParent->mRenderContext->Resize(width, height);
}

#if PEGASUS_ENABLE_PROXIES
//----------------------------------------------------------------------------------------

void WindowMessageHandler::OnMouseEvent(IWindowComponent::MouseButton button, bool isDown, float x, float y)
{
    for (unsigned i = 0; i < mParent->mComponents.GetSize(); ++i)
    {
        mParent->mComponents[i].mComponent->OnMouseEvent(mParent->mComponents[i].mState,  button, isDown, x, y);
    }
}

#endif

//----------------------------------------------------------------------------------------

Window::Window(const WindowConfig& config)
:   mAllocator(config.mAllocator),
    mComponents(config.mAllocator),
    mRenderAllocator(config.mRenderAllocator),
    mDevice(config.mDevice),
    mWindowContext(config.mWindowContext),
    mRenderContext(nullptr),
    mContextCreated(false),
    mWidth(config.mWidth),
    mHeight(config.mHeight),
    mRatio(config.mHeight > 0 ? static_cast<float>(config.mWidth) / static_cast<float>(config.mHeight)   : 1.0f),
    mRatioInv(config.mWidth > 0 ? static_cast<float>(config.mHeight) / static_cast<float>(config.mWidth) : 1.0f),
    mIsChild(config.mIsChild)
{
    // Create platform stuff
    mMessageHandler = PG_NEW(mAllocator, -1, "Window message handler", Pegasus::Alloc::PG_MEM_PERM) WindowMessageHandler(this);
    mPrivateImpl = IWindowImpl::CreateImpl(config, mAllocator, mMessageHandler);

#if PEGASUS_ENABLE_PROXIES
    //! Create the proxy associated with the timeline
    mProxy = PG_NEW(mAllocator, -1, "Window::mProxy", Pegasus::Alloc::PG_MEM_PERM) WindowProxy(this);
#endif  // PEGASUS_ENABLE_PROXIES
}

//----------------------------------------------------------------------------------------

Window::~Window()
{
    // Destroy platform stuff
    IWindowImpl::DestroyImpl(mPrivateImpl, mAllocator);
    PG_DELETE(mAllocator, mMessageHandler);

#if PEGASUS_ENABLE_PROXIES
    //! Destroy the proxy associated with the window
    PG_DELETE(mAllocator, mProxy);
#endif
}

//----------------------------------------------------------------------------------------

Os::WindowHandle Window::GetHandle() const
{
    return mPrivateImpl->GetHandle();
}

//----------------------------------------------------------------------------------------

void Window::AttachComponent(IWindowComponent* component)
{
    Window::StateComponentPair& scp = mComponents.PushEmpty();
    ComponentContext ctx = { mWindowContext, this };
    scp.mState = component->CreateState(ctx);
    scp.mComponent = component;
}

//----------------------------------------------------------------------------------------

void Window::RemoveComponents()
{
    for (unsigned int i = 0; i < mComponents.GetSize(); ++i)
    {
        Window::StateComponentPair& scp = mComponents[i];
        ComponentContext ctx = { mWindowContext, this };
        scp.mComponent->DestroyState(ctx, scp.mState);
    }

    mComponents.Clear();
}

//----------------------------------------------------------------------------------------

void Window::Draw()
{
    if (mRenderContext != nullptr)
    {
        //Use this context on this thread.
        mRenderContext->Bind();
        
        ComponentContext ctx = { mWindowContext, this };

        //call Update for all components.
        for (unsigned int i = 0; i < mComponents.GetSize(); ++i)
        {
            Window::StateComponentPair& scp = mComponents[i];
            scp.mComponent->WindowUpdate(ctx, scp.mState);
        }

        //call Render for all components.
        for (unsigned int i = 0; i < mComponents.GetSize(); ++i)
        {
            Window::StateComponentPair& scp = mComponents[i];
            scp.mComponent->Render(ctx, scp.mState);
        }

        //Double buffer / end frame update.
        mRenderContext->Swap();
    }
    else
    {
        PG_FAILSTR("Trying to refresh a window but its render context is undefined");
    }
}

//----------------------------------------------------------------------------------------

void Window::Resize(unsigned int width, unsigned int height)
{
    mPrivateImpl->Resize(width, height);
    mWidth    = width;
    mHeight   = height;
    mRatio    = (height > 0 ? static_cast<float>(width) / static_cast<float>(height) : 1.0f);
    mRatioInv = (width > 0 ? static_cast<float>(height) / static_cast<float>(width) : 1.0f);
}

//----------------------------------------------------------------------------------------
void Window::HandleMainWindowEvents()
{
    if (!mIsChild)
    {
        mPrivateImpl->SetAsMainWindow();
    }
}

#if PEGASUS_ENABLE_PROXIES

//----------------------------------------------------------------------------------------

void Window::OnMouseEvent(IWindowComponent::MouseButton button, bool isDown, float x, float y)
{
    //broadcast mouse event to components
    for (unsigned int i = 0; i < mComponents.GetSize(); ++i)
    {
        Window::StateComponentPair& scp = mComponents[i];
        scp.mComponent->OnMouseEvent(scp.mState, button, isDown, x, y);
    }
}

//----------------------------------------------------------------------------------------

void Window::OnKeyEvent(char key, bool isDown)
{
    //broadcast keyboard event to components
    for (unsigned int i = 0; i < mComponents.GetSize(); ++i)
    {
        Window::StateComponentPair& scp = mComponents[i];
        scp.mComponent->OnKeyEvent(scp.mState, key, isDown);
    }
}
#endif

}   // namespace Wnd
}   // namespace Pegasus
