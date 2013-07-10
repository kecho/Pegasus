/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	Settings.h
//! \author	Kevin Boulanger
//! \date	10th July 2013
//! \brief	Settings for the entire editor

#ifndef EDITOR_SETTINGS_H
#define EDITOR_SETTINGS_H

#include <QString>
#include <QStringList>
#include <QPalette>


//! Settings for the entire editor
class Settings : public QObject
{
    Q_OBJECT

public:

    //! Constructor reading the parameters from the operating system,
    //! setting the default values for the unknown parameters
    //! and applying the values to the application
    Settings();

    //! Destructor
    ~Settings();

    //! Save the settings into a configuration file
    //! \note The function does not fail. If saving of the settings fails, nothing happens
    void Save();

    //------------------------------------------------------------------------------------

public:

    // Appearance settings

    //! Get the list of possible widget style names
    //! Get the list of possible widget style names
    const QStringList & GetWidgetStyleNameList() const;

    //! Get the name of the widget style
    //! \return Name of the widget style ("Windows", "CDE", "Plastique", etc. for example)
    const QString & GetWidgetStyleName() const;

    //! Test whether the widget style uses the style palette
    //! \return True if the widget style uses the style palette,
    //!         false if it uses the original palette
    bool IsUsingWidgetStylePalette() const;

    //! Get the background color of the console
    //! \return Background color of the console
    //! \todo Remove?
    const QColor & GetConsoleBackgroundColor() const;

    //! Get the text color of the console
    //! \return Text color of the console
    //! \todo Remove?
    const QColor & GetConsoleTextColor() const;

public slots:

    //! Set the name of the widget style
    //! \note If the input name is invalid, no style change occurs
    //! \param name Name of the widget style (one of the style names
    //!             returned by \a GetWidgetStyleNameList)
    void SetWidgetStyleName(const QString & name);

    //! Set the palette to use for the widget style
    //! \param stylePalette true to use the style palette,
    //!                     false to use the original palette
    void SetUseWidgetStylePalette(bool stylePalette);

    //! Set the background color of the console
    //! \param color Background color of the console
    //! \todo Remove?
    void SetConsoleBackgroundColor(const QColor & color);

    //! Set the text color of the console
    //! \param color Text color of the console
    //! \todo Remove?
    void SetConsoleTextColor(const QColor & color);

    //------------------------------------------------------------------------------------

public:

    // Default value for some of the settings

    //! Get the default background color of the console
    //! \return Default background color of the console
    QColor GetConsoleDefaultBackgroundColor() const;

    //! Get the default text color of the console
    //! \return Default text color of the console
    QColor GetConsoleDefaultTextColor() const;

    //------------------------------------------------------------------------------------

private:

    // Appearance settings

    //! Name of the widget style ("Windows", "CDE", "Plastique", etc. for example)
    QString mWidgetStyleName;

    //! True if the widget style uses the style palette,
    //! false if it uses the original palette
    bool mUseWidgetStylePalette;

    //! Background color of the console
    //! \todo Remove?
    QColor mConsoleBackgroundColor;

    //! Text color of the console
    //! \todo Remove?
    QColor mConsoleTextColor;

    //------------------------------------------------------------------------------------

private:

    // Internal variables

    //! List of possible widget style names
    QStringList mWidgetStyleNameList;

    //! Original palette of the application
    QPalette mOriginalPalette;
};


#endif  // EDITOR_SETTINGS_H
