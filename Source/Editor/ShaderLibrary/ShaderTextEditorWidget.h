/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ShaderTextEditorWidget.h
//! \author	Kleber Garcia
//! \date	22nd April 2014
//! \brief	Graphics widget representing a single shader text editor

#ifndef EDITOR_SHADERTEXTEDITORWIDGET_H
#define EDITOR_SHADERTEXTEDITORWIDGET_H
#include <QTextEdit>

namespace Pegasus {
    namespace Shader {
        class IShaderProxy;
    }
}
 
class QEvent;
class QSyntaxHighlighter;

//! text editor widget class. Represents the text editor widget
class ShaderTextEditorWidget : public QTextEdit
{
    Q_OBJECT;
public:
    //! constructor
    ShaderTextEditorWidget();

    //! destructor
    virtual ~ShaderTextEditorWidget();

    //! initialization function. Call whenever you want a shader to be opened
    void Initialize(Pegasus::Shader::IShaderProxy * shader); 

    //! function that clears the state of the widget
    void Uninitialize(); 

    //! gets the shader being used
    //! \return the shader assigned
    Pegasus::Shader::IShaderProxy * GetShader() { return mShader; }

    //! updates the syntax of a single line in the document.
    //! \param the line to update. If the line is out of range nothing happens
    void UpdateLineSyntax(int line);

    //! forces a full syntax update on the document
    //! \warning this function is very expensive
    void UpdateAllDocumentSyntax();

    //! flushes the text container to the shader internals
    void FlushTextToShader();

protected:
    //! event callback
    virtual bool event(QEvent * e);

private:
    //! shader reference
    Pegasus::Shader::IShaderProxy * mShader;

    QSyntaxHighlighter * mSyntaxHighlighter;
};


#endif