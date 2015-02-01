Name: webkit2-efl
Summary: Webkit2 EFL
Version: 152340_0.10.252.0
Release: 1
Group: Applications/Internet
License: LGPLv2.1 or BSD
Source0: %{name}-%{version}.tar.gz

%{?TIZEN_PROFILE_TV:%define _profile TV}
%if "%{?tizen_profile_name}" == "wearable"
    %define _profile WEARABLE
%endif
%if "%{?tizen_profile_name}" == "mobile"
    %define _profile MOBILE
%endif
%{?model_build_feature_location_position:%define _geolocation ON}

Requires(post): /sbin/ldconfig
Requires(post): xkeyboard-config
Requires(postun): /sbin/ldconfig

BuildRequires: cmake, edje-bin, embryo-bin, python, bison, flex, gperf, libjpeg-turbo-devel, perl, gettext, ruby, libcap, libcap-devel

BuildRequires: pkgconfig(cairo)
BuildRequires: pkgconfig(deviced)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(ecore-evas)
BuildRequires: pkgconfig(ecore-file)
BuildRequires: pkgconfig(edbus)
BuildRequires: pkgconfig(edje)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(feedback)
BuildRequires: pkgconfig(fontconfig)
BuildRequires: pkgconfig(freetype2)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(gstreamer-0.10)
BuildRequires: pkgconfig(gstreamer-plugins-base-0.10)
BuildRequires: pkgconfig(harfbuzz)
BuildRequires: pkgconfig(icu-i18n)
BuildRequires: pkgconfig(libmedia-utils)
BuildRequires: pkgconfig(libsoup-2.4)
BuildRequires: pkgconfig(openssl)
BuildRequires: pkgconfig(libxml-2.0)
BuildRequires: pkgconfig(libxslt)
BuildRequires: pkgconfig(mmutil-imgp)
BuildRequires: pkgconfig(mmutil-jpeg)
BuildRequires: pkgconfig(mm-session)
BuildRequires: pkgconfig(mm-sound)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: pkgconfig(utilX)
BuildRequires: pkgconfig(vconf)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(leveldb)
BuildRequires: pkgconfig(capi-system-sensor)
BuildRequires: pkgconfig(xext)
BuildRequires: pkgconfig(libavcodec)
BuildRequires: pkgconfig(gles20)
BuildRequires: pkgconfig(capi-system-device)
BuildRequires: pkgconfig(capi-system-info)
BuildRequires: pkgconfig(libdri2)
BuildRequires: pkgconfig(libdrm)
BuildRequires: pkgconfig(libtbm)
BuildRequires: pkgconfig(x11)
BuildRequires: pkgconfig(xfixes)
BuildRequires: pkgconfig(efl-assist)
BuildRequires: pkgconfig(capi-media-camera)
BuildRequires: pkgconfig(libsmack)

%if "%{?tizen_profile_name}" == "wearable"
BuildRequires: pkgconfig(tts)
BuildRequires: pkgconfig(stt)
%if 0%{?accessibility}
BuildRequires: pkgconfig(eail)
BuildRequires: pkgconfig(atk)
%endif
%if "%{_geolocation}" == "ON"
BuildRequires: pkgconfig(capi-location-manager)
%endif
%endif

%if "%{_profile}" == "MOBILE" || "%{_profile}" == "TV"
BuildRequires: pkgconfig(capi-location-manager)
BuildRequires: pkgconfig(capi-network-connection)
BuildRequires: pkgconfig(location)
BuildRequires: pkgconfig(tts)
BuildRequires: pkgconfig(ui-gadget-1)
BuildRequires: pkgconfig(xt)
%endif

%if "%{_profile}" == "MOBILE"
BuildRequires: pkgconfig(capi-telephony-network-info)
%endif

%if "%{_profile}" == "TV"
BuildRequires: pkgconfig(stt)
BuildRequires: pkgconfig(eeze)
%endif

%description
Browser Engine based on Webkit2 EFL (Shared Library)

%package devel
Summary: Webkit2 EFL
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
Browser Engine dev library based on Webkit EFL (Developement files)

%prep
%setup -q

%build

%{!?enable_api_test:%define enable_api_test ON}
%{!?enable_tools:%define enable_tools ON}

%define _build_type Release

%ifarch %{arm}
    %define _build_target arm
    %define _emulator OFF
%else
    export CFLAGS="$(echo $CFLAGS | sed 's/-Wl,--as-needed//g')"
    export CXXFLAGS="$(echo $CXXFLAGS | sed 's/-Wl,--as-needed//g')"

    %if "%{_repository}" == "emulator"
        %define _build_target emulator
        %define _emulator ON
    %else
        %define _build_target i386
        %define _emulator OFF
    %endif
%endif

%if 0%{?nodebug}
    export CFLAGS=$(echo $CFLAGS | sed 's/ -g / /')
    export CXXFLAGS=$(echo $CXXFLAGS | sed 's/ -g / /')
%endif

export SOURCE_DIR="$PWD"

%if 0%{?_webkit_build_dir}
    %define _build_dir %{_webkit_build_dir}
%else
    %define _build_dir BuildWK2-%{_profile}-%{_build_target}
%endif

mkdir -p %{_build_dir}
cd %{_build_dir}

%if 0%{?_tizen_layout_test}
    %define _shared_core ON
%else
    %define _shared_core OFF
%endif

cmake $SOURCE_DIR \
    -DPORT=Efl \
    -DPROJECT_VERSION=%{version} \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    -DCMAKE_BUILD_TYPE=%{?_build_type:%_build_type} \
    -DENABLE_TIZEN_SUPPORT=ON \
    -DENABLE_WEBKIT=OFF \
    -DENABLE_WEBKIT2=ON \
    -DENABLE_TIZEN_ALL_IN_ONE=ON \
    -DENABLE_EGL=ON \
    -DSHARED_CORE=%{_shared_core} \
    -DENABLE_GLES2=ON \
    -DTIZEN_PROFILE=%{_profile} \
    -DENABLE_TIZEN_EMULATOR=%{_emulator} \
    -DENABLE_API_TESTS=%{enable_api_test} \
    -DENABLE_TOOLS=%{enable_tools} \
    %{?_geolocation:-DENABLE_GEOLOCATION=%_geolocation} \
    %{?INSTALL_MINIBROWSER:-DINSTALL_MINIBROWSER=%INSTALL_MINIBROWSER} \
    %{?_webkit_cmake_options:%_webkit_cmake_options}
make %{?_make_option:%_make_option} %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/%{name}

cd %{_build_dir}
%make_install
mkdir -p %{buildroot}/usr/share/edje

%post
#!/bin/sh
#change file permissions
# 1. libraries
chmod 644 %{_libdir}/libewebkit2.so
# 2. Resources
chmod 644 %{_datadir}/edje/webkit.edj
chmod 644 %{_datadir}/edje/legacyWebkit.edj
# 3. executables
setfattr -n security.capability -v 0sAQAAAgAAAAAAAAAAAAAAAAIAAAA= %{_bindir}/WebProcess 

%postun

%files
%manifest %{name}.manifest
/usr/share/license/%{name}
%defattr(-,root,root,-)
%{_libdir}/*.so*
%{_bindir}/*
%{_datadir}/*

%files devel
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/pkgconfig/*
