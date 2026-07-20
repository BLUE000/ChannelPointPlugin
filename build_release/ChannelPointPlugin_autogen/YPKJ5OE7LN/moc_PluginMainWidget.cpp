/****************************************************************************
** Meta object code from reading C++ file 'PluginMainWidget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/ui/PluginMainWidget.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PluginMainWidget.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN16PluginMainWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto PluginMainWidget::qt_create_metaobjectdata<qt_meta_tag_ZN16PluginMainWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PluginMainWidget",
        "testPlayRequested",
        "",
        "rewardId",
        "emergencyStopTriggered",
        "onRewardSelected",
        "index",
        "onAddNewRewardClicked",
        "onTwitchSyncClicked",
        "onSaveSettingClicked",
        "onDeleteRewardClicked",
        "onTestPlayClicked",
        "onMediaTypeChanged",
        "onPreviewPositionChanged",
        "newX",
        "newY",
        "onEmergencyStopClicked",
        "onSessionSelected",
        "onExportCsvClicked",
        "refreshRewardList",
        "refreshAnalyticsTab",
        "updateQueueStatus",
        "current",
        "max"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'testPlayRequested'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'emergencyStopTriggered'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onRewardSelected'
        QtMocHelpers::SlotData<void(int)>(5, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 6 },
        }}),
        // Slot 'onAddNewRewardClicked'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTwitchSyncClicked'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveSettingClicked'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeleteRewardClicked'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTestPlayClicked'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMediaTypeChanged'
        QtMocHelpers::SlotData<void(int)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 6 },
        }}),
        // Slot 'onPreviewPositionChanged'
        QtMocHelpers::SlotData<void(int, int)>(13, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 14 }, { QMetaType::Int, 15 },
        }}),
        // Slot 'onEmergencyStopClicked'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSessionSelected'
        QtMocHelpers::SlotData<void(int)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 6 },
        }}),
        // Slot 'onExportCsvClicked'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'refreshRewardList'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'refreshAnalyticsTab'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateQueueStatus'
        QtMocHelpers::SlotData<void(int, int)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 22 }, { QMetaType::Int, 23 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PluginMainWidget, qt_meta_tag_ZN16PluginMainWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PluginMainWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16PluginMainWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16PluginMainWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16PluginMainWidgetE_t>.metaTypes,
    nullptr
} };

void PluginMainWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PluginMainWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->testPlayRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->emergencyStopTriggered(); break;
        case 2: _t->onRewardSelected((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->onAddNewRewardClicked(); break;
        case 4: _t->onTwitchSyncClicked(); break;
        case 5: _t->onSaveSettingClicked(); break;
        case 6: _t->onDeleteRewardClicked(); break;
        case 7: _t->onTestPlayClicked(); break;
        case 8: _t->onMediaTypeChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->onPreviewPositionChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 10: _t->onEmergencyStopClicked(); break;
        case 11: _t->onSessionSelected((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 12: _t->onExportCsvClicked(); break;
        case 13: _t->refreshRewardList(); break;
        case 14: _t->refreshAnalyticsTab(); break;
        case 15: _t->updateQueueStatus((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (PluginMainWidget::*)(const QString & )>(_a, &PluginMainWidget::testPlayRequested, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (PluginMainWidget::*)()>(_a, &PluginMainWidget::emergencyStopTriggered, 1))
            return;
    }
}

const QMetaObject *PluginMainWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PluginMainWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16PluginMainWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int PluginMainWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void PluginMainWidget::testPlayRequested(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void PluginMainWidget::emergencyStopTriggered()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
