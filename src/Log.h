#pragma once

#define _BL_EXPAND(x) x
#define _BL_VARGS(_9, _8, _7, _6, _5, _4, _3, _2, _1, N, ...) N

// ----- Logging -----
#if BL_ENABLE_LOGGING

#define _BL_LOG1(format)                                  \
do                                                        \
{                                                         \
	fprintf(stdout, "[INFO] %s:%d ", __FILE__, __LINE__); \
	fprintf(stdout, format);                              \
	fprintf(stdout, "\n");                                \
} while(0)

#define _BL_LOG2(format, ...)                             \
do                                                        \
{                                                         \
	fprintf(stdout, "[INFO] %s:%d ", __FILE__, __LINE__); \
	fprintf(stdout, format, __VA_ARGS__);                 \
	fprintf(stdout, "\n");                                \
} while(0)

#define _BL_LOG_CHOOSER(...) _BL_EXPAND( \
_BL_VARGS(__VA_ARGS__,                   \
_BL_LOG2, _BL_LOG2, _BL_LOG2,            \
_BL_LOG2, _BL_LOG2, _BL_LOG2,            \
_BL_LOG2, _BL_LOG2, _BL_LOG1)            \
)

#define BL_LOG(...) _BL_EXPAND(_BL_LOG_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

#else

#define BL_LOG(...)

#endif // BL_LOGGING

// --- Assertions ---
#if BL_ENABLE_ASSERTIONS

#define _BL_ASSERT1(condition)                                 \
do                                                             \
{                                                              \
	if (!(condition))                                          \
	{                                                          \
		fprintf(stdout, "[ERROR] %s:%d ", __FILE__, __LINE__); \
		fprintf(stdout, "Assertion failed!");                  \
		fprintf(stdout, "\n");                                 \
	}                                                          \
} while(0)

#define _BL_ASSERT2(condition, format)                         \
do                                                             \
{                                                              \
	if (!(condition))                                          \
	{                                                          \
		fprintf(stdout, "[ERROR] %s:%d ", __FILE__, __LINE__); \
		fprintf(stdout, "Assertion failed! ");                 \
		fprintf(stdout, format);                               \
		fprintf(stdout, "\n");                                 \
	}                                                          \
} while (0)

#define _BL_ASSERT3(condition, format, ...)                    \
do                                                             \
{                                                              \
	if (!(condition))                                          \
	{                                                          \
		fprintf(stdout, "[ERROR] %s:%d ", __FILE__, __LINE__); \
		fprintf(stdout, "Assertion failed! ");                 \
		fprintf(stdout, format, __VA_ARGS__);                  \
		fprintf(stdout, "\n");                                 \
	}                                                          \
} while (0)

#define _BL_ASSERT_CHOOSER(...) _BL_EXPAND( \
_BL_VARGS(__VA_ARGS__,                      \
_BL_ASSERT3, _BL_ASSERT3, _BL_ASSERT3,      \
_BL_ASSERT3, _BL_ASSERT3, _BL_ASSERT3,      \
_BL_ASSERT3, _BL_ASSERT2, _BL_ASSERT1)      \
)

#define BL_ASSERT(...) _BL_EXPAND(_BL_ASSERT_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

#else

// We still want to execute the condition in case we rely on it !

#define _BL_ASSERT1(condition)              \
do                                          \
{                                           \
	if ((condition)) {}                     \
} while (0)

#define _BL_ASSERT2(condition, format)      \
do                                          \
{                                           \
	if ((condition)) {}                     \
} while (0)

#define _BL_ASSERT3(condition, format, ...) \
do                                          \
{                                           \
	if ((condition)) {}                     \
} while (0)

#define _BL_ASSERT_CHOOSER(...) _BL_EXPAND( \
_BL_VARGS(__VA_ARGS__,                      \
_BL_ASSERT3, _BL_ASSERT3, _BL_ASSERT3,      \
_BL_ASSERT3, _BL_ASSERT3, _BL_ASSERT3,      \
_BL_ASSERT3, _BL_ASSERT2, _BL_ASSERT1)      \
)

#define BL_ASSERT(...) _BL_EXPAND(_BL_ASSERT_CHOOSER(__VA_ARGS__)(__VA_ARGS__))

#endif  // BL_ASSERTION
