Name:           docsnap
Version:        1.0.3
Release:        5%{?dist}
Summary:        Webcam image capture application
License:        GPLv2+
URL:            https://github.com/Osndok/docsnap
Source0:        docsnap-%{version}.tar.gz
BuildRequires:  gcc desktop-file-utils
BuildRequires:  gettext-devel libv4l-devel gtk3-devel cairo
BuildRequires:  gdk-pixbuf2-devel
Requires:       hicolor-icon-theme

Requires: libv4l >= 8:1.16.7

%description
A simple Gnome webcam capture application designed to feed a larger image-processing workflow.
Originally a fork of the camorama application, and still *slowly* diverging therefrom.

%prep
%autosetup -p1

%build
%configure
make %{?_smp_mflags}


%install
export GCONF_DISABLE_MAKEFILE_SCHEMA_INSTALL=1
%make_install

rm -rf $RPM_BUILD_ROOT%{_datadir}/locale
#find_lang %{name}

mv $RPM_BUILD_ROOT%{_datadir}/applications/{camorama,docsnap}.desktop
mv $RPM_BUILD_ROOT%{_bindir}/{camorama,docsnap}
mv $RPM_BUILD_ROOT%{_datadir}/camorama/{camorama,docsnap}.glade
mv $RPM_BUILD_ROOT%{_datadir}/{camorama,docsnap}
#mv $RPM_BUILD_ROOT%{_datadir}/doc/{camorama,docsnap}

#mv $RPM_BUILD_ROOT%{_mandir}/man1/{camorama,docsnap}.1

# Try not to conflict with (or depend on) being installed with camorama
for FILE in $(find $RPM_BUILD_ROOT{/etc/gconf/schemas,/usr/share/pixmaps} -type f )
do
	NEXT=$(echo "$FILE" | sed -e 's/camorama/docsnap/g')
	mv $FILE $NEXT
done

# desktop-file-install --dir $RPM_BUILD_ROOT%{_datadir}/applications %{name}.desktop


%files
#doc AUTHORS ChangeLog NEWS README THANKS TODO
%license COPYING
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
##{_datadir}/icons/hicolor/*x*/devices/docsnap.png
##{_datadir}/glib-2.0/schemas/*
##{_datadir}/metainfo/docsnap.appdata.xml
##{_mandir}/man1/docsnap.1*
/etc/gconf/schemas/docsnap.schemas
/usr/share/pixmaps/docsnap*


