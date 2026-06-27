Name:           razor
Version:        1.6.1
Release:        0.1.pre0%{?dist}
Summary:        Occam compiler and runtime system
License:        GPL-2.0-or-later AND LGPL-2.0-or-later
URL:            https://github.com/jimmoores/razor
%global upstream_version %{version}-pre0
Source0:        %{name}-%{upstream_version}.tar.gz

ExclusiveArch:  x86_64 aarch64

BuildRequires:  autoconf
BuildRequires:  automake
BuildRequires:  elfutils-libelf-devel
BuildRequires:  gawk
BuildRequires:  gcc
BuildRequires:  libtool
BuildRequires:  libxslt
BuildRequires:  make
BuildRequires:  perl
BuildRequires:  pkgconf-pkg-config
BuildRequires:  python3

Requires:       bash
Requires:       binutils
Requires:       gawk
Requires:       gcc
Requires:       make
Requires:       perl
Requires:       python3

%description
Razor is a compiler, runtime, and toolset for the occam programming language.
It includes the occ21 compiler, the tranx86 native-code translator, the CCSP
runtime, occam libraries, documentation tools, and the razor and occbuild
command-line drivers.

%prep
%autosetup -n %{name}-%{upstream_version}

%build
autoreconf -vfi
%configure --with-toolchain=razor
%make_build

%install
%make_install

%check
python3 -m py_compile bin/occbuild.in

%files
%license compiler/occ21/COPYING runtime/ccsp/COPYRIGHT translator/tranx86/COPYING
%doc BUGS CHANGELOG CONTRIBUTORS.md MODULES.md README.md
%{_bindir}/*
%{_includedir}/razor/
%{_libdir}/libccsp.a
%{_libdir}/libccsp.so
%{_libdir}/libkrocif.a
%{_libdir}/libkrocif.so
%{_libdir}/liboccam_*.a
%{_libdir}/pkgconfig/ccsp-1.6.pc
%{_libdir}/razor/
%{_libdir}/rtsmain.o
%{_mandir}/man1/*
%{_mandir}/man3/*
%{_datadir}/aclocal/*.m4
%{_datadir}/razor/

%changelog
* Sat Jun 27 2026 Razor Maintainers <jimmoores@users.noreply.github.com> - 1.6.1-0.1.pre0
- Initial native Razor package metadata.
