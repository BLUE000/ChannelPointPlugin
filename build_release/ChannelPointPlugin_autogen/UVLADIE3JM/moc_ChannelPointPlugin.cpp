/****************************************************************************
** Meta object code from reading C++ file 'ChannelPointPlugin.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/ChannelPointPlugin.h"
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ChannelPointPlugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN18ChannelPointPluginE_t {};
} // unnamed namespace

template <> constexpr inline auto ChannelPointPlugin::qt_create_metaobjectdata<qt_meta_tag_ZN18ChannelPointPluginE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ChannelPointPlugin",
        "onTestPlayRequested",
        "",
        "rewardId",
        "onEmergencyStopTriggered"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onTestPlayRequested'
        QtMocHelpers::SlotData<void(const QString &)>(1, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Slot 'onEmergencyStopTriggered'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ChannelPointPlugin, qt_meta_tag_ZN18ChannelPointPluginE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ChannelPointPlugin::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18ChannelPointPluginE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18ChannelPointPluginE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN18ChannelPointPluginE_t>.metaTypes,
    nullptr
} };

void ChannelPointPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ChannelPointPlugin *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onTestPlayRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->onEmergencyStopTriggered(); break;
        default: ;
        }
    }
}

const QMetaObject *ChannelPointPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ChannelPointPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18ChannelPointPluginE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "IChannelPlugin"))
        return static_cast< IChannelPlugin*>(this);
    if (!strcmp(_clname, "com.blue000.twitchchannelmanagementtool.IChannelPlugin"))
        return static_cast< IChannelPlugin*>(this);
    return QObject::qt_metacast(_clname);
}

int ChannelPointPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

#ifdef QT_MOC_EXPORT_PLUGIN_V2
static constexpr unsigned char qt_pluginMetaDataV2_ChannelPointPlugin[] = {
    0xbf, 
    // "IID"
    0x02,  0x78,  0x1e,  'c',  'o',  'm',  '.',  'b', 
    'l',  'u',  'e',  '0',  '0',  '0',  '.',  'C', 
    'h',  'a',  'n',  'n',  'e',  'l',  'P',  'o', 
    'i',  'n',  't',  'P',  'l',  'u',  'g',  'i', 
    'n', 
    // "className"
    0x03,  0x72,  'C',  'h',  'a',  'n',  'n',  'e', 
    'l',  'P',  'o',  'i',  'n',  't',  'P',  'l', 
    'u',  'g',  'i',  'n', 
    // "MetaData"
    0x04,  0xa1,  0x64,  'K',  'e',  'y',  's',  0x81, 
    0x78,  0x1e,  'c',  'o',  'm',  '.',  'b',  'l', 
    'u',  'e',  '0',  '0',  '0',  '.',  'C',  'h', 
    'a',  'n',  'n',  'e',  'l',  'P',  'o',  'i', 
    'n',  't',  'P',  'l',  'u',  'g',  'i',  'n', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN_V2(ChannelPointPlugin, ChannelPointPlugin, qt_pluginMetaDataV2_ChannelPointPlugin)
#else
QT_PLUGIN_METADATA_SECTION
Q_CONSTINIT static constexpr unsigned char qt_pluginMetaData_ChannelPointPlugin[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', '!',
    // metadata version, Qt version, architectural requirements
    0, QT_VERSION_MAJOR, QT_VERSION_MINOR, qPluginArchRequirements(),
    0xbf, 
    // "IID"
    0x02,  0x78,  0x1e,  'c',  'o',  'm',  '.',  'b', 
    'l',  'u',  'e',  '0',  '0',  '0',  '.',  'C', 
    'h',  'a',  'n',  'n',  'e',  'l',  'P',  'o', 
    'i',  'n',  't',  'P',  'l',  'u',  'g',  'i', 
    'n', 
    // "className"
    0x03,  0x72,  'C',  'h',  'a',  'n',  'n',  'e', 
    'l',  'P',  'o',  'i',  'n',  't',  'P',  'l', 
    'u',  'g',  'i',  'n', 
    // "MetaData"
    0x04,  0xa1,  0x64,  'K',  'e',  'y',  's',  0x81, 
    0x78,  0x1e,  'c',  'o',  'm',  '.',  'b',  'l', 
    'u',  'e',  '0',  '0',  '0',  '.',  'C',  'h', 
    'a',  'n',  'n',  'e',  'l',  'P',  'o',  'i', 
    'n',  't',  'P',  'l',  'u',  'g',  'i',  'n', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN(ChannelPointPlugin, ChannelPointPlugin)
#endif  // QT_MOC_EXPORT_PLUGIN_V2

QT_WARNING_POP
