#ifndef WRAPPER_H
#define WRAPPER_H
#if QT_VERSION < 0x050700
#if __cplusplus > 201703L
#define qAsConst(x) std::as_const(x)
#else
#define qAsConst
#endif
#endif
#endif