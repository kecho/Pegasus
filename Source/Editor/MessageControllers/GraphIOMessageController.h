/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphIOMessageController.h
//! \author	Karolyn Boulanger
//! \date	December 25th 2015
//! \brief	Graph IO controller, communicates events from the graph editor
//!         in a thread-safe way

#ifndef PEGASUS_GRAPHIOMESSAGECONTROLLER_H
#define PEGASUS_GRAPHIOMESSAGECONTROLLER_H

#include "Pegasus/Texture/Shared/TextureEventDefs.h"
#include "MessageControllers/AssetIOMessageController.h"
#include <QMap>
#include <QSet>

namespace Pegasus
{
    namespace Texture {
        class ITextureNodeProxy;
    }

    namespace App {
        class IApplicationProxy;
    }
}

class GraphNodeObserver;
class ViewportWidget;

//----------------------------------------------------------------------------------------

//! Graph IO controller, communicates events from the graph editor in a thread-safe way
//! \todo Make it work on generic graphs, not just textures
class GraphIOMessageController : public QObject, public IAssetTranslator, private Pegasus::Texture::ITextureNodeEventListener
{
    Q_OBJECT

public:

    //! Structure with the update of an element
    struct UpdateElement
    {
        QString mNodeName;

    public:

        UpdateElement()
        {
        }
    };


    // Accumulation of update elements
    struct UpdateCache
    {
        QVector<UpdateElement> mUpdateCache;
        UpdateCache()
        { }
    };


    //! User data attached to every texture
    class GraphNodeUserData : public Pegasus::Core::IEventUserData
    {
    public:
        GraphNodeUserData(Pegasus::Texture::ITextureNodeProxy* proxy, 
                            UpdateCache* updateCache)
            :   mTextureNodeProxy(proxy), mUpdateCache(updateCache)
            { }

        virtual ~GraphNodeUserData() { }

        Pegasus::Texture::ITextureNodeProxy* GetProxy() const { return mTextureNodeProxy; }
        UpdateCache* GetUpdateCache() const { return mUpdateCache; }

    private:
        Pegasus::Texture::ITextureNodeProxy* mTextureNodeProxy;
        UpdateCache* mUpdateCache;
    };


    //! Message container class so the UI can communicate with the application render thread
    class Message
    {
    public:
        enum MessageType
        {
            VIEW_GRAPH_ON_VIEWPORT,
            INVALID
        };
        

        //! Constructors
        explicit Message(MessageType type)
            : mMessageType(type), mGraphNodeObserver(nullptr), mTargetViewport(nullptr) { }
        
        Message()
            : mMessageType(INVALID), mGraphNodeObserver(nullptr), mTargetViewport(nullptr) { }
    

        // Getters
        inline MessageType GetMessageType() const { return mMessageType; }
        GraphNodeObserver* GetGraphNodeObserver() const { return mGraphNodeObserver; }
        ViewportWidget* GetTargetViewport() const { return mTargetViewport; }
        AssetInstanceHandle GetGraphHandle() const { return mGraphHandle; }

        // Setters
        void SetMessageType(MessageType t) { mMessageType = t; }
        void SetGraphNodeObserver(GraphNodeObserver* graphNodeObserver) { mGraphNodeObserver = graphNodeObserver; }
        void SetTargetViewport(ViewportWidget* viewport) { mTargetViewport = viewport; }
        void SetGraphHandle(AssetInstanceHandle graphHandle) { mGraphHandle = graphHandle; }
        
    private:

        //! Type of the message being sent
        MessageType mMessageType;
        GraphNodeObserver* mGraphNodeObserver;
        ViewportWidget* mTargetViewport;
        AssetInstanceHandle mGraphHandle;
    };


    //! Constructor
    explicit GraphIOMessageController(Pegasus::App::IApplicationProxy* appProxy);

    //! Destructor
    virtual ~GraphIOMessageController();

    //! \param object input asset to open
    //! \return a qvariant that contains the representation of this asset for the ui to read.
    virtual QVariant TranslateToQt(AssetInstanceHandle handle, Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //! \return null terminated list that contains all the necessary asset data so assets can be translated.
    virtual const Pegasus::PegasusAssetTypeDesc** GetTypeList() const;

    //! Called by the render thread when a render thread message should be processed
    //! \param m The message to get processed
    void OnRenderThreadProcessMessage(const Message& m);

    //! Call at the end of the frame, whenever it is ideal to flush all the changes in the graph
    void FlushAllPendingUpdates();

private:

    //message processing
    void OnRenderThreadViewGraphOnViewport(ViewportWidget* viewport, AssetInstanceHandle objectHandle);

    // Texture event listener callbacks (from app to UI)
    virtual void OnInitUserData(Pegasus::Texture::ITextureNodeProxy* proxy, const char* name);
    virtual void OnDestroyUserData(Pegasus::Texture::ITextureNodeProxy* proxy, const char* name);
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Texture::TextureNodeNotificationEvent& e);
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Texture::TextureNodeGenerationEvent& e);
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Texture::TextureNodeOperationEvent& e);

    Pegasus::App::IApplicationProxy* mAppProxy;

};


//! Class representing a texture node modifier callback. To be used when we want to update the view
//! after an element of the texture graph has been updated either by the UI or the render app
class GraphNodeObserver : public QObject
{
    friend class GraphIOMessageController;

    Q_OBJECT

public:

    //! Constructor
    GraphNodeObserver();
    
    //! Destructor
    virtual ~GraphNodeObserver() {}

    //! Called when this observer gets notified that the IO controller is done initializing
    virtual void OnInitialized() = 0;

    //! Called when an external process (the render thread) has edited an element of the graph
    virtual void OnUpdated() = 0;

    //! Called when the handle has been closed, or the proxy object has been destroyed and no longer exists
    virtual void OnShutdown() = 0;

signals:

    void OnInitializedSignal();
    void OnUpdatedSignal();
    void OnShutdownSignal();

private slots:

    void OnInitializedSlot() { OnInitialized(); }
    void OnUpdatedSlot() { OnUpdated(); }
    void OnShutdownSlot() { OnShutdown(); }
};

#endif  // PEGASUS_GRAPHIOMESSAGECONTROLLER_H
