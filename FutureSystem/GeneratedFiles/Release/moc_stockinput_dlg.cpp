/****************************************************************************
** Meta object code from reading C++ file 'stockinput_dlg.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../stockinput_dlg.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'stockinput_dlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_StockInputDlg_t {
    QByteArrayData data[8];
    char stringdata[100];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_StockInputDlg_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_StockInputDlg_t qt_meta_stringdata_StockInputDlg = {
    {
QT_MOC_LITERAL(0, 0, 13),
QT_MOC_LITERAL(1, 14, 26),
QT_MOC_LITERAL(2, 41, 0),
QT_MOC_LITERAL(3, 42, 3),
QT_MOC_LITERAL(4, 46, 19),
QT_MOC_LITERAL(5, 66, 5),
QT_MOC_LITERAL(6, 72, 21),
QT_MOC_LITERAL(7, 94, 4)
    },
    "StockInputDlg\0FlushFromStationListWidget\0"
    "\0str\0OnClickedListWidget\0index\0"
    "ChangeFromStationText\0text\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_StockInputDlg[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x08,
       4,    1,   32,    2, 0x08,
       6,    1,   35,    2, 0x08,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QModelIndex,    5,
    QMetaType::Void, QMetaType::QString,    7,

       0        // eod
};

void StockInputDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        StockInputDlg *_t = static_cast<StockInputDlg *>(_o);
        switch (_id) {
        case 0: _t->FlushFromStationListWidget((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->OnClickedListWidget((*reinterpret_cast< QModelIndex(*)>(_a[1]))); break;
        case 2: _t->ChangeFromStationText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject StockInputDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_StockInputDlg.data,
      qt_meta_data_StockInputDlg,  qt_static_metacall, 0, 0}
};


const QMetaObject *StockInputDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *StockInputDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_StockInputDlg.stringdata))
        return static_cast<void*>(const_cast< StockInputDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int StockInputDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
