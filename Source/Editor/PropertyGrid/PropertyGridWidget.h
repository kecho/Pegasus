/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridWidget.h
//! \author	Karolyn Boulanger
//! \date	August 4th, 2015
//! \brief	Widget show the tree of a property grid

#ifndef EDITOR_PROPERTYGRIDWIDGET_H
#define EDITOR_PROPERTYGRIDWIDGET_H

#include "PropertyGrid/PropertyGridPropertyManagers.h"
#include "PropertyGrid/PropertyGridEditorFactories.h"
#include "MessageControllers/MsgDefines.h"
#include "PropertyTypes.h"
#include <QWidget>
#include <QVector>
#include <QColor>


namespace Pegasus {
    namespace App {
        class IApplicationProxy;
    }
}

class PegasusDockWidget;
class PropertyGridObserver;
class QtTreePropertyBrowser;
class QLabel;

//! Property grid widget UI element
class PropertyGridWidget : public QWidget
{
    Q_OBJECT

public:
    friend class PgwObserver;

    //! Constructor
    PropertyGridWidget(QWidget * parent);

    //! Destructor
    ~PropertyGridWidget();

    //! Set the loaded application proxy
    //! \param application Application proxy being loaded, nullptr otherwise
    inline void SetApplicationProxy(Pegasus::App::IApplicationProxy * applicationProxy) { mApplicationProxy = applicationProxy; }

    //! Sets the messenger responsible to send messages to the render thread.
    //! \param messenger - the messenger to cache
    inline void SetMessenger(PegasusDockWidget * messenger) { mMessenger = messenger; }

    //! Set the property grid object proxy associated with the asset instance handle.
    //! the content of the property browser will update accordingly.
    //! \param handle of the instance of the asset object. If there is no property grid associated with this object
    //!        this call won't affect the contents of the property grid widget.
    void SetCurrentProxy(AssetInstanceHandle assetHandle);

    //! Clears all contents of this widget.
    void ClearProperties();

    //! Sets the property grid to be the block of a specific timeline handle.
    //! Ideally, this function should live somewhere else.
    void SetCurrentTimelineBlock(AssetInstanceHandle timelineHandle, unsigned int blockGuid, const QString& title);

    //! Call when you want this property to lose references, only when the app closes.
    void Clear();

    //@{
    //! Gets the corresponding property value from the property structure passed in.
    bool     GetBoolProperty(QtProperty * property);
    unsigned GetUintProperty(QtProperty * property);
    int      GetIntProperty(QtProperty * property);
    float    GetFloatProperty(QtProperty * property);
    Vec2Property     GetVec2Property(QtProperty * property);
    Vec3Property     GetVec3Property(QtProperty * property);
    Vec4Property     GetVec4Property(QtProperty * property);
    QString    GetS64Property(QtProperty * property);
    QColor     GetRgbProperty(QtProperty * property);
    QColor     GetRgbaProperty(QtProperty * property);
    //@}

signals:
    void OnPropertyUpdated(QtProperty* property);

private slots:

    //! Called when an item of the property browser has its value changed
    void boolPropertyChanged(QtProperty * property);
    void uintPropertyChanged(QtProperty * property);
    void intPropertyChanged(QtProperty * property);
    void floatPropertyChanged(QtProperty * property);
    void vec2PropertyChanged(QtProperty * property);
    void vec3PropertyChanged(QtProperty * property);
    void vec4PropertyChanged(QtProperty * property);
    void rgbPropertyChanged(QtProperty * property);
    void rgbaPropertyChanged(QtProperty * property);
    void s64PropertyChanged(QtProperty * property);
    void enumPropertyChanged(QtProperty * property);
    

private:

    void OnInitialized(PropertyGridHandle handle, QString title, const Pegasus::PropertyGrid::IPropertyGridObjectProxy* objectProxy);
    void OnUpdated(PropertyGridHandle handle, const QVector<PropertyGridIOMCUpdateElement>& els);

    const Pegasus::PropertyGrid::PropertyRecord * FindPropertyRecord(const QtProperty * property, unsigned int & outIndex) const;

    bool IsReady() const { return !mIsInitializing && mProxyHandle != INVALID_PGRID_HANDLE; }

    void UpdateProxy(const PropertyGridIOMCUpdateElement& el);

    //! Send an open message to the IO controller
    //! \param proxy Proxy assigned to the widget
    void SendOpenMessage(Pegasus::PropertyGrid::IPropertyGridObjectProxy * proxy, const QString& title);

    //! Send a close message to the IO controller if there is a handle defined
    void SendCloseMessage();

    // Loaded application proxy, nullptr otherwise
    Pegasus::App::IApplicationProxy * mApplicationProxy;

    // Widget showing a set of properties
    QtTreePropertyBrowser * mBrowser;

    // Group manager, to create sections per class in the tree property browser
    QtGroupPropertyManager m_groupManager;

    // dock widget that sends messages.
    PegasusDockWidget* mMessenger;

    //! Handle returned in OnInitialized from observer, which represents the current propertyGrid
    PropertyGridHandle mProxyHandle;

    //! Pair containing a property / record pair
    struct PropertyRecordPair
    {
        const Pegasus::PropertyGrid::PropertyRecord * mRecord;
        QtProperty * mProperty;
    };

    //! List of properties, follows same order as schema (index are consistent)
    QVector<PropertyRecordPair> mProperties[Pegasus::PropertyGrid::NUM_PROPERTY_CATEGORIES];

    //! Observer of this widget, used to communicate with the IO controller.
    PropertyGridObserver * mObserver;

    // List of property managers, one per property type
    PropertyGridBoolPropertyManager mBoolManager;
    PropertyGridUIntPropertyManager mUIntManager;
    PropertyGridIntPropertyManager mIntManager;
    PropertyGridFloatPropertyManager mFloatManager;
    PropertyGridVec2PropertyManager mVec2Manager;
    PropertyGridVec3PropertyManager mVec3Manager;
    PropertyGridVec4PropertyManager mVec4Manager;
    PropertyGridColor8RGBPropertyManager mColor8RGBManager;
    PropertyGridColor8RGBAPropertyManager mColor8RGBAManager;
    PropertyGridString64PropertyManager mString64Manager;
    PropertyGridEnumPropertyManager mEnumManager;
    
    // List of editor factories, one per property editor type
    PropertyGridBoolEditorFactory mBoolEditorFactory;
    PropertyGridUIntEditorFactory mUIntEditorFactory;
    PropertyGridIntEditorFactory mIntEditorFactory;
    PropertyGridFloatEditorFactory mFloatEditorFactory;
    PropertyGridColor8RGBEditorFactory mColor8RGBEditorFactory;
    PropertyGridColor8RGBAEditorFactory mColor8RGBAEditorFactory;
    PropertyGridString64EditorFactory mString64EditorFactory;
    PropertyGridEnumEditorFactory mEnumEditorFactory;

    // title of the property grid widget
    QLabel* mTitle;

    //bool used to prevent massive onPropertyChanged messages when the block is initializing
    bool mIsInitializing;
};


#endif // EDITOR_PROPERTYGRIDWIDGET_H
