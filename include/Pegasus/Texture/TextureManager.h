/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureManager.h
//! \author	Kevin Boulanger
//! \date	03rd November 2013
//! \brief	Global texture node manager, including the factory features

#ifndef PEGASUS_TEXTURE_TEXTUREMANAGER_H
#define PEGASUS_TEXTURE_TEXTUREMANAGER_H

#include "Pegasus/Graph/Node.h"
#include "Pegasus/Texture/Texture.h"
#include "Pegasus/Texture/TextureGenerator.h"
#include "Pegasus/Texture/TextureOperator.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Texture/TextureTracker.h"
#endif

namespace Pegasus {
    namespace Graph {
        class NodeManager;
    }
}

namespace Pegasus {
namespace Texture {

class ITextureFactory;

#if PEGASUS_USE_GRAPH_EVENTS
class ITextureEventListener;
#endif

//! Global texture node manager, including the factory features
class TextureManager
{
public:

    //! Constructor
    //! \note Registers all texture nodes of the Texture project
    //! \param nodeManager Pointer to the global node manager (!- nullptr)
    TextureManager(Graph::NodeManager * nodeManager, ITextureFactory * textureFactory);

    //! Destructor
    virtual ~TextureManager();


    //! Register a texture node class, to be called before any node of this type is created
    //! \param className String of the node class (maximum length NodeManager::MAX_CLASS_NAME_LENGTH)
    //! \param createNodeFunc Pointer to the texture node member function that instantiates the node
    //! \warning If the number of registered node classes reaches NodeManager::MAX_NUM_REGISTERED_NODES,
    //!          an assertion is thrown and the class does not get registered.
    //!          If that happens, increase the value of NodeManager::MAX_NUM_REGISTERED_NODES
    void RegisterTextureNode(const char * className, Graph::Node::CreateNodeFunc createNodeFunc);

    //! Create a texture node
    //! \param configuration Configuration of the texture, such as resolution and pixel format
    //! \return Reference to the created node, null reference if an error occurred
    TextureReturn CreateTextureNode(const TextureConfiguration & configuration);

    //! Create a texture generator node by class name
    //! \param className Name of the texture generator node class to instantiate
    //! \param configuration Configuration of the texture, such as resolution and pixel format
    //! \return Reference to the created node, null reference if an error occurred
    TextureGeneratorReturn CreateTextureGeneratorNode(const char * className,
                                                      const TextureConfiguration & configuration);

    //! Create an texture operator node by class name
    //! \param className Name of the texture operator node class to instantiate
    //! \param configuration Configuration of the texture, such as resolution and pixel format
    //! \return Reference to the created node, null reference if an error occurred
    TextureOperatorReturn CreateTextureOperatorNode(const char * className,
                                                    const TextureConfiguration & configuration);

#if PEGASUS_ENABLE_PROXIES
    //! Returns the texture tracker, to get a list of texture proxies
    const TextureTracker & GetTracker() const { return mTracker; }
#endif

#if PEGASUS_USE_GRAPH_EVENTS
    //! Registers an event listener so we can listen to texture specific events whilst constructing nodes.
    //! \param the event listener to use
    void RegisterEventListener(ITextureEventListener * eventListener) { mEventListener = eventListener; }
#endif
    
private:

    // The texture node manager is unique in each application
    PG_DISABLE_COPY(TextureManager)

        
    //! Register all the texture nodes of the Texture project
    void RegisterAllTextureNodes();


    //! Pointer to the node manager (!= nullptr)
    Graph::NodeManager * mNodeManager;

    //! Pointer to the GPU factory. Generates GPU data from cpu texture data
    ITextureFactory * mFactory;

#if PEGASUS_ENABLE_PROXIES
    //! Texture tracker, to get a list of texture proxies
    TextureTracker mTracker;
#endif

#if PEGASUS_USE_GRAPH_EVENTS
    ITextureEventListener * mEventListener;
#endif
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_TEXTUREMANAGER_H
