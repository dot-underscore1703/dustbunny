Name:           dustbunny
Version:        0.1.0
Release:        1%{?dist}
Summary:        Custom Linux shell

License:        MIT
URL:            https://github.com/dot-underscore1703/dustbunny
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  gcc
BuildRequires:  make

%description
Dustbunny is a hobby shell targeted at Linux systems. It serves as an exercise for me learning the C Programming Language, and disciplining me as a programmer in general.

%prep
%autosetup

%build
%cmake
%cmake_build

%install
%cmake_install

%files
%license LICENSE
%{_bindir}/dustbunny

%changelog
* Fri Aug 21 2026 Rory Lane <roryl12@outlook.com> - 0.1.0-1
- Initial RPM package
