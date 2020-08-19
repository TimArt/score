cninja_require(lld)
cninja_require(linkerwarnings)
cninja_require(debugsyms)
cninja_require(debugsplit)
cninja_require(score-warnings)

set_cache(SCORE_PCH True)
set_cache(SCORE_DYNAMIC_PLUGINS True)

string(APPEND CMAKE_C_FLAGS_INIT " -ggnu-pubnames -fdebug-types-section")
string(APPEND CMAKE_CXX_FLAGS_INIT " -ggnu-pubnames -fdebug-types-section")
