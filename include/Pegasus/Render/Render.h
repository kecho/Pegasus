/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Render.h
//! \author Kleber Garcia
//! \date   25th March of 2014
//! \brief  Master render library. PARR - Pegasus Abstract Render Recipes	
//!         This library encapsulates most of the underlying API and serves as a way 
//!         of accessing it. Provides render shortcuts.

#ifndef PARR_H
#define PARR_H

#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Math/Color.h"
#include "Pegasus/Texture/Texture.h"

#define PEGASUS_RENDER_MAX_UNIFORM_NAME_LEN 64 

namespace Pegasus
{
namespace Render
{

    class Constants
    {
    public:
        static const int MAX_RENDER_TARGETS = 8;
    };

    //! Structure representing a uniform location in a particular shader
    //! The name holds a copy of the actual uniform name.
    //! The internal values are obfuscated, not to be used.
    struct Uniform
    {
        char mName[PEGASUS_RENDER_MAX_UNIFORM_NAME_LEN];
        int  mInternalIndex;
        int  mInternalOwner;
        int  mInternalVersion;
    public:
            Uniform()
            : mInternalIndex(-1), mInternalVersion(-1), mInternalOwner(-1)
            {
                mName[0] = 0;
            }
    };

    //! Structure representing a buffer location in the GPU
    //! the size holds the size of this buffer at creation time
    //! the mInternalData is data used by the specific graphis library
    //! In openGL this would be used as buffer usage and the index of the buffer
    //! in D3D this would be the raw device child pointer
    struct Buffer
    {
        int mSize;
        void* mInternalData; //! 64 bit compressed data, used internally
    public:
        Buffer()
           : mInternalData(0), mSize(0)
        {
        }
    };

    //! Structure handle with the respective render target configuration
    struct RenderTargetConfig
    {
        int mWidth;
        int mHeight;
    public:
        RenderTargetConfig()
        : mWidth(-1), mHeight(-1)
        {
        }

        RenderTargetConfig(int width, int height)
        : mWidth(width), mHeight(height)
        {
        }
    };

    //! Structure handle with the respective cube map configuration
    struct CubeMapConfig
    {
        int mWidth;
        int mHeight;
    };
   
    //! Structure handle representing a render target and its data
    //! In openGL the internal data will hold the necesary opaque handles (views/framebuffer/rt)
    //! In directX it will hold the pointer to the actual rendertarget.
    struct RenderTarget
    {
        RenderTargetConfig mConfig;
        void* mInternalData;
    public:
        RenderTarget ()
            : mInternalData(nullptr)
        {
        }
    };

    //! Structure handle representing a cube map
    struct CubeMap
    {
        CubeMapConfig mConfig;
        void* mInternalData;
    public:
        CubeMap ()
            : mInternalData(nullptr)
        {
        }
    };

    //! Structure handle representing an actual depth stencil target
    struct DepthStencilTarget
    {
        void* mInternalData;
    public:
        DepthStencilTarget()
            : mInternalData(nullptr)
        {
        }
    };

    //! Enumeration representing the cube map face to set in a render target
    enum CubeFace
    {
        X, NX, Y, NY, Z, NZ
    };

    // primitive mode
    enum PrimitiveMode
    {        
        PRIMITIVE_TRIANGLE_LIST,
        PRIMITIVE_TRIANGLE_STRIP,
        PRIMITIVE_LINE_LIST,
        PRIMITIVE_LINE_STRIP,
        PRIMITIVE_POINTS,
        PRIMITIVE_AUTOMATIC,
        PRIMITIVE_COUNT
    };

    //! Container specifying rectangle viewport in pixel coordinates
    //! Represents a rectangle
    struct Viewport
    {
        int mXOffset;
        int mYOffset;
        int mWidth;
        int mHeight;
    public:
        Viewport()
        :
        mXOffset(-1), mYOffset(-1), mWidth(-1), mHeight(-1)
        {
        }

        Viewport(int x, int y, int width, int height)
        :
        mXOffset(x), mYOffset(y), mWidth(width), mHeight(height)
        {
        }

        Viewport(int width, int height)
        :
        mXOffset(0), mYOffset(0), mWidth(width), mHeight(height)
        {
        }
    };

    //! Structure containing configuration for rasterizer state
    struct RasterizerConfig
    {
        enum PegasusDepthFunc
        {
            NONE_DF,
            GREATER_DF,
            LESSER_DF,
            GREATER_EQUAL_DF,
            LESSER_EQUAL_DF,
            EQUAL_DF,
            COUNT_DF
        };

        enum PegasusCullMode
        {
            NONE_CM,
            CCW_CM,
            CW_CM,
            COUNT_CM
        };

        RasterizerConfig ()
        :    mCullMode(NONE_CM),
             mDepthFunc(NONE_DF)
        { 
        }    

        PegasusCullMode  mCullMode;
        PegasusDepthFunc mDepthFunc;

       
    };

    //! Structure representing a rasterizer state
    struct RasterizerState
    {
        RasterizerConfig mConfig;
        void * mInternalData;
        void * mInternalDataAux;
    };

    //! Structure containing configuration for blending state
    struct BlendingConfig
    {
        enum BlendOperator
        {
            NONE_BO,
            ADD_BO,
            SUB_BO,
            COUNT_BO
        };

        enum Multiplicator
        {
            ZERO_M,
            ONE_M,
            COUNT_M
        };

        BlendingConfig()
        : mBlendingOperator(NONE_BO),
          mSource(ZERO_M),
          mDest(ZERO_M)
        {
        }
        
        BlendOperator mBlendingOperator;
        Multiplicator mSource;
        Multiplicator mDest;
    };

    //! Structure representing a blending state
    struct BlendingState
    {
        BlendingConfig mConfig;
        void * mInternalData;
    };

    //! Dispatches a shader pipeline.
    //! \param program the shader program to dispatch
    void SetProgram (Shader::ProgramLinkageInOut program);

    //! Dispatches a mesh.
    //! \param mesh that the system will dispatch.
    //! \WARNING: a shader must have been dispatched before this call.
    //!           not dispatching a shader will cause the mesh to be incorrectly rendered
    //!           or throw an assert
    void SetMesh (Mesh::MeshInOut mesh);

    //! Sets a viewport
    //! \param the viewport to set
    void SetViewport(const Viewport& viewport);

    //! Sets a viewport using the render target's dimensions
    //! \param the render target's whose dimensions be used as viewport
    void SetViewport(const RenderTarget& viewport);

    //! Sets a viewport using the depth stencil target's dimensions
    //! \param the depthstencil target, which dimensions used as viewport
    void SetViewport(const DepthStencilTarget& viewport);

    //! Dispatches a render target
    //! \param render target to dispatch
    //! \NOTE clears the depth target being dispatched
    void SetRenderTarget (RenderTarget& renderTarget);

    //! Dispatches a render target 
    //! \param render target to dispatch
    //! \param depth stencil to be set
    void SetRenderTarget (RenderTarget& renderTarget, DepthStencilTarget& depthStencil);

    //! Dispatches a set of render targets
    //! \param number of targets  to dispatch
    //! \param render target array to dispatch
    //! \note clears the depth target being dispatched
    //! \note renderTargetCount must be between 0 and MAX_RENDER_TARGETS
    void SetRenderTargets (int renderTargetCount, RenderTarget** renderTarget);

    //! Dispatches a set of render targets and depth stencil
    //! \param number of targets  to dispatch
    //! \param render target array
    //! \param depth stencil dispatched
    //! \note renderTargetCount must be between 0 and MAX_RENDER_TARGETS
    void SetRenderTargets (int renderTargetNum, RenderTarget** renderTarget, DepthStencilTarget& depthStencil);

    //! Sets all color / depth and stencil to null 
    //! \note this is equivalent of calling SetRenderTargets(0, nullptr);
    void ClearAllTargets();

    //! Sets the default render target frame buffer (the basic window render target)
    //! Sets also the default depth stencil buffer
    void DispatchDefaultRenderTarget();

    //! Clears the selected buffers
    void Clear(bool color, bool depth, bool stencil);

    //! Sets the clear color
    //! \param col color vector, rgba to set the clear command to
    //! \note the components must be normalized (from 0 to 1)
    void SetClearColorValue(const Pegasus::Math::ColorRGBA& col);

    //! Sets the rasterizer state
    //! \param rasterState
    void SetRasterizerState(const RasterizerState& rasterState);

    //! Sets the blending state
    //! \param blendingState
    void SetBlendingState(const BlendingState& blendignState);

    //! Sets the depth clear value
    //! \param the depth scalar value to clear to
    //! \note the value must be from 0 to 1
    void SetDepthClearValue(float d);

    //! Sets the primitive mode to render to.
    void SetPrimitiveMode(PrimitiveMode mode);

    //! Draws geometry.
    //! \note Requires: -Shader to be dispatched
    //!                 -Mesh to be dispatched
    void Draw();

    //! Fills a uniform reference by name
    //! \param program, the program containing the uniform to get
    //! \param outputUniform, the empty uniform structure to be filled containing the metadata required
    //! \param name, the constant name of the uniform that we will be trying to find in the uniform table
    //! \return boolean, true if the uniform was found, false otherwise
    bool GetUniformLocation(Shader::ProgramLinkageInOut program, const char * name, Uniform& outputUniform);

    //! Creates a buffer optimized for uniform usage
    //! \param bufferSize, the size of the buffer to be used in bytes
    //! \param outputBuffer the output struct to be filled
    void CreateUniformBuffer(int bufferSize, Buffer& outputBuffer);

    //! Creates a render target with the assigned texture configuration
    //! \param configuration of the render target
    //! \param output render target to fill / create
    void CreateRenderTarget(RenderTargetConfig& config, RenderTarget& renderTarget);

    //! Creates a render target from a cube map
    //! \param CubeFace the face of the cube map to set this target to
    //! \param the cube map to use for this target
    //! \param the render target to be set
    void CreateRenderTargetFromCubeMap(CubeFace targetFace, CubeMap& cubeMap, RenderTarget& renderTarget); 

    //! Creates a cube map from a description
    //! \param config the cube map configuration
    //! \param the cube map to create
    void CreateCubeMap(CubeMapConfig& config, CubeMap& cubeMap);

    //! Creates a rasterizer state given a configuration.
    //! \param config the configuration structure
    //! \param output rasterizer state created
    void CreateRasterizerState(RasterizerConfig& config, RasterizerState& rasterizerState);

    //! Creates a blending state given a configuration.
    //! \param config the configuration structure
    //! \param output blendign state created
    void CreateBlendingState(BlendingConfig& config, BlendingState& blendingState);

    //! Deletes a rasterizer state
    //! \param rasterizer state to delete
    void DeleteRasterizerState(RasterizerState& state);

    //! Deletes a rasterizer state
    //! \param blending state to delete
    void DeleteBlendingState(BlendingState& blendingState);

    //! Deletes a cube map
    //! \param cubeMap the cube map to delete
    void DeleteCubeMap(CubeMap& cubeMap);

    //! Memcpys a buffer to the gpu destination.
    //! \param dstBuffer the GPU destination buffer to copy to
    //! \param src the source buffer to use
    //! \param size, the size of the src buffer to copy to dstBuffer in bytes. If -1, 
    //!        it will use the size registered in dstBuffer
    //! \param offset, the offset to use
    void SetBuffer(Buffer& dstBuffer, void * src, int size = -1, int offset = 0);

    //! Destroys a render target with the assigned configuration
    //! \param output render target to fill / create
    void DeleteRenderTarget(RenderTarget& renderTarget);

    //! Deletes a buffer created from CreateUniformBuffer
    //! \param outputBuffer the buffer to delete
    void DeleteBuffer(Buffer& outputBuffer);

    //! Sets the uniform value to a texture
    //! \param u uniform to set the value to
    //! \param texture to set to the uniform slot
    //! \return boolean, true on success, false on error
    bool SetUniformTexture(Uniform& u, Texture::TextureInOut texture);

    //! Sets the uniform block buffer
    //! \param u uniform block to set the value to
    //! \param buffer to set to the uniform block slot
    //! \return boolean, true on success, false on error
    bool SetUniformBuffer(Uniform& u, const Buffer& buffer);

    //! Sets the render target as a texture view in the specified uniform location 
    //! \param u uniform parameter to set the value
    //! \param renderTarget render target to set as a texture view
    //! \return boolean, true on success, false on error
    bool SetUniformTextureRenderTarget(Uniform& u, const RenderTarget& renderTarget);

    //! Sets a cube map as a view
    //! \param u uniform parameter to set the value
    //! \param cubeMap cube map to set 
    //! \return boolean, true on success, false on error
    bool SetUniformCubeMap(Uniform& u, const CubeMap& cubeMap);

    //! function that internally cleans any dispatched programs / shaders / meshes
    //! from the global state.
    void CleanInternalState();

}
}

#endif
