#ifndef COLORS_H
#define COLORS_H

// Dark amber theme — оранжевый + жёлтый на тёмном фоне
namespace Colors {

// -- Main UI Backgrounds --
inline constexpr const char* bgMain       = "#1a1a1e";
inline constexpr const char* bgPanel      = "#242428";
inline constexpr const char* bgWidget     = "#2d2d32";
inline constexpr const char* bgHover      = "#35353a";
inline constexpr const char* bgSelected   = "#3d3d42";
inline constexpr const char* bgCurrentLine= "#28282d";
inline constexpr const char* bgBracket    = "#35353a";

// -- Text Colors --
inline constexpr const char* textPrimary   = "#d4d4d4";
inline constexpr const char* textSecondary = "#9e9e9e";
inline constexpr const char* textMuted     = "#6e6e6e";
inline constexpr const char* textAccent    = "#f0c060";
inline constexpr const char* textSelected  = "#ffffff";
inline constexpr const char* textInverse   = "#1a1a1e";

// -- Borders --
inline constexpr const char* borderMain    = "#3d3d42";
inline constexpr const char* borderLight   = "#35353a";
inline constexpr const char* borderFocus   = "#e6a341";

// -- Accent (orange) --
inline constexpr const char* accent        = "#e6a341";
inline constexpr const char* accentHover   = "#f0b84d";
inline constexpr const char* accentActive  = "#d4952a";

// -- Yellow accent --
inline constexpr const char* yellow        = "#f0c060";
inline constexpr const char* yellowHover   = "#f5d080";

// -- Scrollbar --
inline constexpr const char* scrollBg      = "#242428";
inline constexpr const char* scrollHandle  = "#3d3d42";
inline constexpr const char* scrollHandleHover = "#e6a341";

// -- Diagnostic Status --
inline constexpr const char* diagError     = "#e66a41";
inline constexpr const char* diagWarning   = "#e6c341";
inline constexpr const char* diagInfo      = "#9e9e9e";
inline constexpr const char* diagErrorBg   = "#3d2828";
inline constexpr const char* diagWarningBg = "#3d3828";

// -- Syntax Highlighting --
inline constexpr const char* synKeyword    = "#f0c060";
inline constexpr const char* synType       = "#e6a341";
inline constexpr const char* synString     = "#8cc46a";
inline constexpr const char* synNumber     = "#e69341";
inline constexpr const char* synFunction   = "#e6a341";
inline constexpr const char* synComment    = "#6e6e6e";
inline constexpr const char* synBuiltin    = "#f0c060";
inline constexpr const char* synTag        = "#e66a41";
inline constexpr const char* synAttribute  = "#e6c341";
inline constexpr const char* synPunct      = "#9e9e9e";
inline constexpr const char* synProperty   = "#6ab48a";
inline constexpr const char* synPreproc    = "#e69341";
inline constexpr const char* synBracketFg  = "#e6a341";

// -- Gutter --
inline constexpr const char* gutterBg      = "#242428";
inline constexpr const char* gutterText    = "#9e9e9e";
inline constexpr const char* foldIcon      = "#6e6e6e";
inline constexpr const char* gutterBorder  = "#35353a";

// -- Selection --
inline constexpr const char* selBg         = "#3d3d42";
inline constexpr const char* selFg         = "#ffffff";

// -- File Icons --
inline constexpr const char* fiEltr        = "#f0c060";
inline constexpr const char* fiCpp         = "#e6a341";
inline constexpr const char* fiC           = "#e6a341";
inline constexpr const char* fiRust        = "#e66a41";
inline constexpr const char* fiPython      = "#e6c341";
inline constexpr const char* fiJs          = "#e6a341";
inline constexpr const char* fiHtml        = "#e66a41";
inline constexpr const char* fiCss         = "#e6a341";
inline constexpr const char* fiJson        = "#8cc46a";
inline constexpr const char* fiMd          = "#9e9e9e";
inline constexpr const char* fiLua         = "#5ea8c4";
inline constexpr const char* fiToml        = "#e6a341";
inline constexpr const char* fiYaml        = "#6ab48a";
inline constexpr const char* fiMakefile    = "#e6c341";
inline constexpr const char* fiDockerfile  = "#3d8ec4";
inline constexpr const char* fiDefault     = "#9e9e9e";

}

#endif // COLORS_H
