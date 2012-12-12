/****************************************************************************
** 
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
** 
** This file is part of a Qt Solutions component.
**
** Commercial Usage  
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
** 
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
** 
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
** 
** GNU General Public License Usage 
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** 
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
** 
** If you are unsure which license is appropriate for your use, please
** contact Nokia at qt-info@nokia.com.
** 
****************************************************************************/

#include <QtGui>
#include <qtbrowserplugin.h>

class Trivial : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)

    Q_CLASSINFO("ClassID", "{5a22176d-118f-4185-9653-9f98958a6df8}")
    Q_CLASSINFO("InterfaceID", "{2df735ba-da4f-4fb7-8f35-b8dfbf8cfd9a}")
    Q_CLASSINFO("EventsID", "{449de213-f8bd-4d2e-a2cf-eab407c03245}")

    Q_CLASSINFO("MIME", "trivial/very:xxx:Trivial and useless")
    Q_CLASSINFO("ToSuperClass", "Trivial")
    Q_CLASSINFO("DefaultProperty", "text")

public:
    Trivial(QWidget *parent = 0)
        : QWidget(parent)
    {
        m_text = QString::fromLatin1("Empty");
    }

    void mousePressEvent(QMouseEvent *)
    {
        mouseDown();
    }
    void mouseMoveEvent(QMouseEvent *e)
    {
        mouseMove(e->x(), e->y());
    }
    void mouseReleaseEvent(QMouseEvent*)
    {
        mouseUp();
    }
    QString text() const
    {
        return m_text;
    }

public slots:
    void about()
    {
        QMessageBox::aboutQt(this);
    }
    void setText(const QString &text)
    {
        m_text = text;
        update();
    }

signals:
    void mouseDown();
    void mouseMove(int x, int y);
    void mouseUp();

protected:
    void paintEvent(QPaintEvent*)
    {
        QPainter p(this);
        QRect r(rect());
        r.adjust(0, 0, -1, -1);

        p.drawRect(r);
        p.drawText(r, Qt::AlignCenter, m_text);
    }

private:
    QString m_text;
};

#include "trivial.moc"

QTNPFACTORY_BEGIN("Trivial Qt-based Plugin", "A Qt-based LiveConnected plug-in that does nothing")
    QTNPCLASS(Trivial)
QTNPFACTORY_END()

#ifdef QAXSERVER
#include <ActiveQt/QAxFactory>
QAXFACTORY_BEGIN("{aa3216bf-7e20-482c-84c6-06167bacb616}", "{08538ca5-eb7a-4f24-a3c4-a120c6e04dc4}")
    QAXCLASS(Trivial)
QAXFACTORY_END()
#endif
