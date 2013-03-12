#sbs-git:slp/api/sim capi-telephony-sim 0.1.0 338620cf30c11d71e7f17c6cad5968ac57d724d4
Name:       capi-telephony-sim
Summary:    Telephony SIM Framework
Version: 0.1.7
Release:    1
Group:      TO_BE/FILLED_IN
License:    TO BE FILLED IN
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(tapi)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(capi-base-common)
Requires(post): /sbin/ldconfig  
Requires(postun): /sbin/ldconfig

%description


%package devel
Summary:  Telephony SIM Framework (Development)
Group:    TO_BE/FILLED_IN
Requires: %{name} = %{version}-%{release}

%description devel



%prep
%setup -q


%build
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DFULLVER=%{version} -DMAJORVER=${MAJORVER}


make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%manifest capi-telephony-sim.manifest
%{_libdir}/libcapi-telephony-sim.so.*

%files devel
%{_includedir}/telephony/sim.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-telephony-sim.so


