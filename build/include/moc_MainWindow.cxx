/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.hh'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/MainWindow.hh"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_gazebo__GUIWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x09,
      40,   18,   18,   18, 0x09,
      59,   18,   18,   18, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_gazebo__GUIWindow[] = {
    "gazebo::GUIWindow\0\0OnPauseButtonClick()\0"
    "OnOpenWorldClick()\0OnLogButtonClick()\0"
};

void gazebo::GUIWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GUIWindow *_t = static_cast<GUIWindow *>(_o);
        switch (_id) {
        case 0: _t->OnPauseButtonClick(); break;
        case 1: _t->OnOpenWorldClick(); break;
        case 2: _t->OnLogButtonClick(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData gazebo::GUIWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject gazebo::GUIWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_gazebo__GUIWindow,
      qt_meta_data_gazebo__GUIWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &gazebo::GUIWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *gazebo::GUIWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *gazebo::GUIWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_gazebo__GUIWindow))
        return static_cast<void*>(const_cast< GUIWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int gazebo::GUIWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
