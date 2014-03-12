/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHISLOGVIEWER_H
#define PHISLOGVIEWER_H

#include <QWidget>

class QPlainTextEdit;

class PHISLogViewer : public QWidget
{
    Q_OBJECT

public:
    static PHISLogViewer* instance();
    virtual ~PHISLogViewer();

protected:
    explicit PHISLogViewer( QWidget *parent=0 );

signals:

private slots:
    void refresh();

private:
    static PHISLogViewer *_instance;
    QPlainTextEdit *_edit;
    QString _filename;
    qint64 _lastSize;
};

#endif // PHISLOGVIEWER_H
