// RUN: %clang_cc1 -std=c++2a -verify %s -pedantic

template<typename T> struct Template {};

struct Linkage1 { struct Inner {}; };
typedef struct { struct Inner {}; } Linkage2;

typedef struct {} const NoLinkage1;
auto x = [] {};
typedef decltype(x) NoLinkage2;
auto f() { return [] {}; }
typedef decltype(f()) NoLinkage3;

inline auto g() { return [] {}; }
typedef decltype(g()) VisibleNoLinkage1;
inline auto y = [] {};
typedef decltype(x) VisibleNoLinkage2;
inline auto h() { struct {} x; return x; }
typedef decltype(h()) VisibleNoLinkage3;

extern Linkage1 linkage1v;
extern Linkage1::Inner linkage1iv;
extern Linkage2 linkage2v;
extern Linkage2::Inner linkage2iv;
extern Template<Linkage1> linkaget1v;
extern Linkage1 linkage1f();
void linkage2f(Linkage2);

void use_linkage() {
  &linkage1v, &linkage1iv, &linkage2v, &linkage2iv, &linkaget1v; // expected-warning 5{{unused}}
  linkage1f();
  linkage2f({});
}

extern NoLinkage1 no_linkage1(); // expected-error {{function 'no_linkage1' is used but not defined in this translation unit}}
extern NoLinkage2 no_linkage2(); // expected-error {{function 'no_linkage2' is used but not defined in this translation unit}}
extern NoLinkage3 no_linkage3(); // expected-error {{function 'no_linkage3' is used but not defined in this translation unit}}

void use_no_linkage() {
  no_linkage1(); // expected-note {{used here}}
  no_linkage2(); // expected-note {{used here}}
  no_linkage3(); // expected-note {{used here}}
}

// FIXME: This should emit an extension warning. It does not because we
// incorrectly give the lambda external linkage.
extern VisibleNoLinkage1 visible_no_linkage1();

// FIXME: We should accept this as an extension. We don't because we
// incorrectly give the lambda no linkage instead of "VisibleNoLinkage".
extern VisibleNoLinkage2 visible_no_linkage2(); // expected-error {{used but not defined}}

// This case is correctly accepted as an extension.
extern VisibleNoLinkage3 visible_no_linkage3(); // expected-warning {{ISO C++ requires a definition}}

void use_visible_no_linkage() {
  visible_no_linkage1();
  visible_no_linkage2(); // expected-note {{used here}}
  visible_no_linkage3(); // expected-note {{used here}}
}


extern inline int not_defined; // expected-error {{not defined}}
extern inline int defined_after_use;
void use_inline_vars() {
  not_defined = 1; // expected-note {{used here}}
  defined_after_use = 2;
}
inline int defined_after_use;
