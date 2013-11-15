Name:       capi-telephony-sim
Summary:    Telephony SIM Framework
Version: 0.1.7
Release:    1
Group:      Telephony/API
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001: 	capi-telephony-sim.manifest
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(tapi)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(capi-base-common)

%description
Telephony SIM Framework.


%package devel
Summary:  Telephony SIM Framework (Development)
Group:    Development/Telephony
Requires: %{name} = %{version}-%{release}

%description devel
%devel_desc



%prep
%setup -q
cp %{SOURCE1001} .


%build
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . -DFULLVER=%{version} -DMAJORVER=${MAJORVER}


make %{?jobs:-j%jobs}

%install
%make_install
mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/%{name}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%manifest %{name}.manifest
%license LICENSE
%manifest capi-telephony-sim.manifest
%{_libdir}/libcapi-telephony-sim.so.*
/usr/share/license/%{name}

%files devel
%manifest %{name}.manifest
%{_includedir}/telephony/sim.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-telephony-sim.so


