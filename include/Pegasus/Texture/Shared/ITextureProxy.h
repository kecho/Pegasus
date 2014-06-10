/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ITextureProxy.h
//! \author	Kevin Boulanger
//! \date	08th June 2014
//! \brief	Proxy interface, used by the editor to interact with a texture

#ifndef PEGASUS_TEXTURE_SHARED_ITEXTUREPROXY_H
#define PEGASUS_TEXTURE_SHARED_ITEXTUREPROXY_H

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Texture {


//! Proxy interface, used by the editor to interact with a texture
class ITextureProxy
{
public:

    //! Destructor
    virtual ~ITextureProxy() {};


    //! Get the name of the texture
    //! \return Name of the texture
    virtual const char * GetName() const = 0;

    //! Get the width of the texture
    //! \return Horizontal resolution of the texture in pixels (>= 1)
    virtual unsigned int GetWidth() const = 0;

    //! Get the height of the texture
    //! \return Vertical resolution of the texture in pixels (>= 1)
    virtual unsigned int GetHeight() const = 0;

    //! Get the depth of the texture
    //! \return Depth of the texture in pixels (>= 1)
    virtual unsigned int GetDepth() const = 0;

    //! Get the number of layers of the texture
    //! \return Number of layers for array textures, 6 for cube maps, 1 otherwise
    virtual unsigned int GetNumLayers() const = 0;
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_SHARED_ITEXTUREPROXY_H