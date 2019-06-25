/****************************************************************************
** Meta object code from reading C++ file 'mock_trade_dlg.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../mock_trade_dlg.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mock_trade_dlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MockTradeDlg_t {
    QByteArrayData data[7];
    char stringdata[91];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_MockTradeDlg_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_MockTradeDlg_t qt_meta_stringdata_MockTradeDlg = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 15),
QT_MOC_LITERAL(2, 29, 0),
QT_MOC_LITERAL(3, 30, 12),
QT_MOC_LITERAL(4, 43, 11),
QT_MOC_LITERAL(5, 55, 17),
QT_MOC_LITERAL(6, 73, 16)
    },
    "MockTradeDlg\0slotHandleQuote\0\0"
    "slotOpenSell\0slotOpenBuy\0slotPositionClose\0"
    "slotBtnCondition\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MockTradeDlg[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    4,   39,    2, 0x0a,
       3,    0,   48,    2, 0x0a,
       4,    0,   49,    2, 0x0a,
       5,    0,   50,    2, 0x0a,
       6,    0,   51,    2, 0x0a,

 // slots: parameters
    QMetaType::Void, QMetaType::Double, QMetaType::Double, QMetaType::Int, QMetaType::Int,    2,    2,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MockTradeDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MockTradeDlg *_t = static_cast<MockTradeDlg *>(_o);
        switch (_id) {
        case 0: _t->slotHandleQuote((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 1: _t->slotOpenSell(); break;
        case 2: _t->slotOpenBuy(); break;
        case 3: _t->slotPositionClose(); break;
        case 4: _t->slotBtnCondition(); break;
        default: ;
        }
    }
}

const QMetaObject MockTradeDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MockTradeDlg.data,
      qt_meta_data_MockTradeDlg,  qt_static_metacall, 0, 0}
};


const QMetaObject *MockTradeDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MockTradeDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MockTradeDlg.stringdata))
        return static_cast<void*>(const_cast< MockTradeDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int MockTradeDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
