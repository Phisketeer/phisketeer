/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
*/
#ifndef PHIUNDO_H
#define PHIUNDO_H

#include <QUndoCommand>
#include <QList>
//#include "phidatasources.h"
#include "phipalette.h"
#include "phi.h"

class PHIBaseItem;
class PHIGraphicsScene;

class PHIUndoCommand : public QUndoCommand
{

public:
    enum Id { Move=0, Color=1, Size, Add, Delete, Opacity, Font, Title, Visibility,
        Line, PenWidth, Pattern, Alignment, Text, ZIndex };
        //Data, Pixmap, TransformPos, ZValue,
        //Transform, Value, Label, ItemId, Parent, Page, TabOrder, Check, ToolTip, Visibility,
        //Effect, TextData, Url, StyleSheet, Attribute };

protected:
    PHIUndoCommand( PHIBaseItem *it );
    inline PHIBaseItem* item() const { return _it; }
    inline void setItem( PHIBaseItem *it ) { _it=it; }

private:
    PHIBaseItem *_it;
};

class PHIUndoMove : public PHIUndoCommand
{
public:
    PHIUndoMove( PHIBaseItem* it, const QPointF &oldPos );
    inline virtual int id() const { return static_cast<int>(Move); }
    virtual bool mergeWith( const QUndoCommand *other );
    virtual void undo();
    virtual void redo();

protected:
    QPointF _newPos, _oldPos;
};

class PHIUndoColor : public PHIUndoCommand
{
public:
    PHIUndoColor( PHIBaseItem* it, PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &newCol );
    inline virtual int id() const { return static_cast<int>(Color); }
    virtual bool mergeWith( const QUndoCommand *other );
    virtual void undo();
    virtual void redo();

protected:
    PHIPalette::ItemRole _itemRole;
    PHIPalette::ColorRole _newColorRole, _oldColorRole;
    QColor _newCol, _oldCol;
};

class PHIUndoAdd : public PHIUndoCommand
{
public:
    PHIUndoAdd( PHIBaseItem *it, PHIGraphicsScene *scene );
    virtual ~PHIUndoAdd();
    inline virtual int id() const { return static_cast<int>(Add); }
    virtual void undo();
    virtual void redo();

protected:
    PHIGraphicsScene *_scene;
};

class PHIUndoDelete : public PHIUndoCommand
{
public:
    PHIUndoDelete( PHIBaseItem *it , PHIGraphicsScene *scene );
    inline virtual int id() const { return static_cast<int>(Delete); }
    virtual void undo();
    virtual void redo();

protected:
    PHIGraphicsScene *_scene;
};

class PHIUndoSize : public PHIUndoCommand
{
public:
    PHIUndoSize( PHIBaseItem*, const QSizeF &oldSize );
    inline virtual int id() const { return static_cast<int>(Size); }
    virtual bool mergeWith( const QUndoCommand *other );
    virtual void undo();
    virtual void redo();

protected:
    QSizeF _oldSize, _newSize;
};

class PHIUndoProperty : public PHIUndoCommand
{
public:
    PHIUndoProperty( PHIBaseItem*, PHIUndoCommand::Id, const QVariant &newProp );
    PHIUndoProperty( PHIBaseItem*, const char *property, const QVariant &newProp );
    inline virtual int id() const { return static_cast<int>(_id); }
    virtual bool mergeWith( const QUndoCommand *other );
    virtual void undo();
    virtual void redo();

protected:
    void createText();
    PHIUndoCommand::Id _id;
    QVariant _oldProp, _newProp;
};

/*
class PHIUndoTransform : public PHIUndoCommand
{
public:
    PHIUndoTransform( PHIBaseItem*, double x, double y, double z, double dx, double dy, double sh, double sv );
    virtual ~PHIUndoTransform();
    inline virtual int id() const { return static_cast<int>(Transform); }
    virtual bool mergeWith( const QUndoCommand *other );
    virtual void undo();
    virtual void redo();

protected:
    double _newX, _oldX;
    double _newY, _oldY;
    double _newZ, _oldZ;
    double _newDX, _oldDX;
    double _newDY, _oldDY;
    double _newSH, _oldSH;
    double _newSV, _oldSV;
};

class PHIUndoUrl : public PHIUndoCommand
{
public:
    PHIUndoUrl( PHIBaseItem*, const QString &oldUrl, const QString &newUrl );
    virtual ~PHIUndoUrl();
    inline virtual int id() const { return static_cast<int>(Url); }
    virtual bool mergeWith( const QUndoCommand *other );
    virtual void undo();
    virtual void redo();

protected:
    QString _oldUrl, _newUrl;
};

class PHIUndoPage : public PHIUndoCommand
{
public:
    PHIUndoPage( PHIScene *scene, const PHISPage &newPage );
    virtual ~PHIUndoPage();
    inline virtual int id() const { return static_cast<int>(Page); }
    virtual bool mergeWith( const QUndoCommand *other );
    virtual void redo();
    virtual void undo();

protected:
    PHIScene *_scene;
    PHISPage _oldPage;
    PHISPage _newPage;
};

class PHIUndoEffect : public PHIUndoCommand
{
public:
    PHIUndoEffect( PHIBaseItem *it, const PHIEffect &newEffect );
    virtual ~PHIUndoEffect();
    inline virtual int id() const { return static_cast<int>(Effect); }
    virtual bool mergeWith( const QUndoCommand *other );
    virtual void redo();
    virtual void undo();

protected:
    PHIEffect _oldEffect;
    PHIEffect _newEffect;
};

class PHIUndoStyleSheet : public PHIUndoCommand
{
public:
    PHIUndoStyleSheet( PHIBaseItem *it, const PHITextData &css );
    virtual ~PHIUndoStyleSheet();
    inline virtual int id() const { return static_cast<int>(StyleSheet); }
    virtual bool mergeWith( const QUndoCommand *other );
    virtual void redo();
    virtual void undo();

protected:
    PHITextData _oldCss;
    PHITextData _newCss;
};

class PHIUndoTextData : public PHIUndoCommand
{
public:
    PHIUndoTextData( PHIBaseItem *it, const PHITextData &newData );
    virtual ~PHIUndoTextData();
    inline virtual int id() const { return static_cast<int>(TextData); }
    virtual bool mergeWith( const QUndoCommand *other );
    virtual void redo();
    virtual void undo();

protected:
    PHITextData _oldText;
    PHITextData _newText;
};
*/

#endif // PHIUNDO_H
