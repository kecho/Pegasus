/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	Settings.cpp
//! \author	Kevin Boulanger
//! \date	10th July 2013
//! \brief	Settings for the entire editor

#include "Settings.h"
#include <QStyleFactory>
#include <QStyle>
#include <QSettings>
#include <QApplication>


Settings::Settings()
:   QObject()
{
    // Set internal variables
    mWidgetStyleNameList = QStyleFactory::keys();
    //! \todo Assert with !mWidgetStyleNameList.isEmpty()
    mOriginalPalette = QApplication::palette();

    // Create the QSettings object that will read the application settings from file
    //! \todo Put the filename in a constant, as it is used also for loading
    QSettings settings("Editor.ini", QSettings::IniFormat);

    //! \todo Write separate functions rather than a large constructor

    // Widget style
    //! \todo Select the proper initial style in the combo box
    /*if (widgetStyleNameList.contains("WindowsVista"))
    widgetStyleName = "WindowsVista";
    else if (widgetStyleNameList.contains("WindowsXP"))
    widgetStyleName = "WindowsXP";
    else if (widgetStyleNameList.contains("Macintosh"))
    widgetStyleName = "Macintosh";
    else if (widgetStyleNameList.contains("Cleanlooks"))
    widgetStyleName = "Cleanlooks";
    else if (widgetStyleNameList.contains("Plastique"))
    widgetStyleName = "Plastique";
    else widgetStyleName = */mWidgetStyleNameList[0];
    SetWidgetStyleName(settings.value("Appearance/WidgetStyle",
                                      mWidgetStyleName).toString());

    // Widget style palette
    mUseWidgetStylePalette = true;
    SetUseWidgetStylePalette(settings.value("Appearance/UseStylePalette",
                                            mUseWidgetStylePalette).toBool());

    // Console colors
    mConsoleBackgroundColor = GetConsoleDefaultBackgroundColor();
    mConsoleTextColor = GetConsoleDefaultTextColor();
    SetConsoleBackgroundColor(settings.value("Appearance/ConsoleBackgroundColor",
                                             mConsoleBackgroundColor).value<QColor>());
    SetConsoleTextColor(settings.value("Appearance/ConsoleTextColor",
                                       mConsoleTextColor).value<QColor>());
}

//----------------------------------------------------------------------------------------

Settings::~Settings()
{
}
//----------------------------------------------------------------------------------------

void Settings::Save()
{
    // Create the QSettings object that will save the application settings
    QSettings settings("Editor.ini", QSettings::IniFormat);

    //! \todo Use settings items strings common for loading and saving

    // Widget style
    settings.setValue("Appearance/WidgetStyle", mWidgetStyleName);
    settings.setValue("Appearance/UseStylePalette", mUseWidgetStylePalette);

    // Neptune 3D console colors
    settings.setValue("Appearance/ConsoleBackgroundColor", mConsoleBackgroundColor);
    settings.setValue("Appearance/ConsoleTextColor", mConsoleTextColor);
}

//----------------------------------------------------------------------------------------

const QStringList & Settings::GetWidgetStyleNameList() const
{
    return mWidgetStyleNameList;
}

//----------------------------------------------------------------------------------------

const QString & Settings::GetWidgetStyleName() const
{
    return mWidgetStyleName;
}

//----------------------------------------------------------------------------------------

const QColor & Settings::GetConsoleBackgroundColor() const
{
    return mConsoleBackgroundColor;
}

//----------------------------------------------------------------------------------------

const QColor & Settings::GetConsoleTextColor() const
{
    return mConsoleTextColor;
}

//----------------------------------------------------------------------------------------
	
bool Settings::IsUsingWidgetStylePalette() const
{
    return mUseWidgetStylePalette;
}

//----------------------------------------------------------------------------------------

void Settings::SetWidgetStyleName(const QString & name)
{
    if (mWidgetStyleNameList.contains(name))
    {
        // Apply the new widget style
        mWidgetStyleName = name;
        QApplication::setStyle(QStyleFactory::create(mWidgetStyleName));

        // Set the palette of the new widget style
        SetUseWidgetStylePalette(mUseWidgetStylePalette);
    }
}

//----------------------------------------------------------------------------------------

void Settings::SetUseWidgetStylePalette(bool stylePalette)
{
    mUseWidgetStylePalette = stylePalette;

    if (mUseWidgetStylePalette)
    {
        QApplication::setPalette(QApplication::style()->standardPalette());
    }
    else
    {
        QApplication::setPalette(mOriginalPalette);
    }
}

//----------------------------------------------------------------------------------------

void Settings::SetConsoleBackgroundColor(const QColor & color)
{
    mConsoleBackgroundColor = color;
    //! \todo Update the console color
    //if (ED_MainWindow::GetN3DConsoleDock())
    //{
    //    ED_MainWindow::GetN3DConsoleDock()->GetConsole()->SetBackgroundColor(mConsoleBackgroundColor);
    //}
}

//----------------------------------------------------------------------------------------

void Settings::SetConsoleTextColor(const QColor & color)
{
    mConsoleTextColor = color;
    //! \todo Update the console color
    //if (ED_MainWindow::GetN3DConsoleDock())
    //{
    //    ED_MainWindow::GetN3DConsoleDock()->GetConsole()->SetTextColor(mConsoleTextColor);
    //}
}

//----------------------------------------------------------------------------------------

QColor Settings::GetConsoleDefaultBackgroundColor() const
{
    return Qt::darkBlue;
}

//----------------------------------------------------------------------------------------
	
QColor Settings::GetConsoleDefaultTextColor() const
{
    return Qt::yellow;
}