/****************************************************************************
** Meta object code from reading C++ file 'DownloaderGUI.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "DownloaderGUI.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DownloaderGUI.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DownloadWorker_t {
    QByteArrayData data[12];
    char stringdata0[125];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DownloadWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DownloadWorker_t qt_meta_stringdata_DownloadWorker = {
    {
QT_MOC_LITERAL(0, 0, 14), // "DownloadWorker"
QT_MOC_LITERAL(1, 15, 16), // "downloadProgress"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 10), // "percentage"
QT_MOC_LITERAL(4, 44, 10), // "downloaded"
QT_MOC_LITERAL(5, 55, 5), // "total"
QT_MOC_LITERAL(6, 61, 5), // "speed"
QT_MOC_LITERAL(7, 67, 16), // "downloadFinished"
QT_MOC_LITERAL(8, 84, 7), // "success"
QT_MOC_LITERAL(9, 92, 7), // "message"
QT_MOC_LITERAL(10, 100, 10), // "logMessage"
QT_MOC_LITERAL(11, 111, 13) // "startDownload"

    },
    "DownloadWorker\0downloadProgress\0\0"
    "percentage\0downloaded\0total\0speed\0"
    "downloadFinished\0success\0message\0"
    "logMessage\0startDownload"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DownloadWorker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,   34,    2, 0x06 /* Public */,
       7,    2,   43,    2, 0x06 /* Public */,
      10,    1,   48,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,   51,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::LongLong, QMetaType::LongLong, QMetaType::Double,    3,    4,    5,    6,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    8,    9,
    QMetaType::Void, QMetaType::QString,    9,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void DownloadWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DownloadWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->downloadProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2])),(*reinterpret_cast< qint64(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 1: _t->downloadFinished((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->logMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->startDownload(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DownloadWorker::*)(int , qint64 , qint64 , double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DownloadWorker::downloadProgress)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DownloadWorker::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DownloadWorker::downloadFinished)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DownloadWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DownloadWorker::logMessage)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DownloadWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_DownloadWorker.data,
    qt_meta_data_DownloadWorker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DownloadWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DownloadWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DownloadWorker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DownloadWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void DownloadWorker::downloadProgress(int _t1, qint64 _t2, qint64 _t3, double _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DownloadWorker::downloadFinished(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DownloadWorker::logMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
struct qt_meta_stringdata_DownloaderGUI_t {
    QByteArrayData data[16];
    char stringdata0[196];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DownloaderGUI_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DownloaderGUI_t qt_meta_stringdata_DownloaderGUI = {
    {
QT_MOC_LITERAL(0, 0, 13), // "DownloaderGUI"
QT_MOC_LITERAL(1, 14, 15), // "onBrowseClicked"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 17), // "onDownloadClicked"
QT_MOC_LITERAL(4, 49, 15), // "onCancelClicked"
QT_MOC_LITERAL(5, 65, 17), // "onClearLogClicked"
QT_MOC_LITERAL(6, 83, 18), // "onDownloadProgress"
QT_MOC_LITERAL(7, 102, 10), // "percentage"
QT_MOC_LITERAL(8, 113, 10), // "downloaded"
QT_MOC_LITERAL(9, 124, 5), // "total"
QT_MOC_LITERAL(10, 130, 5), // "speed"
QT_MOC_LITERAL(11, 136, 18), // "onDownloadFinished"
QT_MOC_LITERAL(12, 155, 7), // "success"
QT_MOC_LITERAL(13, 163, 7), // "message"
QT_MOC_LITERAL(14, 171, 12), // "onLogMessage"
QT_MOC_LITERAL(15, 184, 11) // "updateTimer"

    },
    "DownloaderGUI\0onBrowseClicked\0\0"
    "onDownloadClicked\0onCancelClicked\0"
    "onClearLogClicked\0onDownloadProgress\0"
    "percentage\0downloaded\0total\0speed\0"
    "onDownloadFinished\0success\0message\0"
    "onLogMessage\0updateTimer"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DownloaderGUI[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x08 /* Private */,
       3,    0,   55,    2, 0x08 /* Private */,
       4,    0,   56,    2, 0x08 /* Private */,
       5,    0,   57,    2, 0x08 /* Private */,
       6,    4,   58,    2, 0x08 /* Private */,
      11,    2,   67,    2, 0x08 /* Private */,
      14,    1,   72,    2, 0x08 /* Private */,
      15,    0,   75,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::LongLong, QMetaType::LongLong, QMetaType::Double,    7,    8,    9,   10,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   12,   13,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void,

       0        // eod
};

void DownloaderGUI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DownloaderGUI *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onBrowseClicked(); break;
        case 1: _t->onDownloadClicked(); break;
        case 2: _t->onCancelClicked(); break;
        case 3: _t->onClearLogClicked(); break;
        case 4: _t->onDownloadProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2])),(*reinterpret_cast< qint64(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 5: _t->onDownloadFinished((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 6: _t->onLogMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->updateTimer(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DownloaderGUI::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_DownloaderGUI.data,
    qt_meta_data_DownloaderGUI,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DownloaderGUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DownloaderGUI::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DownloaderGUI.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int DownloaderGUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
