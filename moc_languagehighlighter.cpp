/****************************************************************************
** Meta object code from reading C++ file 'languagehighlighter.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/languagehighlighter.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'languagehighlighter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
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
struct qt_meta_tag_ZN23LanguageHighlighterBaseE_t {};
} // unnamed namespace

template <> constexpr inline auto LanguageHighlighterBase::qt_create_metaobjectdata<qt_meta_tag_ZN23LanguageHighlighterBaseE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "LanguageHighlighterBase"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<LanguageHighlighterBase, qt_meta_tag_ZN23LanguageHighlighterBaseE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject LanguageHighlighterBase::staticMetaObject = { {
    QMetaObject::SuperData::link<QSyntaxHighlighter::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23LanguageHighlighterBaseE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23LanguageHighlighterBaseE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN23LanguageHighlighterBaseE_t>.metaTypes,
    nullptr
} };

void LanguageHighlighterBase::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<LanguageHighlighterBase *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *LanguageHighlighterBase::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LanguageHighlighterBase::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23LanguageHighlighterBaseE_t>.strings))
        return static_cast<void*>(this);
    return QSyntaxHighlighter::qt_metacast(_clname);
}

int LanguageHighlighterBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSyntaxHighlighter::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN14CssHighlighterE_t {};
} // unnamed namespace

template <> constexpr inline auto CssHighlighter::qt_create_metaobjectdata<qt_meta_tag_ZN14CssHighlighterE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CssHighlighter"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CssHighlighter, qt_meta_tag_ZN14CssHighlighterE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CssHighlighter::staticMetaObject = { {
    QMetaObject::SuperData::link<LanguageHighlighterBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14CssHighlighterE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14CssHighlighterE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14CssHighlighterE_t>.metaTypes,
    nullptr
} };

void CssHighlighter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CssHighlighter *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *CssHighlighter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CssHighlighter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14CssHighlighterE_t>.strings))
        return static_cast<void*>(this);
    return LanguageHighlighterBase::qt_metacast(_clname);
}

int CssHighlighter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = LanguageHighlighterBase::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN15HtmlHighlighterE_t {};
} // unnamed namespace

template <> constexpr inline auto HtmlHighlighter::qt_create_metaobjectdata<qt_meta_tag_ZN15HtmlHighlighterE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "HtmlHighlighter"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<HtmlHighlighter, qt_meta_tag_ZN15HtmlHighlighterE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject HtmlHighlighter::staticMetaObject = { {
    QMetaObject::SuperData::link<LanguageHighlighterBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15HtmlHighlighterE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15HtmlHighlighterE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15HtmlHighlighterE_t>.metaTypes,
    nullptr
} };

void HtmlHighlighter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<HtmlHighlighter *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *HtmlHighlighter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HtmlHighlighter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15HtmlHighlighterE_t>.strings))
        return static_cast<void*>(this);
    return LanguageHighlighterBase::qt_metacast(_clname);
}

int HtmlHighlighter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = LanguageHighlighterBase::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN13JsHighlighterE_t {};
} // unnamed namespace

template <> constexpr inline auto JsHighlighter::qt_create_metaobjectdata<qt_meta_tag_ZN13JsHighlighterE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "JsHighlighter"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<JsHighlighter, qt_meta_tag_ZN13JsHighlighterE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject JsHighlighter::staticMetaObject = { {
    QMetaObject::SuperData::link<LanguageHighlighterBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13JsHighlighterE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13JsHighlighterE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13JsHighlighterE_t>.metaTypes,
    nullptr
} };

void JsHighlighter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<JsHighlighter *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *JsHighlighter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *JsHighlighter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13JsHighlighterE_t>.strings))
        return static_cast<void*>(this);
    return LanguageHighlighterBase::qt_metacast(_clname);
}

int JsHighlighter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = LanguageHighlighterBase::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN15RustHighlighterE_t {};
} // unnamed namespace

template <> constexpr inline auto RustHighlighter::qt_create_metaobjectdata<qt_meta_tag_ZN15RustHighlighterE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RustHighlighter"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<RustHighlighter, qt_meta_tag_ZN15RustHighlighterE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RustHighlighter::staticMetaObject = { {
    QMetaObject::SuperData::link<LanguageHighlighterBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15RustHighlighterE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15RustHighlighterE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15RustHighlighterE_t>.metaTypes,
    nullptr
} };

void RustHighlighter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RustHighlighter *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *RustHighlighter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RustHighlighter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15RustHighlighterE_t>.strings))
        return static_cast<void*>(this);
    return LanguageHighlighterBase::qt_metacast(_clname);
}

int RustHighlighter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = LanguageHighlighterBase::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN17PythonHighlighterE_t {};
} // unnamed namespace

template <> constexpr inline auto PythonHighlighter::qt_create_metaobjectdata<qt_meta_tag_ZN17PythonHighlighterE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PythonHighlighter"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PythonHighlighter, qt_meta_tag_ZN17PythonHighlighterE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PythonHighlighter::staticMetaObject = { {
    QMetaObject::SuperData::link<LanguageHighlighterBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17PythonHighlighterE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17PythonHighlighterE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17PythonHighlighterE_t>.metaTypes,
    nullptr
} };

void PythonHighlighter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PythonHighlighter *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *PythonHighlighter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PythonHighlighter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17PythonHighlighterE_t>.strings))
        return static_cast<void*>(this);
    return LanguageHighlighterBase::qt_metacast(_clname);
}

int PythonHighlighter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = LanguageHighlighterBase::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN14LuaHighlighterE_t {};
} // unnamed namespace

template <> constexpr inline auto LuaHighlighter::qt_create_metaobjectdata<qt_meta_tag_ZN14LuaHighlighterE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "LuaHighlighter"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<LuaHighlighter, qt_meta_tag_ZN14LuaHighlighterE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject LuaHighlighter::staticMetaObject = { {
    QMetaObject::SuperData::link<LanguageHighlighterBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14LuaHighlighterE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14LuaHighlighterE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14LuaHighlighterE_t>.metaTypes,
    nullptr
} };

void LuaHighlighter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<LuaHighlighter *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *LuaHighlighter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LuaHighlighter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14LuaHighlighterE_t>.strings))
        return static_cast<void*>(this);
    return LanguageHighlighterBase::qt_metacast(_clname);
}

int LuaHighlighter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = LanguageHighlighterBase::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN14CppHighlighterE_t {};
} // unnamed namespace

template <> constexpr inline auto CppHighlighter::qt_create_metaobjectdata<qt_meta_tag_ZN14CppHighlighterE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CppHighlighter"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CppHighlighter, qt_meta_tag_ZN14CppHighlighterE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CppHighlighter::staticMetaObject = { {
    QMetaObject::SuperData::link<LanguageHighlighterBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14CppHighlighterE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14CppHighlighterE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14CppHighlighterE_t>.metaTypes,
    nullptr
} };

void CppHighlighter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CppHighlighter *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *CppHighlighter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CppHighlighter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14CppHighlighterE_t>.strings))
        return static_cast<void*>(this);
    return LanguageHighlighterBase::qt_metacast(_clname);
}

int CppHighlighter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = LanguageHighlighterBase::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
