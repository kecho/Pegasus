/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureManagerProxy.cpp
//! \author	Kevin Boulanger
//! \date	09th June 2014
//! \brief	Proxy object, used by the editor to interact with the textures

//! \todo Why do we need this in Rel-Debug? TextureManagerProxy should not even be compiled in REL mode
PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Texture/TextureManagerProxy.h"
#include "Pegasus/Texture/TextureManager.h"

namespace Pegasus {
namespace Texture {


TextureManagerProxy::TextureManagerProxy(TextureManager * textureManager)
:   mTextureManager(textureManager)
{
    PG_ASSERTSTR(textureManager != nullptr, "Trying to create a texture manager proxy from an invalid texture manager object");
}

//----------------------------------------------------------------------------------------

TextureManagerProxy::~TextureManagerProxy()
{
}

//----------------------------------------------------------------------------------------

unsigned int TextureManagerProxy::GetNumTextures() const
{
    return mTextureManager->GetTracker().GetNumTextures();
}

//----------------------------------------------------------------------------------------

ITextureProxy * TextureManagerProxy::GetTexture(unsigned int index) const
{
    Texture * texture = mTextureManager->GetTracker().GetTexture(index);
    if (texture != nullptr)
    {
        return texture->GetProxy();
    }
    else
    {
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

void TextureManagerProxy::RegisterEventListener(ITextureEventListener * eventListener)
{
    mTextureManager->RegisterEventListener(eventListener);
}

}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES